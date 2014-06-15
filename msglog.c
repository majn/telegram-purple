#include <stdio.h>
#include <stdarg.h>

#define COLOR_RED "\033[0;31m"
#define COLOR_REDB "\033[1;31m"
#define COLOR_NORMAL "\033[0m"
#define COLOR_GREEN "\033[32;1m"
#define COLOR_GREY "\033[37;1m"
#define COLOR_YELLOW "\033[33;1m"
#define COLOR_BLUE "\033[34;1m"
#define COLOR_MAGENTA "\033[35;1m"
#define COLOR_CYAN "\033[36;1m"
#define COLOR_LCYAN "\033[0;36m"

#define COLOR_INVERSE "\033[7m"

/** 
 * Log a message to the telegram-cli message log, by 
 * just writing it to STDOUT and appending '***'
 */
void log_printf(const char *format, va_list ap)  
{
  printf (COLOR_GREY " *** ");
  vprintf (format, ap);
  printf (COLOR_NORMAL);
}

/**
 * The callback that will log the given msg to the used target
 */
void (*log_cb)(const char* format, va_list ap) = log_printf;

/**
 * Set a custom logging callback to use instead of the regular
 * 	printf to stdout
 */
void set_log_cb(void (*cb)(const char*, va_list ap))
{ 
   log_cb = cb;
}

/**
 * Log a message to the current message log
 */
void logprintf(const char *format, ...) 
{
   va_list ap;
   va_start (ap, format);
   log_cb(format, ap);
   va_end (ap);
}

/*
TODO: implement different log levels

void log_debug(const char* format, ...) 
{
}

void log_warning(const char* format, ...)
{
}

void log_error(const char* format, ...)
{
}
*/

