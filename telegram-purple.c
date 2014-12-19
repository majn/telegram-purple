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
 
    Copyright Matthias Jentsch, Vitaly Valtman, Christopher Althaus, Markus Endres 2014
*/
#include <glib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "purple.h"
#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "account.h"
#include "accountopt.h"
#include "blist.h"
#include "cmds.h"
#include "conversation.h"
#include "connection.h"
#include "debug.h"
#include "privacy.h"
#include "prpl.h"
#include "roomlist.h"
#include "status.h"
#include "util.h"
#include "prpl.h"
#include "prefs.h"
#include "util.h"
#include "eventloop.h"
#include "request.h"

#include <tgl.h>
#include "tgp-2prpl.h"
#include "tgp-net.h"
#include "tgp-timers.h"
#include "telegram-base.h"
#include "telegram-purple.h"
#include "msglog.h"
#include "lodepng.h"

PurplePlugin *_telegram_protocol = NULL;
PurpleGroup *tggroup;
const char *config_dir = ".telegram-purple";
const char *pk_path = "/etc/telegram-purple/server.pub";
int generate_ident_icon(unsigned char* sha1_key);
void tgprpl_login_on_connected();
void on_user_get_info (struct tgl_state *TLS, void *show_info, int success, struct tgl_user *U);

static telegram_conn *get_conn_from_buddy (PurpleBuddy *buddy) {
  telegram_conn *c = purple_connection_get_protocol_data (
      purple_account_get_connection (purple_buddy_get_account (buddy)));
  return c;
}

static const char *format_time (time_t date) {
  struct tm *datetime = localtime(&date);
  return purple_utf8_strftime("%Y.%m.%d %H:%M:%S", datetime);
}

static char *format_status (struct tgl_user_status *status) {
  return status->online ? "Online" : "Mobile";
}

static char *format_img_full (int imgstore) {
  return g_strdup_printf ("<br><img id=\"%u\">", imgstore);
}

static char *format_img_thumb (int imgstore, char *filename) __attribute__ ((unused));
static char *format_img_thumb (int imgstore, char *filename) {
  return g_strdup_printf("<a href=\"%s\"><img id=\"%u\"></a><br/><a href=\"%s\">%s</a>",
           filename, imgstore, filename, filename);
}

static char *format_service_msg (struct tgl_state *TLS, struct tgl_message *M)
{
  assert (M && M->service);

  char *txt_user = NULL;
  char *txt_action = NULL;
  char *txt = NULL;
  
  tgl_peer_t *peer = tgl_peer_get (TLS, M->from_id);
  if (! peer) {
    return NULL;
  }
  txt_user = p2tgl_strdup_alias (peer);
  
  switch (M->action.type) {
    case tgl_message_action_chat_create:
      txt_action = g_strdup_printf ("created chat %s", M->action.title);
      break;
    case tgl_message_action_chat_edit_title:
      txt_action = g_strdup_printf ("changed title to %s", M->action.new_title);
      break;
    case tgl_message_action_chat_edit_photo:
      txt_action = g_strdup ("changed photo");
      break;
    case tgl_message_action_chat_delete_photo:
      txt_action = g_strdup ("deleted photo");
      break;
    case tgl_message_action_chat_add_user:
      {
        tgl_peer_t *peer = tgl_peer_get (TLS, TGL_MK_USER (M->action.user));
        if (peer) {
          char *alias = p2tgl_strdup_alias (peer);
          txt_action = g_strdup_printf ("added user %s", alias);
          g_free (alias);
        }
        break;
      }
    case tgl_message_action_chat_delete_user:
      {
        tgl_peer_t *peer = tgl_peer_get (TLS, TGL_MK_USER (M->action.user));
        if (peer) {
          char *alias = p2tgl_strdup_alias (peer);
          txt_action = g_strdup_printf ("deleted user %s", alias);
          g_free (alias);
        }
        break;
      }
    case tgl_message_action_set_message_ttl:
      txt_action = g_strdup_printf ("set ttl to %d seconds", M->action.ttl);
      break;
    case tgl_message_action_read_messages:
      txt_action = g_strdup_printf ("%d messages marked read", M->action.read_cnt);
      break;
    case tgl_message_action_delete_messages:
      txt_action = g_strdup_printf ("%d messages deleted", M->action.delete_cnt);
      break;
    case tgl_message_action_screenshot_messages:
      txt_action = g_strdup_printf ("%d messages screenshoted", M->action.screenshot_cnt);
      break;
    case tgl_message_action_notify_layer:
      txt_action = g_strdup_printf ("updated layer to %d", M->action.layer);
      break;
      /*
    case tgl_message_action_request_key:
      txt_action = g_strdup_printf ("Request rekey #%016llx\n", M->action.exchange_id);
      break;
    case tgl_message_action_accept_key:
      txt_action = g_strdup_printf ("Accept rekey #%016llx\n", M->action.exchange_id);
      break;
    case tgl_message_action_commit_key:
      txt_action = g_strdup_printf ("Commit rekey #%016llx\n", M->action.exchange_id);
      break;
    case tgl_message_action_abort_key:
      txt_action = g_strdup_printf ("Abort rekey #%016llx\n", M->action.exchange_id);
      break;
      */
    default:
      txt_action = NULL;
      break;
  }
  if (txt_action) {
    debug ("SERVICE MESSAGE: %s", txt_action);
    txt = g_strdup_printf ("%s %s.", txt_user, txt_action);
    g_free (txt_action);
  }
  g_free (txt_user);
  return txt;
}

