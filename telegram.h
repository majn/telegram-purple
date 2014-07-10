/* 
 *  libtelegram
 *  ===========
 *
 * Telegram library based on the telegram cli application by vysheng (see https://github.com/vysheng/tg)
 */

#define MAX_DC_NUM 9
#define MAX_PEER_NUM 100000

#ifndef PROG_NAME
#define PROG_NAME "telegram"
#endif

#include <sys/types.h>
#include "net.h"
#include "mtproto-common.h"
#include "structures.h"


/**
 * A telegram session
 *
 * Contains all globals from the telegram-cli application and should
 * be passed to 
 */
struct telegram {

    /*
     * Read and save the configuration files into this directory
     *
     * Every distinct account needs its own configuration directory, that
     * will be used to store encryption data and the protocol state for this
     * specific user
     */
    char *config_dir;

    /* 
     * Reserved for custom protocol data
     */
    void *protocol_data; 
       
    /*
     * Events and Callbacks
     */
    // TODO: Insert definitions for all function pointers for event and logging

    /* 
     * Internal protocol state
     */
    // TODO: Insert *all* global variables from the telegram-implementation

};

/*
 * Constructor
 */
void telegram_create( /* struct telegram, struct configuration config */ );
    // TODO: Initiate a new telegram instance 

/*
 * Destructor
 */
void telegram_destroy(struct telegram instance);
    // TODO: Write clean-up functions to free all allocated resources of this session

// Export functions for plugins

int tg_login ();

void running_for_first_time ();
void parse_config ();
void store_config ();
void read_auth_file ();

/**
 * Connect to the telegram network with the given configuration
 */
void network_connect();

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
void on_update_new_message(void (*on_msg)(struct message *M));
void event_update_new_message(struct message *M);

void on_update_user_typing();
void on_update_chat_user_typing();
void on_update_user_status();
void on_update_user_name();
void on_update_user_photo();
void on_update_chat_participants();

/*
 * Load known users and chats on connect
 */
void on_user_allocated();

void on_user_allocated(void (*handler)(peer_t *user));
void event_user_allocated(peer_t *user);

void on_chat_allocated(void (*handler)(peer_t *chat));
void event_chat_allocated(peer_t *chat);

// template
//void on_blarg(void (*on_msg)(struct message *M));
//void event_blarg(struct message *M);

void on_chat_allocated();

/**
 * Set a function to use as a handle to read from a network resource
 * instead of the regular socket read function
 */
void set_net_read_cb(int (*cb)(int fd, void *buff, size_t size));

/**
 * Set a function to use as handle to write to a newtork resource
 * instead of the regular socket write function
 */
void set_net_write_cb(int (*cb)(int fd, const void *buff, size_t size));

/**
 * The current proxy connection source.
 */
extern void (*proxy_connection_source)(const char *host, int port, void (*on_connection_created)(int fd));

/**
 * The connection data passed to the connection source.
 */
extern void *proxy_connection_data;

/**
 * Set an alternative connection_source which is used to create open connections instead of the
 *  regular function.
 *
 * @param connection_source Called when a new connection is needed. A connection source must accept
 *                           host and port and pass a valid file descriptor to an open TCP-Socket to the
 *                           callback function on_connection_created
 * @param data              Additional connection data, that will be passed to the callback and may be
 *                          needed for establishing the connection.
 */
void set_proxy_connection_source (void (*connection_source)(const char *host, int port, void (*on_connection_created)(int fd)), void* data);

/**
 * ?
 */
void set_default_username ();
