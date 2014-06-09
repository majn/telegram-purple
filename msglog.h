/**
 * Set a custom logging callback to use instead of regular printing 
 * 	to stdout
 */
void set_log_cb(void (*cb)(const char*, ...));

/**
 * Log a message to the current message log
 */
void log_message(const char *format, ...);

/*
void log_debug(const char* format, ...);
void log_warning(const char* format, ...);
void log_error(const char* format, ...);
*/