static int our_msg (struct tgl_state *TLS, struct tgl_message *M) {
  //return tgl_get_peer_id(M->from_id) == TLS->our_id;
  return M->out;
}

static gboolean queries_timerfunc (gpointer data) {
  debug ("queries_timerfunc()\n");
  telegram_conn *conn = data;
  
  if (conn->updated) {
    conn->updated = 0;
    write_state_file (conn->TLS);
  }
  return 1;
}

static void on_update_user_name (struct tgl_state *TLS, tgl_peer_t *user) __attribute__ ((unused));
static void on_update_user_name (struct tgl_state *TLS, tgl_peer_t *user) {
  p2tgl_got_alias(TLS, user->id, p2tgl_strdup_alias(user));
}

static void on_update_chat_participants (struct tgl_state *TLS, struct tgl_chat *chat) {
  PurpleConversation *pc = purple_find_chat(tg_get_conn(TLS), tgl_get_peer_id(chat->id));
  if (pc) {
    purple_conv_chat_clear_users (purple_conversation_get_chat_data(pc));
    chat_add_all_users (pc, chat);
  }
}

static void on_update_new_user_status (struct tgl_state *TLS, void *peer) __attribute__ ((unused));
static void on_update_new_user_status (struct tgl_state *TLS, void *peer) {
  tgl_peer_t *p = peer;
  p2tgl_prpl_got_user_status(TLS, p->id, &p->user.status);
}

static void update_message_received (struct tgl_state *TLS, struct tgl_message *M);
static void update_user_handler (struct tgl_state *TLS, struct tgl_user *U, unsigned flags);
static void update_chat_handler (struct tgl_state *TLS, struct tgl_chat *C, unsigned flags);
static void update_secret_chat_handler (struct tgl_state *TLS, struct tgl_secret_chat *C, unsigned flags);
static void update_user_typing (struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status);
struct tgl_update_callback tgp_callback = {
  .logprintf = debug,
  .new_msg = update_message_received, 
  .msg_receive = update_message_received,
  .user_update = update_user_handler,
  .chat_update = update_chat_handler,
  .secret_chat_update = update_secret_chat_handler,
  .type_notification = update_user_typing
};

void on_message_load_photo (struct tgl_state *TLS, void *extra, int success, char *filename) {
  gchar *data = NULL;
  size_t len;
  GError *err = NULL;
  g_file_get_contents (filename, &data, &len, &err);
  int imgStoreId = purple_imgstore_add_with_id (g_memdup(data, (guint)len), len, NULL);
    
  char *image = format_img_full(imgStoreId);
  struct tgl_message *M = extra;
  switch (tgl_get_peer_type (M->to_id)) {
    case TGL_PEER_CHAT:
      debug ("PEER_CHAT\n");
      if (! our_msg(TLS, M)) {
        chat_add_message (TLS, M, image);
      }
      break;
      
    case TGL_PEER_USER:
      debug ("PEER_USER\n");
      if (our_msg(TLS, M)) {
        p2tgl_got_im (TLS, M->to_id, image, PURPLE_MESSAGE_SEND | PURPLE_MESSAGE_IMAGES, M->date);
      } else {
        p2tgl_got_im (TLS, M->from_id, image, PURPLE_MESSAGE_RECV | PURPLE_MESSAGE_IMAGES, M->date);
      }
      break;
      
    case TGL_PEER_ENCR_CHAT:
      break;
      
    case TGL_PEER_GEO_CHAT:
      break;
  }
 
  g_free (image);
  telegram_conn *conn = TLS->ev_base;
  conn->updated = 1;
}

