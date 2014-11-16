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
#define TG_AUTHOR "Matthias Jentsch <mtthsjntsch@gmail.com>, Vitaly Valtman, Christopher Althaus <althaus.christopher@gmail.com>, Markus Endres <endresma45241@th-nuernberg.de>. Based on libtgl by Vitaly Valtman."
#define TG_DESCRIPTION "Telegram protocol."
#define TG_VERSION "0.4"
#define TG_BUILD "8"

#include <glib.h>
#include <notify.h>
#include <plugin.h>
#include <version.h>
#include <account.h>
#include <connection.h>

typedef struct {
  struct tgl_state *TLS;

  /*
   * Used during login
   */
  char *hash;

  PurpleAccount *pa;
	PurpleConnection *gc;
        
  /**
   * Whether the state of the protocol has changed since the last save
   */
  int updated;

  /**
   * Queue of all new messages that need to be added to a chat
   */
  GQueue *new_messages;
      
  /**
   * Queue of all joined chats
   */
  GHashTable *joining_chats;

  guint timer;
} telegram_conn;

struct download_desc {
  int type;
  void *data;
};

void telegram_on_ready (struct tgl_state *TLS);
extern const char *pk_path;
extern const char *config_dir;

#endif
