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

    Copyright Matthias Jentsch, Vitaly Valtman, Christopher Althaus, Ben Wiederhake 2014-2015
*/
#ifndef __TG_PURPLE_H__
#define __TG_PURPLE_H__

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

// Define macros for gettext translation, ENABLE_NLS should be set if configure has detected
// libintl.h. If this is not the case a dummy macro is defined to bypass the translation functions
#ifdef ENABLE_NLS
#  include <glib/gi18n-lib.h>
#  define P_(Singular,Plural,N) ((char *) g_dngettext (GETTEXT_PACKAGE, Singular, Plural, N))
#else
#  define _(String) String
#  define P_(Singular,Plural,N) Plural
#  define N_(String) String
#  define gettext(String) String
#endif

#include <tgl.h>
#include <tgl-binlog.h>
#include <tgl-queries.h>
#include <tgl-structures.h>

#include <glib.h>
#include <purple.h>

#include "telegram-base.h"
#include "tgp-blist.h"
#include "tgp-structs.h"
#include "tgp-2prpl.h"
#include "tgp-net.h"
#include "tgp-timers.h"
#include "tgp-utils.h"
#include "tgp-chat.h"
#include "tgp-ft.h"
#include "tgp-msg.h"
#include "tgp-request.h"
#include "tgp-info.h"
#include "msglog.h"

#define PLUGIN_ID "prpl-telegram"
#define PLUGIN_AUTHOR "Matthias Jentsch <mtthsjntsch@gmail.com>\n\t\t\tVitaly Valtman\n\t\t\tBen Wiederhake\n\t\t\tChristopher Althaus <althaus.christopher@gmail.com>"

#define TGP_APP_HASH "99428c722d0ed59b9cd844e4577cb4bb"
#define TGP_APP_ID 16154

#define TGP_MAX_MSG_SIZE 4096
#define TGP_DEFAULT_MAX_MSG_SPLIT_COUNT 4

#define TGP_DEFAULT_FT_HANDLING "ask"
#define TGP_KEY_FT_HANDLING "media-handling-behavior"

#define TGP_DEFAULT_MEDIA_SIZE 32768
#define TGP_KEY_MEDIA_SIZE "media-size-threshold"

#define TGP_KEY_PASSWORD_TWO_FACTOR "password-two-factor"

#define TGP_DEFAULT_ACCEPT_SECRET_CHATS "ask"
#define TGP_KEY_ACCEPT_SECRET_CHATS "accept-secret-chats"

#define TGP_DEFAULT_INACTIVE_DAYS_OFFLINE 7
#define TGP_KEY_INACTIVE_DAYS_OFFLINE  "inactive-days-offline"

#define TGP_DEFAULT_HISTORY_RETRIEVAL_THRESHOLD 14
#define TGP_KEY_HISTORY_RETRIEVAL_THRESHOLD  "history-retrieve-days"

#define TGP_DEFAULT_JOIN_GROUP_CHATS TRUE
#define TGP_KEY_JOIN_GROUP_CHATS "auto-join-group-chats"

#define TGP_DEFAULT_DISPLAY_READ_NOTIFICATIONS FALSE
#define TGP_KEY_DISPLAY_READ_NOTIFICATIONS "display-read-notifications"

#define TGP_DEFAULT_SEND_READ_NOTIFICATIONS TRUE
#define TGP_KEY_SEND_READ_NOTIFICATIONS "send-read-notifications"

#define TGP_DEFAULT_CHANNEL_MEMBERS 200
#define TGP_KEY_CHANNEL_MEMBERS "channel-member-count"

#define TGP_DEFAULT_USE_IPV6 FALSE
#define TGP_KEY_USE_IPV6 "ipv6"

#define TGP_KEY_RESET_AUTH "reset-authorization"

#define TGP_CHANNEL_HISTORY_LIMIT 100

extern const char *pk_path;
extern const char *user_pk_filename;
extern PurplePlugin *_telegram_protocol;
void import_chat_link (struct tgl_state *TLS, const char *link);
void export_chat_link_by_name (struct tgl_state *TLS, const char *name);
void leave_and_delete_chat (struct tgl_state *TLS, tgl_peer_t *P);
void leave_and_delete_chat_by_name (struct tgl_state *TLS, const char *name);
void channel_load_photo (struct tgl_state *TLS, void *extra, int success, struct tgl_channel *C);

#endif