static void update_message_received (struct tgl_state *TLS, struct tgl_message *M) {
  debug ("received message\n");
  telegram_conn *conn = TLS->ev_base;
  conn->updated = 1;

  if (M->service) {
    debug ("service message, skipping...\n");
    char *text = format_service_msg (TLS, M);
    if (text) {
      switch (tgl_get_peer_type (M->to_id)) {
        case TGL_PEER_CHAT:
          chat_add_message (TLS, M, text);
          break;
          
        case TGL_PEER_USER:
          p2tgl_got_im (TLS, M->from_id, text, PURPLE_MESSAGE_SYSTEM, M->date);
          break;
      }
      g_free (text);
    }
    conn->updated = 1;
    return;
  }
  
  if ((M->flags & (FLAG_MESSAGE_EMPTY | FLAG_DELETED)) || !(M->flags & FLAG_CREATED)) {
    return;
  }
  if (!tgl_get_peer_type (M->to_id)) {
    warning ("Bad msg\n");
    return;
  }

  if (M->media.type == tgl_message_media_photo) {
    tgl_do_load_photo (TLS, &M->media.photo, on_message_load_photo, M);
    return;
  }

  if (!M->message) {
    return;
  }

  char *text = purple_markup_escape_text (M->message, strlen (M->message));
  switch (tgl_get_peer_type (M->to_id)) {
    case TGL_PEER_CHAT:
      debug ("PEER_CHAT\n");
      if (! our_msg(TLS, M)) {
        chat_add_message (TLS, M, text);
      }
      break;
      
    case TGL_PEER_USER:
      debug ("PEER_USER\n");
      
      // p2tgl_got_im (TLS, M->to_id, text, PURPLE_MESSAGE_SEND, M->date);
      // :TODO: figure out how to add messages from different devices to history
      if (! our_msg(TLS, M)) {
        p2tgl_got_im (TLS, M->from_id, text, PURPLE_MESSAGE_RECV, M->date);
      }
      break;
      
    case TGL_PEER_ENCR_CHAT:
      break;
      
    case TGL_PEER_GEO_CHAT:
      break;
  }
  
  g_free (text);
}

static void update_user_handler (struct tgl_state *TLS, struct tgl_user *user, unsigned flags) {
//  if (!(flags & TGL_UPDATE_CREATED)) { return; }
  if (TLS->our_id == tgl_get_peer_id (user->id)) {
    if (flags & TGL_UPDATE_NAME) {
      p2tgl_connection_set_display_name (TLS, (tgl_peer_t *)user);
    }
  } else {
    PurpleBuddy *buddy = p2tgl_buddy_find (TLS, user->id);
    if (!buddy) {
      buddy = p2tgl_buddy_new (TLS, (tgl_peer_t *)user);
      purple_blist_add_buddy (buddy, NULL, tggroup, NULL);
    }
    if (flags & TGL_UPDATE_CREATED) {
      purple_buddy_set_protocol_data (buddy, (gpointer)user);
      p2tgl_prpl_got_user_status (TLS, user->id, &user->status);
      p2tgl_buddy_update (TLS, (tgl_peer_t *)user, flags);
    }
    if (flags & (TGL_UPDATE_NAME | TGL_UPDATE_REAL_NAME | TGL_UPDATE_USERNAME) && buddy) {
      p2tgl_blist_alias_buddy (buddy, user);
    }
    if (flags & TGL_UPDATE_PHOTO) {
      tgl_do_get_user_info (TLS, user->id, 0, on_user_get_info, 0);
    }
    if (flags & TGL_UPDATE_DELETED && buddy) {
      purple_blist_remove_buddy (buddy);
    }
  }
}

static void update_secret_chat_handler (struct tgl_state *TLS, struct tgl_secret_chat *C, unsigned flags) {
  //tgl_do_get_chat_info (TLS, chat->id, 0, on_chat_get_info, 0);
  PurpleBuddy *buddy = p2tgl_buddy_find (TLS, C->id);
  
  if (flags & TGL_UPDATE_CREATED) {
    if (!buddy) {
      buddy = p2tgl_buddy_new (TLS, (tgl_peer_t *)C);
      purple_blist_add_buddy (buddy, NULL, tggroup, NULL);
    }
  }
  if (flags & (TGL_UPDATE_NAME | TGL_UPDATE_REAL_NAME | TGL_UPDATE_USERNAME) && buddy) {
  }
  if (flags & (TGL_UPDATE_PHOTO)) {
  }
  if (flags & TGL_UPDATE_DELETED && buddy) {
    purple_blist_remove_buddy (buddy);
  }
}

static void update_chat_handler (struct tgl_state *TLS, struct tgl_chat *chat, unsigned flags) {
  //tgl_do_get_chat_info (TLS, chat->id, 0, on_chat_get_info, 0);
  PurpleChat *ch = p2tgl_chat_find (TLS, chat->id);
  
  if (flags & TGL_UPDATE_CREATED) {
    if (!ch) {
      ch = p2tgl_chat_new (TLS, chat);
      purple_blist_add_chat(ch, NULL, NULL);
    }
  }
  if (flags & TGL_UPDATE_TITLE && ch) {
    purple_blist_alias_chat (ch, chat->print_title);
  }
  if (flags & (TGL_UPDATE_MEMBERS | TGL_UPDATE_ADMIN)) {
    on_update_chat_participants (TLS, chat);
  }
  if (flags & TGL_UPDATE_DELETED) {
    PurpleChat *ch = p2tgl_chat_find (TLS, chat->id);
    if (ch) {
      purple_blist_remove_chat (ch);
    } else {
      warning ("Cannot delete chat %d, not in buddy list.", chat->id);
    }
  }
}

