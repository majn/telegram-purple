/* 
 *  libtelegram
 *  ===========
 *
 * struct telegram library based on the telegram cli application, that was originally made by vysheng (see https://github.com/vysheng/tg)
 */

#ifndef __TELEGRAM_H__
#define __TELEGRAM_H__

#define MAX_DC_NUM 9
#define MAX_PEER_NUM 100000

#ifndef PROG_NAME
#define PROG_NAME "telegram"
#endif

#include <sys/types.h>
#include "glib.h"
#include "loop.h"
//#include "mtproto-client.h"

// forward declarations
struct message;
struct protocol_state;
struct authorization_state;

/*
 * telegram states
 */

#define STATE_INITIALISED 0
#define STATE_DISCONNECTED 1

// Error
#define STATE_ERROR 2

// intermediate authorization states already present and handled in mtproto-client.c
//#define STATE_PQ_REQUESTED 3
//#define STATE_DH_REQUESTED 4
//#define STATE_CDH_REQUESTED 5
#define STATE_AUTHORIZED 6

// dc discovery
#define STATE_CONFIG_REQUESTED 7
#define STATE_EXPORTING_CONFIG 8
#define STATE_DISCONNECTED_SWITCH_DC 9
#define STATE_CONFIG_RECEIVED 11

// login

// - Phone Registration
#define STATE_PHONE_NOT_REGISTERED 13
#define STATE_PHONE_CODE_REQUESTED 14
#define STATE_PHONE_CODE_NOT_ENTERED 15
#define STATE_PHONE_CODE_ENTERED 16

// - Client Registration
#define STATE_CLIENT_IS_REGISTERED_SENT 17
#define STATE_CLIENT_NOT_REGISTERED 18
#define STATE_CLIENT_CODE_REQUESTED 19
#define STATE_CLIENT_CODE_NOT_ENTERED 20
#define STATE_CLIENT_CODE_ENTERED 21

// Ready for sending and receiving messages
#define STATE_READY 22

/**
 * Binary log
 */

#define BINLOG_BUFFER_SIZE (1 << 20)
struct binlog {
  int binlog_buffer[BINLOG_BUFFER_SIZE];
  int *rptr;
  int *wptr;
  int test_dc; // = 0
  int in_replay_log;
  int binlog_enabled; // = 0;
  int binlog_fd;
  long long binlog_pos;

  int s[1000];
};

struct telegram;

/**
 * Contains all options and pointer to callback functions required by telegram
 */
struct telegram_config {

    /**
     * The base path containing the telegram configuration
     */
    const char* base_config_path;
    
    /**
     * Called when there is pending network output
     */
    void (*on_output)(struct telegram *instance);

    /**
     * A callback function that delivers a connections to the given hostname
     * and port by calling telegram_set_proxy. This is useful for tunelling
     * the connection through a proxy server.
     */
    void (*proxy_request_cb)(struct telegram *instance, const char *ip, int port);
    
    /**
     * A callback function that is called once the proxy connection is no longer
     * needed. This is useful for freeing all used resources.
     */
     void (*proxy_close_cb)(struct telegram *instance, int fd);

    /**
     * A callback function that is called when a phone registration is required. 
     *
     * This callback must query first name, last name and the 
     * authentication code from the user and call do_send_code_result_auth once done
     */
    void (*on_phone_registration_required) (struct telegram *instance);

    /**
     * A callback function that is called when a client registration is required. 
     *
     * This callback must query the authentication code from the user and
     * call do_send_code_result once done
     */
    void (*on_client_registration_required) (struct telegram *instance);

    /** 
     * A callback function that is called when telegram is ready
     */
    void (*on_ready) (struct telegram *instance);

    /** 
     * A callback function that is called when telegram is disconnected
     */
    void (*on_disconnected) (struct telegram *instance);

    /**
     * A callback function that is called when a new message was allocated. This is useful
     * for adding new messages to the GUI.
     */
    void (*on_msg_handler)(struct telegram *instance, struct message *M);

    /**
     * A callback function that is called when a new peer was allocated. This is useful
     * for populating the GUI with new peers.
     */
    void (*on_peer_allocated_handler)(struct telegram *instance, void *peer);

    /**
     * A callback function that is called when a user's status has changed
     */ 
    void (*on_update_user_status_handler) (struct telegram *instance, void *peer);

