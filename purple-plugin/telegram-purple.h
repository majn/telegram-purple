/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#ifndef __TG_PURPLE_H__
#define __TG_PURPLE_H__
#define PLUGIN_ID "prpl-telegram"

#define TELEGRAM_AUTH_MODE_PHONE "phone"
#define TELEGRAM_AUTH_MODE_SMS "sms"

#include <glib.h>
#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "account.h"
#include "connection.h"
#include "mtproto-client.h"

typedef struct {
    struct telegram *tg;
	PurpleAccount *pa;
	PurpleConnection *gc;
        
    /**
     * Whether the state of the protocol has changed since the last save
     */
    int updated;

    /**
     * The used purple timeout handler
     */
    guint timer;

    /**
     * Queue of all new messages that need to be added to a chat
     */
    GQueue *new_messages;
      
    /**
     * Queue of all joined chats
     */
    GHashTable *joining_chats;

} telegram_conn;

typedef struct {
   
    /**
     * The mtproto_connection associated with this handle
     */
    struct mtproto_connection *mtp;

    /**
     * Write handler returned by purple_input_add
     */
    guint wh;

    /**
     * Read handler returned by purple_input_add
     */
    guint rh;

    /**
     * The file descriptor of the used socket     
     */
    int fd;

} mtproto_handle;

#endif