static void update_user_typing (struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status) {
  if (status == tgl_typing_typing) {
    p2tgl_got_typing(TLS, U->id, 2);
  }
}

PurpleNotifyUserInfo *create_user_notify_info(struct tgl_user *usr) {
  PurpleNotifyUserInfo *info = purple_notify_user_info_new();
  purple_notify_user_info_add_pair(info, "First name", usr->first_name);
  purple_notify_user_info_add_pair(info, "Last name", usr->last_name);
  purple_notify_user_info_add_pair(info, "Phone", usr->phone);
  purple_notify_user_info_add_pair(info, "Status", usr->status.online == 1 ? "Online" : "Offline");
  return info;
}

static void on_contact_added (struct tgl_state *TLS,void *callback_extra, int success, int size, struct tgl_user *users[]) {
  PurpleBuddy *buddy = callback_extra;
  
  purple_blist_remove_buddy (buddy);
  if (!success || !size) {
    purple_notify_error (_telegram_protocol, "Adding Buddy Failed", "Buddy Not Found", "No contact with this phone number was found.");
  }
}

static void on_userpic_loaded (struct tgl_state *TLS, void *extra, int success, char *filename) {
  if (!success) {
    struct download_desc *dld = extra;
    struct tgl_user *U = dld->data;
    warning ("Can not load userpic for user %s %s\n", U->first_name, U->last_name);
  }
  telegram_conn *conn = TLS->ev_base;

  gchar *data = NULL;
  size_t len;
  GError *err = NULL;
  g_file_get_contents (filename, &data, &len, &err);
  int imgStoreId = purple_imgstore_add_with_id (g_memdup(data, (guint)len), len, NULL);
  
  struct download_desc *dld = extra;
  struct tgl_user *U = dld->data;
  
  if (imgStoreId <= 0) {
    warning ("Can not load userpic for user %s %s\n", U->first_name, U->last_name);
  }

  char *who = g_strdup_printf ("%d", tgl_get_peer_id (U->id));
  if (dld->type == 1) {
    PurpleNotifyUserInfo *info = create_user_notify_info(U);
    char *profile_image = profile_image = format_img_full(imgStoreId);
    purple_notify_user_info_add_pair (info, "Profile image", profile_image);
    //TODO: get the sha1key from the secret chat
    unsigned char sha1_key[20] = {129, 236, 235, 161, 62, 139, 244, 162, 120, 99, 99, 26, 171, 224, 25, 125};
    int sha1key_store_id = generate_ident_icon(sha1_key);
    if(sha1key_store_id != -1)
    {
        char *ident_icon = g_strdup_printf("<br><img id=\"%u\">", sha1key_store_id);
        purple_notify_user_info_add_pair(info, "Identification icon", ident_icon);
         g_free(ident_icon);
    }
    
    purple_notify_userinfo (conn->gc, who, info, NULL, NULL);
    g_free (profile_image);
  }
  purple_buddy_icons_set_for_user(conn->pa, who, g_memdup(data, (guint)len), len, NULL);
  g_free(who);
}

void on_user_get_info (struct tgl_state *TLS, void *show_info, int success, struct tgl_user *U)
{
  assert (success);
  
  if (U->photo.sizes_num == 0) {
    if (show_info) {
      PurpleNotifyUserInfo *info = create_user_notify_info(U);
      p2tgl_notify_userinfo(TLS, U->id, info, NULL, NULL);
    }
  } else {
    struct download_desc *dld = malloc (sizeof(struct download_desc));
    dld->data = U;
    dld->type = show_info ? 1 : 2;
    tgl_do_load_photo (TLS, &U->photo, on_userpic_loaded, dld);
  }
}

void on_chat_get_info (struct tgl_state *TLS, void *extra, int success, struct tgl_chat *C) {
  assert (success);
  
  debug ("on_chat_joined(%d)\n", tgl_get_peer_id (C->id));
  telegram_conn *conn = TLS->ev_base;

  PurpleConversation *conv;
  if (!(conv = purple_find_chat(conn->gc, tgl_get_peer_id(C->id)))) {
    // chat conversation is not existing, create it
    conv = serv_got_joined_chat(conn->gc, tgl_get_peer_id(C->id), C->title);
  }
  purple_conv_chat_clear_users(purple_conversation_get_chat_data(conv));
  chat_add_all_users(conv, C);
  
  struct message_text *mt = 0;
  while ((mt = g_queue_pop_head (conn->new_messages))) {
    if (!chat_add_message(TLS, mt->M, mt->text)) {
      warning ("WARNING, chat %d still not existing... \n", tgl_get_peer_id (C->id));
      break;
    }
    if (mt->text) {
      g_free (mt->text);
    }
    free (mt);
  }
  
  gchar *name = g_strdup_printf ("%d", tgl_get_peer_id(C->id));
  g_hash_table_remove (conn->joining_chats, name);
  g_free (name);
}

// This function generates a png image to visulize the sha1 key from an encrypted chat.

