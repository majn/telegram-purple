#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#include "telegram.h"
#include "msglog.h"
#include "glib.h"
#include "tools.h"
#include "mtproto-client.h"
#include "binlog.h"
#include "loop.h"



/*
 * New message received
 */
void event_update_new_message(struct telegram *instance, struct message *M) 
{
    if (instance->config->on_msg_handler) {
        instance->config->on_msg_handler (instance, M);
    }
}

/*
 * Peer allocated
 */
void event_peer_allocated(struct telegram *instance, void *peer) 
{
    if (instance->config->on_peer_allocated_handler) {
        instance->config->on_peer_allocated_handler (instance, peer);
    }
}

/*
 * Peer user fetched full
 */
void event_user_info_received_handler(struct telegram *instance, struct tgl_user *peer, int show_info)
{
    if (instance->config->on_user_info_received_handler) {
        instance->config->on_user_info_received_handler (instance, peer, show_info);
    }
}

/*
 * Download finished
 */
void event_download_finished_handler(struct telegram *instance, struct download *D) 
{
    if (instance->config->on_download_finished_handler) {
        instance->config->on_download_finished_handler (instance, D);
    }
}

/*
 * User status changed
 */
void event_update_user_status (struct telegram *instance, void *peer) 
{
    if (instance->config->on_update_user_status_handler) {
        instance->config->on_update_user_status_handler (instance, peer);
    }
}

/*
 * User typing changed
 */
void event_update_user_typing (struct telegram *instance, void *peer) 
{
    if (instance->config->on_update_uesr_typing_handler) {
        instance->config->on_update_uesr_typing_handler (instance, peer);
    }
}



/**
 * Calculate the configuration path for the given config file and the given instance
 *
 * @returns The full path to the configuration. 
 *
 * NOTE: the returned string must be freed manually using gfree
 */
char *telegram_get_config(struct telegram *instance, char *config)
{
    return g_strdup_printf("%s/%s", instance->config_path, config);
}

/**
 * Return whether the current client is registered.
 */
int telegram_is_registered(struct telegram *tg) {
    return telegram_get_working_dc(tg)->has_auth;
}


/**
 * Handle state changes of the telegram instance
 *
 * Execute all actions necessary when a certain state is reached. The state machine executes
 * the authorization and registration steps needed to connect the client to the telegram network, 
 * and will either trigger RPC queries or callbacks to the GUI to request input from the user.
 */
void telegram_change_state (struct telegram *instance, int state, void *data) 
{
    instance->session_state = state;
    debug("on_state_change: %d\n", state);
    switch (state) {
        case STATE_ERROR: {
            const char* err = data;
            if (err == NULL) {
                err = "<no description>";
            }
            debug("telegram errored: %s\n", err);
            mtproto_close (instance->connection);
        }
        break;

        case STATE_AUTHORIZED:
            debug("requesting configuration\n");
            telegram_change_state(instance, STATE_CONFIG_REQUESTED, NULL);
            if (telegram_is_registered(instance)) {
                telegram_change_state (instance, STATE_READY, NULL);
                return;
            }
            do_help_get_config (instance);
        break;

        case STATE_CONFIG_RECEIVED:
            debug("received network configuration, checking whether phone is registered.\n");
            telegram_store_session(instance);
            do_auth_check_phone(instance, instance->login);
        break;

        case STATE_PHONE_NOT_REGISTERED:
            debug("phone is not registered, need to register phone number.\n");
            do_send_code(instance, instance->login);
        break;

        case STATE_PHONE_CODE_NOT_ENTERED:
            debug("phone authenticion, user needs to enter code, first and last name.\n");
            assert (instance->config->on_phone_registration_required);
            instance->config->on_phone_registration_required (instance);
            break;

        case STATE_CLIENT_NOT_REGISTERED:
            debug("phone is already registered, need to register client.\n");
            do_send_code(instance, instance->login);
        break;

        case STATE_CLIENT_CODE_NOT_ENTERED:
            debug("client authentication, user needs to enter code.\n");
            assert (instance->config->on_client_registration_required);
            instance->config->on_client_registration_required (instance);
            // wait for user input ...
        break;
        
        case STATE_READY:
            debug("telegram is registered and ready.\n");
            telegram_store_session (instance);
            instance->config->on_ready (instance);
        break;

        case STATE_DISCONNECTED_SWITCH_DC: {
            // telegram demands that we use a different data center, which caused
            // the current mtproto_connection to be disconnected
            
            int target_dc = *(int*) data;
            debug ("Disconnected: Migrate to data center %d\n", target_dc);

            // close old connection and mark it for destruction
            mtproto_close (instance->connection);
            assert (instance->config->proxy_request_cb);

            // remove all left over queries and timers
            free_timers (instance);
            free_queries (instance);

            // start a new connection to the demanded data center. The pointer to the
            // new dc should was already updated by the on_error function of the query
            telegram_connect (instance);
        }
        break;
    }
}

struct telegram *telegram_new(const char* login, struct telegram_config *config)
{
    struct telegram *this = talloc0(sizeof(struct telegram));
    this->protocol_data = NULL;
    this->bl = talloc0 (sizeof(struct binlog));
    this->config = config;

