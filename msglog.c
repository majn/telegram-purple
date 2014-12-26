/*
    This file is part of telegram-purple

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA

    Copyright Matthias Jentsch 2014
*/
#include <stdio.h>
#include <stdarg.h>
#include <debug.h>
#include "telegram-purple.h"

#ifdef DEBUG
#define COLOR_GREY "\033[37;1m"
#define COLOR_YELLOW "\033[33;1m"
#define COLOR_RED "\033[0;31m"
#define COLOR_REDB "\033[1;31m"
#define COLOR_GREEN "\033[32;1m"
#define COLOR_NORMAL "\033[0m"
#else
#define COLOR_GREY ""
#define COLOR_YELLOW ""
#define COLOR_RED ""
#define COLOR_REDB ""
#define COLOR_GREEN ""
#define COLOR_NORMAL ""
#endif

/*
  msglog.c: Convenience methods for logging to libpurple log
 */

void log_level_printf (const char* format, va_list ap, int level, char *color) {
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), format, ap);
  purple_debug(level, PLUGIN_ID, "%s%s%s ", color, buffer, COLOR_NORMAL);
}

void debug(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_MISC, COLOR_NORMAL);
  va_end (ap);
}

void info(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_INFO, COLOR_GREEN);
  va_end (ap);
}

void warning(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_WARNING, COLOR_YELLOW);
  va_end (ap);
}

void failure(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_ERROR, COLOR_YELLOW);
  va_end (ap);
}

void fatal(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_FATAL, COLOR_REDB);
  va_end (ap);
  info ("\n");
}