int generate_ident_icon(unsigned char* sha1_key)
{
    int colors[4] = {
		0xffffff,
		0xd5e6f3,
		0x2d5775,
		0x2f99c9
	};
	unsigned img_size = 160;
	unsigned char* image = (unsigned char*)malloc(img_size * img_size * 4);
    unsigned x, y, i, j, idx = 0;
    int bitpointer = 0;
	for (y = 0; y < 8; y++)
	{
		unsigned offset_y = y * img_size * 4 * (img_size / 8);
		for (x = 0; x < 8; x++)
		{
            int offset = bitpointer / 8;
            int shiftOffset = bitpointer % 8;
            int val = sha1_key[offset + 3] << 24 | sha1_key[offset + 2] << 16 | sha1_key[offset + 1] << 8 | sha1_key[offset];
            idx = abs((val >> shiftOffset) & 3) % 4;
            bitpointer += 2;
			unsigned offset_x = x * 4 * (img_size / 8);
			for (i = 0; i < img_size / 8; i++)
			{
				unsigned off_y = offset_y + i * img_size * 4;
				for (j = 0; j < img_size / 8; j++)
				{
					unsigned off_x = offset_x + j * 4;
					image[off_y + off_x + 0] = (colors[idx] >> 16) & 0xFF;
					image[off_y + off_x + 1] = (colors[idx] >> 8) & 0xFF;
					image[off_y + off_x + 2] = colors[idx] & 0xFF;
					image[off_y + off_x + 3] = 0xFF;
				}
			}
		}
	}
    unsigned char* png;
    size_t pngsize;
    unsigned error = lodepng_encode32(&png, &pngsize, image, img_size, img_size);
    int imgStoreId = -1;
    if(!error)
    {
        imgStoreId = purple_imgstore_add_with_id(g_memdup(png, pngsize), pngsize, NULL);
    }
    g_free(image);
    g_free(png);
    return imgStoreId;
}

void on_ready (struct tgl_state *TLS) {
  debug ("on_ready().\n");
  telegram_conn *conn = TLS->ev_base;
  
  purple_connection_set_state(conn->gc, PURPLE_CONNECTED);
  purple_connection_set_display_name(conn->gc, purple_account_get_username(conn->pa));
  purple_blist_add_account(conn->pa);
  tggroup = purple_find_group("Telegram");
  if (tggroup == NULL) {
    debug ("PurpleGroup = NULL, creating");
    tggroup = purple_group_new ("Telegram");
    purple_blist_add_group (tggroup, NULL);
  }
  
  debug ("seq = %d, pts = %d\n", TLS->seq, TLS->pts);
  tgl_do_get_difference (TLS, 0, 0, 0);
  tgl_do_get_dialog_list (TLS, 0, 0);
  tgl_do_update_contact_list (TLS, 0, 0);
  
  conn->timer = purple_timeout_add (5000, queries_timerfunc, conn);
}

static const char *tgprpl_list_icon (PurpleAccount * acct, PurpleBuddy * buddy) {
  return "telegram";
}

static void tgprpl_tooltip_text (PurpleBuddy * buddy, PurpleNotifyUserInfo * info, gboolean full) {
  debug ("tgprpl_tooltip_text()\n", buddy->name);
  
  tgl_peer_id_t *peer = purple_buddy_get_protocol_data(buddy);
  if (!peer) {
    purple_notify_user_info_add_pair_plaintext(info, "Status", "Offline");
    return;
  }
  tgl_peer_t *P = tgl_peer_get (get_conn_from_buddy (buddy)->TLS, *peer);
  if (!P) {
    warning ("tgprpl_tooltip_text: warning peer with id %d not found in tree.\n", peer->id);
    return;
  }
  purple_notify_user_info_add_pair_plaintext (info, "Status", format_status(&P->user.status));
  purple_notify_user_info_add_pair_plaintext (info, "Last seen: ", format_time(P->user.status.when));
}

static GList *tgprpl_status_types (PurpleAccount * acct) {
  debug ("tgprpl_status_types()\n");
  GList *types = NULL;
  PurpleStatusType *type;
  type = purple_status_type_new_with_attrs (PURPLE_STATUS_AVAILABLE, NULL, NULL,
          1, 1, 0, "last online", "last online", purple_value_new (PURPLE_TYPE_STRING), NULL);
  types = g_list_prepend (types, type);
  
  type = purple_status_type_new_with_attrs (PURPLE_STATUS_MOBILE, NULL, NULL, 1,
          1, 0, "last online", "last online", purple_value_new (PURPLE_TYPE_STRING), NULL);
  types = g_list_prepend (types, type);
  
  type = purple_status_type_new (PURPLE_STATUS_OFFLINE, NULL, NULL, 1);
  types = g_list_append (types, type);
  
  return g_list_reverse (types);
}

