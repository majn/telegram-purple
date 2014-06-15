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
 * Retur if the current phone is registered in the given network.
 */
int network_phone_is_registered();

/**
 * Return if the current client is registered.
 */
int network_client_is_registered();

void set_default_username ();
// Settings
#define AUTH_MODE_SMS "sms"
#define AUTH_MODE_PHONE "phone"