    this->login = g_strdup(login);
    this->config_path = g_strdup_printf("%s/%s", config->base_config_path, login);
    this->download_path = telegram_get_config(this, "downloads");
    this->auth_path = telegram_get_config(this, "auth");
    this->state_path = telegram_get_config(this, "state");
    this->secret_path = telegram_get_config(this, "secret");
    
    debug("%s\n", this->login);
    debug("%s\n", this->config_path);
    debug("%s\n", this->download_path);
    debug("%s\n", this->auth_path);
    debug("%s\n", this->state_path);
    debug("%s\n", this->secret_path);

    telegram_change_state(this, STATE_INITIALISED, NULL);
    return this;
}

void free_bl (struct binlog *bl);
void free_auth (struct dc* DC_list[], int count);
void telegram_destroy(struct telegram *this) 
{
    // close all open connections
    int i = 0; 
    for (; i < 100; i++) {
      if (this->Cs[i] != NULL && !this->Cs[i]->destroy) {
        mtproto_close (this->Cs[i]);
      }
    }
    free_queries (this);
    free_timers (this);
    mtproto_free_closed (this);

    free_bl (this->bl);
    free_auth (this->auth.DC_list, 11);

    g_free(this->login);
    g_free(this->config_path);
    g_free(this->download_path);
    g_free(this->auth_path);
    g_free(this->state_path);
    g_free(this->secret_path);
   
    // TODO: BN_CTX *ctx
    free (this->phone_code_hash);
    free (this->suser);
    free (this->export_auth_str);
    //tfree (this->ML, sizeof(struct message) * MSG_STORE_SIZE);
    tfree(this, sizeof(struct telegram));
}

void free_bl (struct binlog *bl)
{
    // TODO: rptr, wptr
    free_peers (bl);
    free_messages (bl);
    tfree (bl, sizeof (struct binlog));
}

void free_auth (struct dc* DC_list[], int count) 
{
    int i;
    for (i = 0; i < count; i++ ) if (DC_list[i]) {
        tfree (DC_list[i], sizeof(struct dc));
    }
}

void assert_file_usable(const char *file)
{
    debug ("assert_file_usable (%s)\n", file);
    assert(access(file, W_OK | R_OK | F_OK) != -1);
}

void assure_file_exists(const char *dir, const char *file)
{
    g_mkdir_with_parents(dir, 0700);
    char *f = g_strdup_printf("%s/%s", dir, file);
    close(open(f, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR));
    assert_file_usable(f);
    g_free(f);
}

/**
 * Get the currently used connection
 *
 * Will only work after telegram_connect has been called and the connection has
 * not errored.
 */
struct connection *telegram_get_connection(struct telegram *instance)
{
    assert(instance->session_state != STATE_ERROR);

    struct dc *DC = telegram_get_working_dc(instance);
    assert(DC);
    assert(DC->sessions[0]);
    assert(DC->sessions[0]->c);
    return DC->sessions[0]->c;
}

/**
 * Get the currently used DC
 *
 * Will only work after restore_session has been called and the data center configuration 
 * was properly loaded
 */
struct dc *telegram_get_working_dc(struct telegram *instance)
{
    assert(instance->session_state != STATE_ERROR);

    assert(instance->auth.DC_list);
    assert(instance->auth.dc_working_num > 0);
    struct dc *DC = instance->auth.DC_list[instance->auth.dc_working_num];
    return DC;
}

/**
 * Store the current session state to a file
 */
void telegram_restore_session(struct telegram *instance)
{
    g_mkdir_with_parents(instance->config_path, 0700);
    g_mkdir_with_parents(instance->download_path, 0700);
    instance->auth = read_auth_file(instance->auth_path);
    instance->proto = read_state_file(instance->state_path);
    read_secret_chat_file (instance, instance->secret_path);
}

/**
 * Load the current session state
 */
void telegram_store_session(struct telegram *instance)
{
    assure_file_exists(instance->config_path, "auth");
    assure_file_exists(instance->config_path, "state");
    assure_file_exists(instance->config_path, "secret");
    write_auth_file(&instance->auth, instance->auth_path);
    write_state_file(&instance->proto, instance->state_path);
    write_secret_chat_file(instance, instance->secret_path);
}

void on_authorized(struct mtproto_connection *c, void* data);

void telegram_main_connected (struct proxy_request *req)
{
    struct telegram *instance = req->data;
    debug("Authorized... storing current session %d.\n", 
        instance->connection->connection->session[0]);
    telegram_store_session(instance);
    telegram_change_state(instance, STATE_AUTHORIZED, NULL);
}

/**
 * Connect to the nearest data center
 */
void telegram_connect (struct telegram *instance)
{
    debug ("telegram_network_connect()\n");
    if (! instance->auth.DC_list) {
       debug("telegram_network_connect(): cannot connect, restore / init a session first.\n");
       assert(0);
    }
    struct dc *DC_working = telegram_get_working_dc (instance);

    struct proxy_request *req = talloc0(sizeof(struct proxy_request));
    req->type = REQ_CONNECTION;
    req->tg = instance;
    req->data = instance;
    req->DC = DC_working;
    req->done = telegram_main_connected; 

    assert (instance->config->proxy_request_cb);
    instance->config->proxy_request_cb (instance, req);
}

