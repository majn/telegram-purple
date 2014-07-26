#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "mtproto-common.h"
#include "telegram.h"
#include "msglog.h"
#include "glib.h"
#include "mtproto-client.h"

/* 
 * Events
 */

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
    logprintf("Changing struct telegram state %d\n", state);
    instance->session_state = state;
    GList *curr = instance->change_state_listeners; 
    while ((curr = g_list_next(change_listeners)) != NULL) {
       ((state_listener_t)curr->data)(instance, state, data);
    }
}

struct telegram *telegram_new(struct dc *DC, const char* login, const char *config_path)
{
    struct telegram *this = malloc(sizeof(struct telegram));
    this->protocol_data = NULL;
    this->curr_dc = 0;
    this->auth.DC_list[0] = DC;
    this->change_state_listeners = NULL;
    this->config_path = config_path;
    strcpy(this->login, malloc(strlen(login) + 1));
    telegram_change_state(this, STATE_INITIALISED, NULL);
    return this;
}

void telegram_free(struct telegram *instance) 
{
    g_list_free(instance->change_state_listeners);
    free(instance->login);
    free(instance);
}

void assert_file_usable(const char *file)
{
    assert(access(file, W_OK | R_OK | F_OK) != -1);
}

void assure_file_exists(const char *dir, const char *file)
{
    char f[256];
    g_mkdir_with_parents(dir, 0700);
    sprintf(f, "%s/%s", dir, file);
    close(open(f, O_RDWR | O_CREAT, S_IRUSR, S_IWUSR));
    assert_file_usable(f);
}

struct connection *telegram_get_connection(struct telegram *instance)
{
    struct dc *DC = instance->auth.DC_list[instance->auth.dc_working_num];
    return DC->sessions[0]->c;
}

struct dc *telegram_get_working_dc(struct telegram *instance)
{
    struct dc *DC = instance->auth.DC_list[instance->auth.dc_working_num];
    return DC;
}

void telegram_restore_session(struct telegram *instance)
{
    char file[256];
    sprintf(file, "%s/%s/%s", instance->config_path, instance->login, "auth");
    instance->auth = read_auth_file(file);
    sprintf(file, "%s/%s/%s", instance->config_path, instance->login, "state");
    instance->proto = read_state_file(file);
}

void telegram_store_session(struct telegram *instance)
{
    char file[256];
    sprintf(file, "%s/%s", instance->config_path, instance->login);
    assure_file_exists(file, "auth");
    assure_file_exists(file, "state");

    sprintf(file, "%s/%s/%s", instance->config_path, instance->login, "auth");
    write_auth_file(instance->auth, file);

    sprintf(file, "%s/%s/%s", instance->config_path, instance->login, "state");
    write_state_file(instance->proto, file);
}

void telegram_get_downloads_dir(struct telegram *instance)
{
    
}

void telegram_network_connect(struct telegram *instance, int fd)
{
    on_start ();
    struct dc *DC_working = telegram_get_working_dc(instance);

    // check whether authentication is needed
    if (!DC_working->auth_key_id) {
        logprintf("No working DC, authenticating.\n");
     
        // init a new connection
        struct connection *c = create_connection (DC_working->ip, DC_working->port, fd);
        // init a new session with random session id
        dc_create_session(DC_working, c);

        // Request PQ
        char byte = 0xef;
        assert (write_out (c, &byte, 1) == 1);
        send_req_pq_packet (instance);
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
            break;
        }
    }
}


/**
 * Read newest rpc response and update state
 */
void try_rpc_interpret(struct telegram *instance, int op, int len)
{
    switch (instance->session_state) {
        case STATE_PQ_REQUESTED:
            rpc_execute_req_pq(instance, len);
            telegram_change_state(instance, STATE_DH_REQUESTED, NULL);
        break;
        case STATE_DH_REQUESTED:
            rpc_execute_rq_dh(instance, len);
            telegram_change_state(instance, STATE_CDH_REQUESTED, NULL);
        break;
        case STATE_CDH_REQUESTED:
            rpc_execute_cdh_sent(instance, len);
            telegram_change_state(instance, STATE_AUTH_DONE, NULL);
        break;
        case STATE_AUTH_DONE:
            rpc_execute_authorized(instance, op, len);
            telegram_change_state(instance, STATE_CONFIG_REQUESTED, NULL);
            do_help_get_config (instance);
        break;
        case STATE_CONFIG_REQUESTED:
            // ?
            telegram_store_session(instance);
        break;
    }
}

void telegram_read_input (struct telegram *instance)
{
    try_read(instance);
}