static GList *tgprpl_chat_join_info (PurpleConnection * gc) {
  debug ("tgprpl_chat_join_info()\n");
  struct proto_chat_entry *pce;
  
  pce = g_new0(struct proto_chat_entry, 1);
  pce->label = "_Subject:";
  pce->identifier = "subject";
  pce->required = TRUE;
  return g_list_append(NULL, pce);
}

static void tgprpl_login (PurpleAccount * acct) {
  debug ("tgprpl_login()\n");
  PurpleConnection *gc = purple_account_get_connection(acct);
  char const *username = purple_account_get_username(acct);
  
  struct tgl_state *TLS = tgl_state_alloc ();
  
  const char *dir = config_dir;
  struct passwd *pw = getpwuid(getuid());
  size_t len = strlen (dir) + strlen (pw->pw_dir) + 2 + strlen (username);
  TLS->base_path = malloc (len);
  snprintf (TLS->base_path, len, "%s/%s/%s", pw->pw_dir, dir, username);
  debug ("base configuration path: '%s'", TLS->base_path);
  g_mkdir_with_parents(TLS->base_path, 0700);
  
  len += strlen ("/downloads");
  char *ddir = malloc (len);
  sprintf (ddir, "%s/downloads", TLS->base_path);
  tgl_set_download_directory (TLS, ddir);
  g_mkdir_with_parents(ddir, 0700);
  free (ddir);
  
  tgl_set_verbosity (TLS, 4);
  tgl_set_rsa_key (TLS, pk_path);
  
  // create handle to store additional info for libpurple in
  // the new telegram instance
  telegram_conn *conn = g_new0(telegram_conn, 1);
  conn->TLS = TLS;
  conn->gc = gc;
  conn->pa = acct;
  conn->new_messages = g_queue_new ();
  conn->joining_chats = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
  purple_connection_set_protocol_data (gc, conn);
  
  tgl_set_ev_base (TLS, conn);
  tgl_set_net_methods (TLS, &tgp_conn_methods);
  tgl_set_timer_methods (TLS, &tgp_timers);
  tgl_set_callback (TLS, &tgp_callback);
  tgl_register_app_id (TLS, TGP_APP_ID, TGP_APP_HASH); 
  
  tgl_init (TLS);
  purple_connection_set_state (conn->gc, PURPLE_CONNECTING);
  
  telegram_login (TLS);
}

static void tgprpl_close (PurpleConnection * gc) {
  debug ("tgprpl_close()\n");
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  purple_timeout_remove(conn->timer);
  tgl_free_all (conn->TLS);
}

static int tgprpl_send_im (PurpleConnection * gc, const char *who, const char *message, PurpleMessageFlags flags) {
  debug ("tgprpl_send_im()\n");
  
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  PurpleAccount *pa = conn->pa;
 
  // this is part of a workaround to support clients without
  // the request API (request.h), see telegram-base.c:request_code()
  if (conn->in_fallback_chat) {
    request_code_entered (conn->TLS, message);
    conn->in_fallback_chat = 0;
    return 1;
  }
  
  PurpleBuddy *b = purple_find_buddy (pa, who);
  if (!b) {
    warning ("Buddy %s not found, cannot send IM\n", who);
    return -1;
  }
  
  tgl_peer_t *peer = tgl_peer_get(conn->TLS, TGL_MK_USER(atoi (who)));
  if (!peer) {
    warning ("Protocol data tgl_peer_t for %s not found, cannot send IM\n", who);
    return -1;
  }
  gchar *raw = purple_unescape_html(message);
  tgl_do_send_message (conn->TLS, peer->id, raw, (int)strlen (raw), 0, 0);
  g_free(raw);
  return 1;
}

static unsigned int tgprpl_send_typing (PurpleConnection * gc, const char *who, PurpleTypingState typing) {
  debug ("tgprpl_send_typing()\n");
  int id = atoi (who);
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  tgl_peer_t *U = tgl_peer_get (conn->TLS, TGL_MK_USER (id));
  if (U) {
    if (typing == PURPLE_TYPING) {
      tgl_do_send_typing (conn->TLS, U->id, tgl_typing_typing, 0, 0);
    } else {
      tgl_do_send_typing (conn->TLS, U->id, tgl_typing_cancel, 0, 0);
    }
  }
  return 0;
}

static void tgprpl_get_info (PurpleConnection * gc, const char *username) {
  debug ("tgprpl_get_info()\n");
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  tgl_peer_id_t u = TGL_MK_USER(atoi(username));
  tgl_do_get_user_info (conn->TLS, u, 0, on_user_get_info, (void *)1l);
}

static void tgprpl_set_status (PurpleAccount * acct, PurpleStatus * status) {
  debug ("tgprpl_set_status()\n");
}

static void tgprpl_add_buddy (PurpleConnection * gc, PurpleBuddy * buddy, PurpleGroup * group) {
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  const char* first = buddy->alias ? buddy->alias : "";
  
  tgl_do_add_contact (conn->TLS, buddy->name, (int)strlen (buddy->name), first, (int)strlen (first), "", 0, 0, on_contact_added, buddy);
}

