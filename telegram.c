#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#include "mtproto-common.h"
#include "telegram.h"
#include "msglog.h"
#include "glib.h"
#include "tools.h"
#include "mtproto-client.h"

/*
 * New message received
 */
void (*on_msg_handler)(struct message *M);
void on_update_new_message(void (*on_msg)(struct message *M)) 
{
    on_msg_handler = on_msg;
}
void event_update_new_message(struct message *M) 
{
    if (on_msg_handler) {
        on_msg_handler(M);
    }
}

/*
 * Peer allocated
 */
void (*on_peer_allocated_handler)(peer_t *peer);
void on_peer_allocated(void (*handler)(peer_t *peer)) 
{
    on_peer_allocated_handler = handler;
}
void event_peer_allocated(peer_t *peer) 
{
    if (on_peer_allocated_handler) {
        on_peer_allocated_handler(peer);
    }
}

/*
 * State changed
 */
GList *change_listeners = NULL;
void telegram_add_state_change_listener(struct telegram *instance, state_listener_t listener) 
{
    instance->change_state_listeners = g_list_append(instance->change_state_listeners, listener);
}
void telegram_change_state(struct telegram *instance, int state, void *data) 
{
    logprintf("telegram connection state changed to: %d\n", state);
    instance->session_state = state;
    GList *curr = instance->change_state_listeners; 
    while ((curr = g_list_next(change_listeners)) != NULL) {
       ((state_listener_t)curr->data)(instance, state, data);
    }
}

struct telegram *telegram_new(struct dc *DC, const char* login, const char *config_path)
{
    on_start ();
    struct telegram *this = malloc(sizeof(struct telegram));
    this->protocol_data = NULL;
    this->curr_dc = 0;
    this->auth.DC_list[0] = DC;
    this->change_state_listeners = NULL;

    this->login = g_strdup(login);
    this->config_path = g_strdup_printf("%s/%s", config_path, login);
    this->download_path = telegram_get_config(this, "downloads");
    this->auth_path = telegram_get_config(this, "auth");
    this->state_path = telegram_get_config(this, "state");
    this->secret_path = telegram_get_config(this, "secret");
    
    logprintf("%s\n", this->login);
    logprintf("%s\n", this->config_path);
    logprintf("%s\n", this->download_path);
    logprintf("%s\n", this->auth_path);
    logprintf("%s\n", this->state_path);

    telegram_change_state(this, STATE_INITIALISED, NULL);
    return this;
}

void telegram_free(struct telegram *this) 
{
    g_list_free(this->change_state_listeners);
    g_free(this->login);
    g_free(this->config_path);
    g_free(this->download_path);
    g_free(this->auth_path);
    g_free(this->state_path);
    g_free(this->secret_path);
    free(this);
}

void assert_file_usable(const char *file)
{
    assert(access(file, W_OK | R_OK | F_OK) != -1);
}

void assure_file_exists(const char *dir, const char *file)
{
    g_mkdir_with_parents(dir, 0700);
    char *f = g_strdup_printf("%s/%s", dir, file);
    close(open(f, O_RDWR | O_CREAT, S_IRUSR, S_IWUSR));
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
    //logprintf("get_connection() -> fd: %d\n", DC->sessions[0]->c->fd);
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
 * Load the current session state from a file
 */
void telegram_store_session(struct telegram *instance)
{
    assure_file_exists(instance->config_path, "auth");
    assure_file_exists(instance->config_path, "state");
    write_auth_file(&instance->auth, instance->auth_path);
    write_state_file(&instance->proto, instance->state_path);
}

char *telegram_get_config(struct telegram *instance, char *config)
{
    return g_strdup_printf("%s/%s", instance->config_path, config);
}

void telegram_network_connect(struct telegram *instance, int fd)
{
    logprintf("telegram_network_connect()\n");
    if (!instance->auth.DC_list) {
       logprintf("telegram_network_connect(): cannot connect, restore / initialise a session first.\n");
       assert(0);
    }
    struct dc *DC_working = telegram_get_working_dc(instance);

    // check whether authentication is needed
    if (!DC_working->auth_key_id) {
        logprintf("No working DC, authenticating.\n");
     
        // init a new connection
        struct connection *c = create_connection (DC_working->ip, DC_working->port, fd, instance);
        // init a new session with random session id
        dc_create_session(DC_working, c);

        // Request PQ
        send_req_pq_packet (c);
        telegram_change_state(instance, STATE_PQ_REQUESTED, NULL);
    } else {
        logprintf("Already working session, setting state to connected.\n");
        telegram_change_state(instance, STATE_CONNECTED, NULL);
    }
}

void on_connected(struct telegram *instance)
{
    assert (telegram_get_working_dc(instance)->auth_key_id);
    logprintf("Done...\n");
    telegram_store_session(instance);
}

void on_state_change(struct telegram *instance, int state, void *data) 
{
    switch (state) {
        case STATE_CONNECTED:
            on_connected(instance);
            break;

        case STATE_ERROR: {
            const char* err = data;
            logprintf("Telegram errored: %s \n", err);
        }
        break;

        case STATE_AUTH_DONE:
            logprintf("requesting configuration");
            telegram_change_state(instance, STATE_CONFIG_REQUESTED, NULL);
            do_help_get_config (instance);
        break;

        case STATE_CONFIG_REQUESTED:
            logprintf("switch: config_requested\n");
            telegram_store_session(instance);
        break;
    }
}


/**
 * Read newest rpc response and update state
 */
void try_rpc_interpret(struct telegram *instance, int op, int len)
{
    switch (instance->session_state) {
        case STATE_PQ_REQUESTED:
            logprintf("switch: pq_requested\n");
            rpc_execute_req_pq(instance, len);
            telegram_change_state(instance, STATE_DH_REQUESTED, NULL);
        break;
        case STATE_DH_REQUESTED:
            logprintf("switch: dh_requested\n");
            rpc_execute_rq_dh(instance, len);
            telegram_change_state(instance, STATE_CDH_REQUESTED, NULL);
        break;
        case STATE_CDH_REQUESTED:
            logprintf("switch: cdh_requested\n");
            rpc_execute_cdh_sent(instance, len);
            telegram_change_state(instance, STATE_AUTH_DONE, NULL);
        break;
        case STATE_AUTH_DONE:
            logprintf("switch: auth_done\n");
            rpc_execute_authorized(instance, op, len);
        break;
    }
}

void telegram_read_input (struct telegram *instance)
{
    return try_read(instance);
}

int telegram_write_output (struct telegram *instance)
{
    return try_write(instance);
}

int telegram_has_output (struct telegram *instance)
{
    return !all_queries_done();
}

