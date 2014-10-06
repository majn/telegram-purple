#include <stdarg.h>

/**
 * Set a custom logging callback to use instead of regular printing 
 * 	to stdout
 */
void hexdump (int *in_ptr, int *in_end);

void debug(const char* format, ...);
void info(const char* format, ...);
void warning(const char* format, ...);
void failure(const char* format, ...);
void fatal(const char* format, ...);