static void tgprpl_add_buddies (PurpleConnection * gc, GList * buddies, GList * groups) {
  debug ("tgprpl_add_buddies()\n");
}

static void tgprpl_remove_buddy (PurpleConnection * gc, PurpleBuddy * buddy, PurpleGroup * group) {
  debug ("tgprpl_remove_buddy()\n");
  if (!buddy) { return; }
  
  telegram_conn *conn = purple_connection_get_protocol_data (gc);
  struct tgl_user *user = purple_buddy_get_protocol_data (buddy);
  if (!user) { warning ("cannot remove buddy '%s', no protocol data found\n", buddy->name); return; }
  
  tgl_do_del_contact (conn->TLS, user->id, NULL, NULL);
}

static void tgprpl_remove_buddies (PurpleConnection * gc, GList * buddies, GList * groups){
  debug ("tgprpl_remove_buddies()\n");
}

static void tgprpl_add_deny (PurpleConnection * gc, const char *name){
  debug ("tgprpl_add_deny()\n");
}

static void tgprpl_rem_deny (PurpleConnection * gc, const char *name){
  debug ("tgprpl_rem_deny()\n");
}

static void tgprpl_chat_join (PurpleConnection * gc, GHashTable * data) {
  debug ("tgprpl_chat_join()\n");
  
  telegram_conn *conn = purple_connection_get_protocol_data (gc);
  const char *groupname = g_hash_table_lookup (data, "subject");
  
  char *id = g_hash_table_lookup(data, "id");
  if (!id) {
    warning ("Got no chat id, aborting...\n");
    return;
  }
  if (!purple_find_chat(gc, atoi(id))) {
    tgl_do_get_chat_info (conn->TLS, TGL_MK_CHAT(atoi(id)), 0, on_chat_get_info, 0);
  } else {
    serv_got_joined_chat(conn->gc, atoi(id), groupname);
  }
}

static char *tgprpl_get_chat_name (GHashTable * data) {
  debug ("tgprpl_get_chat_name()\n");
  return g_strdup(g_hash_table_lookup(data, "subject"));
}

static PurpleXfer *tgprpl_new_xfer (PurpleConnection * gc, const char *who) {
  debug ("tgprpl_new_xfer()\n");
  return (PurpleXfer *)NULL;
}

static void tgprpl_send_file (PurpleConnection * gc, const char *who, const char *file) {
  debug ("tgprpl_send_file()\n");
}

static GHashTable *tgprpl_chat_info_deflt (PurpleConnection * gc, const char *chat_name) {
  debug ("tgprpl_chat_info_defaults()\n");
  return NULL;
}

static void tgprpl_chat_invite (PurpleConnection * gc, int id, const char *message, const char *name) {
  debug ("tgprpl_chat_invite()\n");

  telegram_conn *conn = purple_connection_get_protocol_data (gc);
  tgl_peer_t *chat = tgl_peer_get(conn->TLS, TGL_MK_CHAT (id));
  tgl_peer_t *user = tgl_peer_get(conn->TLS, TGL_MK_USER (atoi(name)));
  
  if (! chat || ! user) {
    purple_notify_error (_telegram_protocol, "Not found", "Cannot invite buddy to chat.", "Specified user is not existing.");
    return;
  }
  
  tgl_do_add_user_to_chat (conn->TLS, chat->id, user->id, 0, NULL, NULL);
}

static int tgprpl_send_chat (PurpleConnection * gc, int id, const char *message, PurpleMessageFlags flags) {
  debug ("tgprpl_send_chat()\n");
  telegram_conn *conn = purple_connection_get_protocol_data (gc);
  
  gchar *raw = purple_unescape_html(message);
  tgl_do_send_message (conn->TLS, TGL_MK_CHAT(id), raw, (int)strlen (raw), 0, 0);
  g_free (raw);
  
  p2tgl_got_chat_in(conn->TLS, TGL_MK_CHAT(id), TGL_MK_USER(conn->TLS->our_id), message,
                    PURPLE_MESSAGE_RECV, time(NULL));
  return 1;
}

static void tgprpl_group_buddy (PurpleConnection * gc, const char *who, const char *old_group, const char *new_group) {
  debug ("tgprpl_group_buddy()\n");
}

static void tgprpl_rename_group (PurpleConnection * gc, const char *old_name, PurpleGroup * group, GList * moved_buddies) {
  debug ("tgprpl_rename_group()\n");
}

static void tgprpl_convo_closed (PurpleConnection * gc, const char *who){
  debug ("tgprpl_convo_closed()\n");
}

static void tgprpl_set_buddy_icon (PurpleConnection * gc, PurpleStoredImage * img) {
  debug ("tgprpl_set_buddy_icon()\n");
  
  telegram_conn *conn = purple_connection_get_protocol_data (gc);
  if (purple_imgstore_get_filename (img)) {
    char* filename = g_strdup_printf ("%s/icons/%s", purple_user_dir(), purple_imgstore_get_filename (img));
    debug (filename);
    
    tgl_do_set_profile_photo (conn->TLS, filename, NULL, NULL);
    
    g_free (filename);
  }
}

