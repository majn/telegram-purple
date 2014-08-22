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
void (*on_peer_allocated_handler)(void *peer);
void on_peer_allocated(void (*handler)(void *peer)) 
{
    on_peer_allocated_handler = handler;
}
void event_peer_allocated(void *peer) 
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
    do {
       ((state_listener_t)curr->data)(instance, state, data);
    } while ((curr = g_list_next(change_listeners)) != NULL);
}

void on_state_change(struct telegram *instance, int state, void *data) 
{
    logprintf("on_state_change: %d\n", state);
    switch (state) {
        case STATE_ERROR: {
            const char* err = data;
            logprintf("Telegram errored: %s \n", err);
        }
        break;

        case STATE_AUTHORIZED:
            logprintf("requesting configuration");
            telegram_change_state(instance, STATE_CONFIG_REQUESTED, NULL);
            do_help_get_config (instance);
        break;

        case STATE_CONFIG_RECEIVED:
            logprintf("received network configuration, checking whether phone is registered.");
            telegram_store_session(instance);
            do_auth_check_phone(instance, instance->login);
        break;

        case STATE_PHONE_NOT_REGISTERED:
            logprintf("phone is not registered, need to register phone number.\n");
            do_send_code(instance, instance->login);
        break;

        case STATE_PHONE_CODE_REQUESTED:
            logprintf("phone authenticion, user needs to enter code, first and last name.\n");
            // wait for user input ...
            break;

        case STATE_CLIENT_NOT_REGISTERED:
            logprintf("phone is already registered, need to register client.\n");
            do_send_code(instance, instance->login);
        break;

        case STATE_CLIENT_CODE_NOT_ENTERED:
            logprintf("client authentication, user needs to enter code");
            // wait for user input ...
        break;

        case STATE_DISCONNECTED_SWITCH_DC:
            logprintf("Have to migrate to other DC");
            instance->connection
        break;
    }
}

struct telegram *telegram_new(struct dc *DC, const char* login, const char *config_path)
{
    struct telegram *this = malloc(sizeof(struct telegram));
    this->protocol_data = NULL;
    //this->curr_dc = 0;
    this->auth.DC_list[0] = DC;
    this->change_state_listeners = NULL;
    this->bl = talloc0 (sizeof(struct binlog));

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

    telegram_add_state_change_listener(this, on_state_change);
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
    tfree(this->bl, sizeof(struct binlog));
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

void on_authorized(struct mtproto_connection *c, void* data);

/**
 * Connect to the currently active data center
 */
void telegram_network_connect(struct telegram *instance, int fd)
{
    logprintf("telegram_network_connect()\n");
    if (!instance->auth.DC_list) {
       logprintf("telegram_network_connect(): cannot connect, restore / init a session first.\n");
       assert(0);
    }
    struct dc *DC_working = telegram_get_working_dc(instance);
    instance->connection = mtproto_new(DC_working, fd, instance);
    instance->connection->on_ready = on_authorized;
    instance->connection->on_ready_data = instance;
    mtproto_connect(instance->connection);
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
    return try_read(instance->connection->connection);
}

int telegram_write_output (struct telegram *instance)
{
    return try_write(instance->connection->connection);
}

int telegram_has_output (struct telegram *instance)
{
    return instance->connection->queries_num > 0;
}

