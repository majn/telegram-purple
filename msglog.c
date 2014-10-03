#include <stdio.h>
#include <stdarg.h>
#include "debug.h"
#include "purple-plugin/telegram-purple.h"

#define COLOR_GREY "\033[37;1m"
#define COLOR_YELLOW "\033[33;1m"
#define COLOR_RED "\033[0;31m"
#define COLOR_REDB "\033[1;31m"
#define COLOR_GREEN "\033[32;1m"
#define COLOR_NORMAL "\033[0m"

/** 
 * Log a message to the telegram-cli message log, by 
 * just writing it to STDOUT and appending '***'
 */
void log_printf(const char *format, va_list ap)  
{
    vprintf (format, ap);
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

void hexdump (int *in_ptr, int *in_end) {
    int *ptr = in_ptr;
    while (ptr < in_end) { printf (" %08x", *(ptr ++)); }
    printf ("\n");
}

void log_level_printf (const char* format, va_list ap, int level, char *color)
{
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, ap);
    purple_debug(level, PLUGIN_ID, "%s%s%s ", color, buffer, COLOR_NORMAL);
}

void debug(const char* format, ...) 
{
    va_list ap;
    va_start (ap, format);
    log_level_printf (format, ap, PURPLE_DEBUG_MISC, COLOR_NORMAL);
    va_end (ap);
}

void info(const char* format, ...)
{
    va_list ap;
    va_start (ap, format);
    log_level_printf (format, ap, PURPLE_DEBUG_INFO, COLOR_GREEN);
    va_end (ap);
}

void warning(const char* format, ...)
{
    va_list ap;
    va_start (ap, format);
    log_level_printf (format, ap, PURPLE_DEBUG_WARNING, COLOR_YELLOW);
    va_end (ap);
}

void failure(const char* format, ...)
{
    va_list ap;
    va_start (ap, format);
    log_level_printf (format, ap, PURPLE_DEBUG_ERROR, COLOR_YELLOW);
    va_end (ap);
}

void fatal(const char* format, ...)
{
    va_list ap;
    va_start (ap, format);
    log_level_printf (format, ap, PURPLE_DEBUG_FATAL, COLOR_REDB);
    va_end (ap);
    info ("\n");
}

