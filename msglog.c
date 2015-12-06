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

    Copyright Matthias Jentsch 2014-2015
*/
#include <stdio.h>
#include <stdarg.h>
#include "telegram-purple.h"

/*
  msglog.c: Convenience methods for logging to libpurple log
 */

void log_level_printf (const char* format, va_list ap, int level) {
  char buffer[256];
  vsnprintf (buffer, sizeof(buffer), format, ap);
  
  int last = (int)strlen (buffer) - 1;
  if (last >= 2 && buffer[last] == '\n') {
    buffer[last] = '\0';
  }
  purple_debug (level, PLUGIN_ID, "%s\n", buffer);
}

void debug(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_MISC);
  va_end (ap);
}

void info(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_INFO);
  va_end (ap);
}

void warning(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_WARNING);
  va_end (ap);
}

void failure(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_ERROR);
  va_end (ap);
}

void fatal(const char* format, ...) {
  va_list ap;
  va_start (ap, format);
  log_level_printf (format, ap, PURPLE_DEBUG_FATAL);
  va_end (ap);
  info ("\n");
}

const char *print_flags (const char **names, int len, unsigned flags) {
  static char *text = NULL;
  if (text) {
    g_free (text);
    text = NULL;
  }
  int i;
  for (i = 0; i < len; i ++) {
    if (flags & 1) {
      char *new;
      if (text) {
        new = g_strconcat (text, " ", names[i], NULL);
        g_free (text);
      } else {
        new = g_strdup (names[i]);
      }
      text = new;
    }
    flags >>= 1;
  }
  return (const char*)text;
}

const char *print_flags_peer (unsigned flags) {
  const char *names[] = {
      "CREATED",
      "HAS_PHOTO",
      "DELETED",
      "OFFICIAL",
      "KICKED",
      "ADMIN",
      "CREATOR",
      "LEFT",
      "DEACTIVATED"
  };
  return print_flags (names, 9, flags);
}

const char *print_flags_channel (unsigned flags) {
  static char *text;
  if (text) {
    g_free (text);
    text = NULL;
  }
  const char *names[] = {
      "BROADCAST",
      "EDITOR",
      "MODERATOR",
      "MEGAGROUP"
  };
  text = g_strdup (print_flags_peer (flags));
  char *old = text;
  text = g_strconcat (text, " ", print_flags (names, 4, flags >> 16), NULL);
  g_free (old);
  return text;
}

const char *print_flags_update (unsigned flags) {
  const char *names[] = {
    "CREATED",
    "DELETED",
    "PHONE",
    "CONTACT",
    "PHOTO",
    "BLOCKED",
    "REAL_NAME",
    "NAME",
    "REQUESTED",
    "WORKING",
    "FLAGS",
    "TITLE",
    "ADMIN",
    "MEMBERS",
    "ACCESS_HASH",
    "USERNAME"
  };
  return print_flags (names, 16, flags);
}