void on_auth_imported (void *extra)
{
   debug ("on_auth_imported()\n");
   struct download *dl = extra;
   struct mtproto_connection *c = dl->c;
   struct telegram *tg = c->instance;
   bl_do_dc_signed (tg->bl, c, dl->dc);
   write_auth_file (&tg->auth, tg->auth_path);
   load_next_part (tg, dl);
   telegram_flush (tg);
}

void on_auth_exported (char *export_auth_str UU, int len UU, void *extra)
{
   debug ("on_auth_exported()\n");
   struct download *dl = extra;
   do_import_auth (dl->c->instance, dl->dc, on_auth_imported, extra);
   telegram_flush (dl->c->instance);
}

void telegram_dl_connected (struct proxy_request *req)
{
   debug ("telegram_dl_connected(dc=%d)\n", req->DC->id);
   struct telegram *tg = req->tg;
   // TODO: error handling

   struct download *dl = req->data;
   dl->c = req->conn;
   struct dc *DC = tg->auth.DC_list[dl->dc];
   if (!DC->has_auth) {
      do_export_auth (tg, dl->dc, on_auth_exported, dl);
      telegram_flush (tg);
   } else {
      on_auth_imported (dl);
   }
}


/**
 * Create a connection for the given download
 */
void telegram_dl_add (struct telegram *instance, struct download *dl)
{
    debug ("telegram_connect_dl(dc_num=%d, dc=%d)\n", dl->dc, instance->auth.DC_list[dl->dc]);
    if (!instance->dl_queue) {
        instance->dl_queue = g_queue_new ();
    }
    g_queue_push_tail(instance->dl_queue, dl);
}

void telegram_dl_next (struct telegram *instance)
{
    assert (instance->dl_queue);
    assert (instance->config->proxy_request_cb);
    if (!instance->dl_curr) {
        struct download *dl = g_queue_pop_head (instance->dl_queue);
        if (dl) {
            struct proxy_request *req = talloc0(sizeof(struct proxy_request));
            req->type = REQ_DOWNLOAD;
            req->DC = instance->auth.DC_list[dl->dc];
            req->tg = instance;
            req->done = telegram_dl_connected;
            req->data = dl;
            instance->dl_curr = dl;

            debug ("telegrma_dl_start(workin_dc=%d, ): starting new download..\n", instance->auth.dc_working_num);
            if (dl->dc == instance->auth.dc_working_num) {
                debug ("is working DC, start download...\n");
                assert (telegram_get_working_dc(instance)->sessions[0]->c);
                req->conn = instance->connection;
                dl->c = req->conn;
                telegram_dl_connected (req);
            }  else {
                debug ("is remote DC, requesting connection...\n");
                instance->config->proxy_request_cb (instance, req);
            }
        } else {
            debug ("telegrma_dl_start(): no more downloads, DONE!\n");
            mtproto_close_foreign (instance);
        }
    } else {
        debug ("telegrma_dl_start(): download busy...\n");
    }
}

/**
 * Login, and perform a registration when needed
 */
int telegram_login(struct telegram *instance)
{
    if (instance->session_state != STATE_AUTHORIZED) {
        debug("Cannot log in, invalid state: %d \n", instance->session_state);
        return -1;
    }
    do_help_get_config(instance);
    telegram_change_state(instance, STATE_CONFIG_REQUESTED, NULL);
    return 0;
}

void on_authorized(struct mtproto_connection *c UU, void *data)
{
    debug ("on_authorized()...\n");
    struct proxy_request *req = data;
    assert (req->done);
    req->done (req);
    tfree (req, sizeof(struct proxy_request));
}

struct mtproto_connection *telegram_add_proxy(struct telegram *instance, struct proxy_request *req,
    int fd, void *handle)
{
    struct mtproto_connection *c = mtproto_new (req->DC, fd, instance);
    c->handle = handle;
    c->on_ready = on_authorized;
    c->on_ready_data = req;
    req->conn = c;
    if (req->type == REQ_CONNECTION) {
        req->tg->connection = c;
    }
    mtproto_connect (c);
    return c;
}

void mtp_read_input (struct mtproto_connection *mtp)
{
    try_read (mtp->connection);
}

int mtp_write_output (struct mtproto_connection *mtp)
{
    return try_write(mtp->connection);
}

int telegram_authenticated (struct telegram *instance)
{
    return telegram_get_working_dc (instance)->auth_key_id > 0;
}

void telegram_flush (struct telegram *instance)
{
    debug ("telegram flush()\n");
    int i;
    for (i = 0; i < 100; i++) {
        struct mtproto_connection *c = instance->Cs[i];
        if (!c) continue;
        if (!c->connection) continue;
        if (c->connection->out_bytes) {
            debug ("connection %d has %d bytes, triggering on_output.\n", 
                i, c->connection->out_bytes);
            instance->config->on_output(c->handle);
        } else {
            debug ("connection %d has no bytes, skipping\n", i);
        }
    }
}

