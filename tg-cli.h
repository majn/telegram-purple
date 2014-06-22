#include <sys/types.h>
#include "net.h"
#include "mtproto-common.h"
#include "structures.h"

// Export functions for plugins

int tg_login ();

void running_for_first_time ();
void parse_config ();
void store_config ();
void read_auth_file ();

/**
 * Connect to the telegram network with the given configuration
 */
void network_connect ();

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
 * Set a function to use as handle to create new connections instead of the regular
 * socket write function
 */
void set_dc_ensure_session_cb (void (*dc_ens_sess)(struct dc *DC, void (*on_session_ready)(void)));

/**
 * ?
 */
void set_default_username ();
// Settings
#define AUTH_MODE_SMS "sms"
#define AUTH_MODE_PHONE "phone"