static gboolean tgprpl_can_receive_file (PurpleConnection * gc, const char *who) {
  debug ("tgprpl_can_receive_file()\n");
  return 0;
}

static gboolean tgprpl_offline_message (const PurpleBuddy * buddy) {
  debug ("tgprpl_offline_message()\n");
  return 0;
}

static PurplePluginProtocolInfo prpl_info = {
  OPT_PROTO_NO_PASSWORD,
  NULL,                    // user_splits, initialized in tgprpl_init()
  NULL,                    // protocol_options, initialized in tgprpl_init()
  {
    "png",
    1,                     // min_width
    1,                     // min_height
    512,                   // max_width
    512,                   // max_height
    64000,                 // max_filesize
    PURPLE_ICON_SCALE_SEND,
  },
  tgprpl_list_icon,
  NULL,
  NULL,
  tgprpl_tooltip_text,
  tgprpl_status_types,
  NULL,                    // blist_node_menu
  tgprpl_chat_join_info,
  tgprpl_chat_info_deflt,
  tgprpl_login,
  tgprpl_close,
  tgprpl_send_im,
  NULL,                    // set_info
  tgprpl_send_typing,
  tgprpl_get_info,
  tgprpl_set_status,
  NULL,                    // set_idle
  NULL,                    // change_passwd
  tgprpl_add_buddy,
  tgprpl_add_buddies,
  tgprpl_remove_buddy,
  tgprpl_remove_buddies,
  NULL,                    // add_permit
  tgprpl_add_deny,
  NULL,                    // rem_permit
  tgprpl_rem_deny,
  NULL,                    // set_permit_deny
  tgprpl_chat_join,
  NULL,                    // reject_chat
  tgprpl_get_chat_name,
  tgprpl_chat_invite,
  NULL,                    // chat_leave
  NULL,                    // chat_whisper
  tgprpl_send_chat,
  NULL,                    // keepalive
  NULL,                    // register_user
  NULL,                    // get_cb_info
  NULL,                    // get_cb_away
  NULL,                    // alias_buddy
  tgprpl_group_buddy, 
  tgprpl_rename_group,
  NULL,                    // buddy_free
  tgprpl_convo_closed,     
  purple_normalize_nocase, 
  tgprpl_set_buddy_icon,
  NULL,                    // remove_group
  NULL,                    // get_cb_real_name
  NULL,                    // set_chat_topic
  NULL,                    // find_blist_chat
  NULL,                    // roomlist_get_list
  NULL,                    // roomlist_cancel
  NULL,                    // roomlist_expand_category
  tgprpl_can_receive_file,
  tgprpl_send_file,       
  tgprpl_new_xfer,        
  tgprpl_offline_message,
  NULL,                    // whiteboard_prpl_ops
  NULL,                    // send_raw
  NULL,                    // roomlist_room_serialize
  NULL,                    // unregister_user
  NULL,                    // send_attention
  NULL,                    // get_attention_types
  sizeof(PurplePluginProtocolInfo),
  NULL,           		     // get_account_text_table
  NULL,                    // initiate_media
  NULL,                    // get_media_caps
  NULL,                    // get_moods
  NULL,                    // set_public_alias
  NULL,                    // get_public_alias
  NULL,                    // add_buddy_with_invite
  NULL                     // add_buddies_with_invite
};


static void tgprpl_init (PurplePlugin *plugin) {
  PurpleAccountOption *opt;
  
  opt = purple_account_option_bool_new("Compatibility Mode (read SMS code from settings)",
          "compat-verification", 0);
  prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, opt);
  
  _telegram_protocol = plugin;
}

static GList *tgprpl_actions(PurplePlugin * plugin, gpointer context) {
  // return possible actions (See Libpurple doc)
  return (GList *)NULL;
}

static PurplePluginInfo plugin_info = {
  PURPLE_PLUGIN_MAGIC,
  PURPLE_MAJOR_VERSION,
  PURPLE_MINOR_VERSION,
  PURPLE_PLUGIN_PROTOCOL,
  NULL,
  0,
  NULL,
  PURPLE_PRIORITY_DEFAULT,
  PLUGIN_ID,
  "Telegram",
  TG_VERSION " libtgl: " TGL_VERSION,
  "Telegram",
  TG_DESCRIPTION,
  TG_AUTHOR,
  "https://github.com/majn/telegram-purple",
  NULL,           // on load
  NULL,           // on unload
  NULL,           // on destroy
  NULL,           // ui specific struct
  &prpl_info,
  NULL,           // prefs info
  tgprpl_actions,
  NULL,           // reserved
  NULL,           // reserved
  NULL,           // reserved
  NULL            // reserved
};

PURPLE_INIT_PLUGIN (telegram, tgprpl_init, plugin_info)