    /**
     * A callback function that is called when a user starts or stops typing
     */
    void (*on_update_uesr_typing_handler) (struct telegram *instance, void *peer);
};

/**
 * A telegram session
 *
 * Contains all globals from the telegram-cli application is passed to every
 * query call
 */
struct telegram {
    void *protocol_data; 
    //int curr_dc;

    char *login;
    char *config_path;
    char *download_path;
    char *auth_path;
    char *state_path;
    char *secret_path;

    int session_state;
    struct telegram_config *config;
    
    /*
     * protocol state
     */
    struct protocol_state proto;
    struct authorization_state auth;

    /*
     * connection
     */
    struct mtproto_connection *connection;

    /*
     * binlog
     */
    struct binlog *bl;

    // TODO: Bind this to the current data center, since the code hash is only
    // valid in its context
    char *phone_code_hash;

    void *extra;
};

/**
 * Create a new telegram application
 *
 * @param DC            The initial data center to use
 * @param login         The phone number to use as login name
 * @param config        Contains all callbacks used for the telegram instance
 */
struct telegram *telegram_new(struct dc *DC, const char* login, struct telegram_config *config);

/**
 * Resume the session to 
 */
void telegram_restore_session(struct telegram *instance);

/**
 * Store
 */
void telegram_store_session(struct telegram *instance);

/**
 * Destructor
 */
void telegram_free(struct telegram *instance);

/** 
 * Get the currently active connection
 */
struct connection *telegram_get_connection(struct telegram *instance);

/**
 * Return the current working dc
 */
struct dc *telegram_get_working_dc(struct telegram *instance);

/* 
 * Events
 */

/**
 * Change the state of the given telegram instance and execute all event handlers
 *
 * @param instance  The telegram instance that changed its state
 * @param state     The changed state
 * @param data      Extra data that depends on switched state
 */
void telegram_change_state(struct telegram *instance, int state, void *data);

/**
 * Connect to the telegram network with the given configuration
 */
void telegram_network_connect(struct telegram *instance);

int telegram_login (struct telegram *instance);

/**
 * Read the authorization_state stored in the given file
 */

// Export functions for plugins
void running_for_first_time ();

/**
 * Read and process all available input from the network
 */
void telegram_read_input (struct telegram *instance);

/**
 * Write all available output to the network
 */
int telegram_write_output (struct telegram *instance);

/**
 * Return whether there is pending output.
 */
int telegram_has_output (struct telegram *instance);

/**
 * Try to interpret RPC calls and apply the changes to the current telegram state
 */
void try_rpc_interpret(struct telegram *instance, int op, int len);

/* 
 * TODO: Refactor all old calls to take a telegrma instance
 */

/**
 * Request a registration code
 */
char* network_request_registration();

/**
 * Verify the registration with the given registration code
 */
int network_verify_registration(const char *code, const char *sms_hash);

/**
 * Verify the registration with the given registration code
 */
int network_verify_phone_registration(const char *code, const char *sms_hash, 
	const char *first, const char *last);

/**
 * Retur if the current phone is registered in the given network.
 */
int network_phone_is_registered();

/**
 * Return if the current client is registered.
 */
int network_client_is_registered();

/**
 * Export the current registration to all available data centers
 */
void network_export_registration();

/**
 * Fetch all unknown messages of the current session
 */
void session_get_difference();

/**
 * Fetch all known contacts
 */
void session_update_contact_list();

/* 
 * Events
 */
void event_update_new_message(struct telegram *instance, struct message *M);
void event_update_user_status(struct telegram *instance, void *peer);
void event_update_user_typing(struct telegram *instance, void *peer);

/*
 * Load known users and chats on connect
 */
void event_peer_allocated(struct telegram *instance, void *peer);

/**
 * Set a function to use as a handle to read from a network resource
 * instead of the regular socket read function
 */
void set_net_read_cb(ssize_t (*cb)(int fd, void *buff, size_t size));

/**
 * Set a function to use as handle to write to a newtork resource
 * instead of the regular socket write function
 */
void set_net_write_cb(ssize_t (*cb)(int fd, const void *buff, size_t size));

/**
 * Set the proxy-connection to use
 *
 * NOTE: you may only call this function from the 
 */
void telegram_set_proxy(struct telegram *instance, int fd);

#endif
