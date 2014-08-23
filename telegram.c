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
    return g_strdup_printf("%s/%s", instance->config->base_config_path, config);
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
    logprintf("on_state_change: %d\n", state);
    switch (state) {
        case STATE_ERROR: {
            const char* err = data;
            if (err == NULL) {
                err = "<no description>";
            }
            logprintf("telegram errored: %s\n", err);

            // close the connection
            mtproto_close (instance->connection);
        }
        break;

        case STATE_AUTHORIZED:
            logprintf("requesting configuration\n");
            telegram_change_state(instance, STATE_CONFIG_REQUESTED, NULL);
            if (telegram_is_registered(instance)) {
                telegram_change_state (instance, STATE_READY, NULL);
                return;
            }
            do_help_get_config (instance);
        break;

        case STATE_CONFIG_RECEIVED:
            logprintf("received network configuration, checking whether phone is registered.\n");
            telegram_store_session(instance);
            do_auth_check_phone(instance, instance->login);
        break;

        case STATE_PHONE_NOT_REGISTERED:
            logprintf("phone is not registered, need to register phone number.\n");
            do_send_code(instance, instance->login);
        break;

        case STATE_PHONE_CODE_NOT_ENTERED:
            logprintf("phone authenticion, user needs to enter code, first and last name.\n");
            assert (instance->config->on_phone_registration_required);
            instance->config->on_phone_registration_required (instance);
            // wait for user input ...
            break;

        case STATE_CLIENT_NOT_REGISTERED:
            logprintf("phone is already registered, need to register client.\n");
            do_send_code(instance, instance->login);
        break;

        case STATE_CLIENT_CODE_NOT_ENTERED:
            logprintf("client authentication, user needs to enter code.\n");
            assert (instance->config->on_client_registration_required);
            instance->config->on_client_registration_required (instance);
            // wait for user input ...
        break;
        
        case STATE_READY:
            logprintf("telegram is registered and ready.\n");
            telegram_store_session (instance);
            instance->config->on_ready (instance);
        break;

        case STATE_DISCONNECTED_SWITCH_DC: {
            // telegram demands that we use a different data center, which caused
            // the current mtproto_connection to be disconnected
            
            int target_dc = *(int*) data;
            logprintf ("Disconnected: Migrate to data center %d\n", target_dc);

            // close old connection and mark it for destruction
            mtproto_close (instance->connection);
            assert (instance->config->proxy_request_cb);
            // tell the proxy to close all connections
            instance->config->proxy_close_cb (instance, instance->connection->connection->fd);
            
            // start a new connection to the demanded data center. The pointer to the
            // currently working dc should have already been updated by the 
            // on_error function of the query
            telegram_network_connect (instance);
        }
        break;
    }
}

struct telegram *telegram_new(struct dc *DC, const char* login, struct telegram_config *config)
{
    struct telegram *this = talloc0(sizeof(struct telegram));
    this->protocol_data = NULL;
    this->auth.DC_list[0] = DC;
    this->bl = talloc0 (sizeof(struct binlog));
    this->config = config;

    this->login = g_strdup(login);
    this->config_path = g_strdup_printf("%s/%s", config->base_config_path, login);
    this->download_path = telegram_get_config(this, "downloads");
    this->auth_path = telegram_get_config(this, "auth");
    this->state_path = telegram_get_config(this, "state");
    this->secret_path = telegram_get_config(this, "secret");
    
    logprintf("%s\n", this->login);
    logprintf("%s\n", this->config_path);
    logprintf("%s\n", this->download_path);
    logprintf("%s\n", this->auth_path);
    logprintf("%s\n", this->state_path);
    logprintf("%s\n", this->secret_path);

    telegram_change_state(this, STATE_INITIALISED, NULL);
    return this;
}

void telegram_free(struct telegram *this) 
{
    g_free(this->login);
    g_free(this->config_path);
    g_free(this->download_path);
    g_free(this->auth_path);
    g_free(this->state_path);
    g_free(this->secret_path);
    tfree(this->bl, sizeof(struct binlog));
    tfree(this, sizeof(struct telegram));
}

void assert_file_usable(const char *file)
{
    logprintf ("assert_file_usable (%s)\n", file);
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
    instance->auth = read_auth_file(instance->auth_path);
    instance->proto = read_state_file(instance->state_path);
}

/**
 * Load the current session state
 */
void telegram_store_session(struct telegram *instance)
{
    assure_file_exists(instance->config_path, "auth");
    assure_file_exists(instance->config_path, "state");
    write_auth_file(&instance->auth, instance->auth_path);
    write_state_file(&instance->proto, instance->state_path);
}

void on_authorized(struct mtproto_connection *c, void* data);

/**
 * Connect to the currently active data center
 */
void telegram_network_connect(struct telegram *instance)
{
    logprintf ("telegram_network_connect()\n");
    if (! instance->auth.DC_list) {
       logprintf("telegram_network_connect(): cannot connect, restore / init a session first.\n");
       assert(0);
    }
    struct dc *DC_working = telegram_get_working_dc (instance);
    assert (instance->config->proxy_request_cb);
    instance->config->proxy_request_cb (instance, DC_working->ip, DC_working->port);
}

/**
 * Login, and perform a registration when needed
 */
int telegram_login(struct telegram *instance)
{
    if (instance->session_state != STATE_AUTHORIZED) {
        logprintf("Cannot log in, invalid state: %d \n", instance->session_state);
        return -1;
    }
    do_help_get_config(instance);
    telegram_change_state(instance, STATE_CONFIG_REQUESTED, NULL);
    return 0;
}

void on_authorized(struct mtproto_connection *c, void *data)
{
    struct telegram *instance = data;
    logprintf("Authorized... storing current session %d.\n", c->connection->session[0]);
    telegram_store_session(instance);
    telegram_change_state(instance, STATE_AUTHORIZED, NULL);
}

void telegram_read_input (struct telegram *instance)
{
    try_read(instance->connection->connection);
    mtproto_free_closed();
    // free all mtproto_connections that may have errored through
    // a received query
}

void telegram_set_proxy(struct telegram *instance, int fd)
{
    struct dc *DC_working = telegram_get_working_dc (instance);
    instance->connection = mtproto_new (DC_working, fd, instance);
    instance->connection->on_ready = on_authorized;
    instance->connection->on_ready_data = instance;
    mtproto_connect (instance->connection);
}

int telegram_write_output (struct telegram *instance)
{
    return try_write(instance->connection->connection);
}

int telegram_has_output (struct telegram *instance)
{
    return instance->connection->queries_num > 0;
}

