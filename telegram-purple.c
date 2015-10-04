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
 
    Copyright Matthias Jentsch, Vitaly Valtman, Christopher Althaus, Markus Endres 2014-2015
*/

#include "telegram-purple.h"

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
#include <regex.h>

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
#include <tgl-binlog.h>
#include <tgl-queries.h>
#include <tools.h>
#include <tgl-methods-in.h>

#include "msglog.h"
#include "telegram-base.h"
#include "tgp-structs.h"
#include "tgp-2prpl.h"
#include "tgp-net.h"
#include "tgp-timers.h"
#include "tgp-utils.h"
#include "tgp-chat.h"
#include "tgp-ft.h"
#include "tgp-msg.h"
#include "tgp-request.h"

static void get_password (struct tgl_state *TLS, enum tgl_value_type type, const char *prompt, int num_values,
                          void (*callback)(struct tgl_state *TLS, const char *string[], void *arg), void *arg);
static void update_message_handler (struct tgl_state *TLS, struct tgl_message *M);
static void update_user_handler (struct tgl_state *TLS, struct tgl_user *U, unsigned flags);
static void update_user_status_handler (struct tgl_state *TLS, struct tgl_user *U);
static void update_chat_handler (struct tgl_state *TLS, struct tgl_chat *C, unsigned flags);
static void update_secret_chat_handler (struct tgl_state *TLS, struct tgl_secret_chat *C, unsigned flags);
static void update_user_typing (struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status);
static void update_marked_read (struct tgl_state *TLS, int num, struct tgl_message *list[]);
static char *format_print_name (struct tgl_state *TLS, tgl_peer_id_t id, const char *a1, const char *a2, const char *a3, const char *a4);
void on_user_get_info (struct tgl_state *TLS, void *info_data, int success, struct tgl_user *U);

PurpleGroup *tggroup;
const char *config_dir = "telegram-purple";
const char *pk_path = "/etc/telegram-purple/server.tglpub";

struct tgl_update_callback tgp_callback = {
  .logprintf = debug,
  .get_values = get_password,
  .new_msg = update_message_handler,
  .msg_receive = update_message_handler,
  .user_update = update_user_handler,
  .user_status_update = update_user_status_handler,
  .chat_update = update_chat_handler,
  .secret_chat_update = update_secret_chat_handler,
  .type_notification = update_user_typing,
  .marked_read = update_marked_read,
  .create_print_name = format_print_name
};

static void _update_buddy (struct tgl_state *TLS, tgl_peer_t *user, unsigned flags) {
  PurpleBuddy *buddy = p2tgl_buddy_find (TLS, user->id);
  if (buddy) {
    if (flags & TGL_UPDATE_DELETED) {
      purple_blist_remove_buddy (buddy);
    } else {
      if (flags & (TGL_UPDATE_NAME | TGL_UPDATE_REAL_NAME | TGL_UPDATE_USERNAME)) {
        char *alias = p2tgl_strdup_alias (user);
        purple_blist_alias_buddy (buddy, alias);
        g_free (alias);
      }
      if (flags & TGL_UPDATE_PHOTO) {
        tgl_do_get_user_info (TLS, user->id, 0, on_user_get_info, get_user_info_data_new (0, user->id));
      }
    }
  }
}

static void update_user_handler (struct tgl_state *TLS, struct tgl_user *user, unsigned flags) {
  if (tgl_get_peer_id (TLS->our_id) == tgl_get_peer_id (user->id) && flags & TGL_UPDATE_NAME) {
    p2tgl_connection_set_display_name (TLS, (tgl_peer_t *)user);
    return;
  }
  
  if (! (flags & TGL_UPDATE_CREATED)) {
     // buddy was altered, update it
      _update_buddy (TLS, (tgl_peer_t *)user, flags);
  } else {
    // new buddy was fetched, just update the status for all buddies in the contact list
    if (p2tgl_buddy_find (TLS, user->id)) {
      p2tgl_prpl_got_user_status (TLS, user->id, &user->status);
    }
  }
}

static void update_message_handler (struct tgl_state *TLS, struct tgl_message *M) {
  write_files_schedule (TLS);
  tgp_msg_recv (TLS, M);
}

static void update_user_status_handler (struct tgl_state *TLS, struct tgl_user *U) {
  p2tgl_prpl_got_user_status (TLS, U->id, &U->status);
}

static void update_secret_chat_handler (struct tgl_state *TLS, struct tgl_secret_chat *U, unsigned flags) {
  PurpleBuddy *buddy = p2tgl_buddy_find (TLS, U->id);
  debug ("update_secret_chat_handler: state=%d", U->state);
  
  if (!(flags & TGL_UPDATE_DELETED)) {
    if (! buddy) {
      buddy = p2tgl_buddy_new (TLS, (tgl_peer_t *)U);
      purple_blist_add_buddy (buddy, NULL, tggroup, NULL);
      purple_blist_alias_buddy (buddy, U->print_name);
    }
    p2tgl_prpl_got_set_status_mobile (TLS, U->id);
  }
  
  if (flags & TGL_UPDATE_WORKING || flags & TGL_UPDATE_DELETED) {
    write_secret_chat_file (TLS);
  }
  
  if (flags & TGL_UPDATE_REQUESTED) {
    connection_data *conn = TLS->ev_base;
    const char* choice = purple_account_get_string (conn->pa, "accept-secret-chats", "ask");
    if (! strcmp (choice, "always")) {
      tgl_do_accept_encr_chat_request (TLS, U, write_secret_chat_gw, 0);
    } else if (! strcmp(choice, "ask")) {
      request_accept_secret_chat (TLS, U);
    }
  }
  
  if (!(flags & TGL_UPDATE_CREATED) && buddy) {
    if (flags & TGL_UPDATE_DELETED) {
      p2tgl_got_im (TLS, U->id, _("Secret chat terminated."), PURPLE_MESSAGE_SYSTEM | PURPLE_MESSAGE_WHISPER, time(0));
      p2tgl_prpl_got_set_status_offline (TLS, U->id);
    } else {
      _update_buddy (TLS, (tgl_peer_t *)U, flags);
    }
  }
}

static void update_chat_handler (struct tgl_state *TLS, struct tgl_chat *chat, unsigned flags) {
  if (! (flags & TGL_UPDATE_CREATED)) {
    PurpleChat *ch = p2tgl_chat_find (TLS, chat->id);
    
    if (flags & TGL_UPDATE_TITLE && ch) {
      purple_blist_alias_chat (ch, chat->print_title);
    }
    if (flags & TGL_UPDATE_DELETED && ch) {
      purple_blist_remove_chat (ch);
    }
  }
}

static void update_user_typing (struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status) {
  if (status == tgl_typing_typing) {
    p2tgl_got_typing (TLS, U->id, 2);
  }
}

static void update_marked_read (struct tgl_state *TLS, int num, struct tgl_message *list[]) {
  connection_data *conn = TLS->ev_base;
  if (! purple_account_get_bool (conn->pa, "display-read-notifications", FALSE)) {
    return;
  }
  
  int i;
  for (i = 0; i < num; i++) if (list[i]) {
    tgl_peer_id_t to_id;
    if (tgl_get_peer_type (list[i]->to_id) == TGL_PEER_USER && tgl_get_peer_id (list[i]->to_id) == tgl_get_peer_id (TLS->our_id)) {
      to_id = list[i]->from_id;
    } else {
      to_id = list[i]->to_id;
    }
    PurpleConversation *conv = p2tgl_find_conversation_with_account (TLS, to_id);
    if (conv) {
      p2tgl_conversation_write (conv, to_id, _("Message marked as read."),
                                 PURPLE_MESSAGE_NO_LOG | PURPLE_MESSAGE_SYSTEM, (int)time (NULL));
    }
  }
}

static char *format_print_name (struct tgl_state *TLS, tgl_peer_id_t id, const char *a1, const char *a2, const char *a3, const char *a4) {
  const char *d[4];
  d[0] = a1; d[1] = a2; d[2] = a3; d[3] = a4;
  static char buf[10000];
  buf[0] = 0;
  int i;
  int p = 0;
  for (i = 0; i < 4; i++) {
    if (d[i] && strlen (d[i])) {
      p += tgl_snprintf (buf + p, 9999 - p, "%s%s", p ? " " : "", d[i]);
      assert (p < 9990);
    }
  }
  char *s = buf;
  while (*s) {
    if (*s == '\n') { *s = ' '; }
    if (*s == '#') { *s = '@'; }
    s++;
  }
  s = buf;
  int fl = (int)strlen (s);
  int cc = 0;
  while (1) {
    tgl_peer_t *P = tgl_peer_get_by_name (TLS, s);
    if (!P || !tgl_cmp_peer_id (P->id, id)) {
      break;
    }
    cc ++;
    assert (cc <= 9999);
    tgl_snprintf (s + fl, 9999 - fl, " #%d", cc);
  }
  return tgl_strdup (s);
}

static void get_password (struct tgl_state *TLS, enum tgl_value_type type, const char *prompt, int num_values,
                          void (*callback)(struct tgl_state *TLS, const char *string[], void *arg), void *arg) {
  if (type == tgl_cur_password) {
    connection_data *conn = TLS->ev_base;
    const char *P = purple_account_get_string (conn->pa, TGP_KEY_PASSWORD_TWO_FACTOR, NULL);
    
    if (str_not_empty (P)) {
      if (conn->password_retries++ < 1) {
        callback (TLS, &P, arg);
        return;
      }
    }
    request_password (TLS, callback, arg);
  }
}

/*
static void on_contact_added (struct tgl_state *TLS,void *callback_extra, int success, int size, struct tgl_user *users[]) {
  if (!success || !size) {
    PurpleBuddy *buddy = callback_extra;
    purple_blist_remove_buddy (buddy);
    purple_notify_error (_telegram_protocol, "Adding Buddy Failed", "Buddy Not Found",
                         "No contact with this phone number was found.");
  } else {
    _update_buddy (TLS, (tgl_peer_t *)users[0], TGL_UPDATE_PHOTO | TGL_UPDATE_NAME);
  }
}
*/

static void on_userpic_loaded (struct tgl_state *TLS, void *extra, int success, const char *filename) {
  connection_data *conn = TLS->ev_base;
  
  struct download_desc *dld = extra;
  struct tgl_user *U = dld->data;
  tgl_peer_t *P = tgl_peer_get (TLS, dld->get_user_info_data->peer);
  
  if (!success || !P) {
    warning ("Can not load userpic for user %s %s", U->first_name, U->last_name);
    tgp_notify_on_error_gw (TLS, NULL, success);
    free (dld->get_user_info_data);
    free (dld);
    return;
  }
  
  int imgStoreId = p2tgl_imgstore_add_with_id (filename);
  if (imgStoreId > 0) {
    used_images_add (conn, imgStoreId);

    p2tgl_buddy_icons_set_for_user (conn->pa, &P->id, filename);
    if (dld->get_user_info_data->show_info == 1) {
      PurpleNotifyUserInfo *info = p2tgl_notify_peer_info_new (TLS, P);
      p2tgl_notify_userinfo (TLS, P->id, info, NULL, NULL);
    }
  }
  
  free (dld->get_user_info_data);
  free (dld);
}

static void on_get_dialog_list_done (struct tgl_state *TLS, void *callback_extra, int success, int size,
                                     tgl_peer_id_t peers[], tgl_message_id_t *last_msg_id[], int unread_count[]) {
  
  connection_data *conn = TLS->ev_base;
  
  int i = size - 1;
  for (; i >= 0; i--) {
    tgl_peer_t *UC = tgl_peer_get (TLS, peers[i]);
    
    switch (tgl_get_peer_type (peers[i])) {
      case TGL_PEER_USER:
        assert (UC);
        if (tgl_get_peer_id (UC->id) == tgl_get_peer_id (TLS->our_id)) {
          p2tgl_connection_set_display_name (TLS, UC);
          continue;
        }
        
        if (! (UC->user.flags & TGLUF_DELETED)) {
          PurpleBuddy *buddy = p2tgl_buddy_find (TLS, UC->id);
          if (! buddy) {
            buddy = p2tgl_buddy_new (TLS, UC);
            purple_blist_add_buddy (buddy, NULL, tggroup, NULL);
            
            if (UC->user.photo_id) {
              debug ("tgl_do_get_user_info(%s)", UC->print_name);
              tgl_do_get_user_info (TLS, UC->id, 0, on_user_get_info, get_user_info_data_new (0, UC->id));
            }
          }
          
          p2tgl_prpl_got_user_status (TLS, UC->id, &UC->user.status);
        }
        break;
        
      case TGL_PEER_CHAT:
        assert (UC);
        if (UC->chat.users_num > 0 && purple_account_get_bool (conn->pa, TGP_KEY_JOIN_GROUP_CHATS, TGP_DEFAULT_JOIN_GROUP_CHATS)) {
          PurpleChat *PC = p2tgl_chat_find (TLS, UC->id);
          if (!PC) {
            PC = p2tgl_chat_new (TLS, &UC->chat);
            purple_blist_add_chat (PC, NULL, NULL);
          }
        }
        break;
    }
  }
}

void on_user_get_info (struct tgl_state *TLS, void *info_data, int success, struct tgl_user *U) {
  get_user_info_data *user_info_data = (get_user_info_data *)info_data;
  tgl_peer_t *P = tgl_peer_get (TLS, user_info_data->peer);
  
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }

  if (!U->photo || U->photo->sizes_num == 0) {
    // No profile pic to load, display it right away
    if (user_info_data->show_info) {
      PurpleNotifyUserInfo *info = p2tgl_notify_peer_info_new (TLS, P);
      p2tgl_notify_userinfo (TLS, P->id, info, NULL, NULL);
    }
    g_free (user_info_data);
  } else {
    struct download_desc *dld = malloc (sizeof(struct download_desc));
    dld->data = U;
    dld->get_user_info_data = info_data;
    tgl_do_load_photo (TLS, U->photo, on_userpic_loaded, dld);
  }
}

void on_ready (struct tgl_state *TLS) {
  debug ("on_ready().");
  connection_data *conn = TLS->ev_base;
  
  purple_connection_set_state (conn->gc, PURPLE_CONNECTED);
  purple_connection_set_display_name (conn->gc, purple_account_get_username (conn->pa));
  purple_blist_add_account (conn->pa);
  
  tggroup = purple_find_group ("Telegram");
  if (tggroup == NULL) {
    tggroup = purple_group_new ("Telegram");
    purple_blist_add_group (tggroup, NULL);
  }

  debug ("seq = %d, pts = %d, date = %d", TLS->seq, TLS->pts, TLS->date);
  tgl_do_get_difference (TLS, purple_account_get_bool (conn->pa, "history-sync-all", FALSE), tgp_notify_on_error_gw, NULL);
  tgl_do_get_dialog_list (TLS, 200, 0, on_get_dialog_list_done, NULL);
  tgl_do_update_contact_list (TLS, 0, 0);
}

static const char *tgprpl_list_icon (PurpleAccount * acct, PurpleBuddy * buddy) {
  return "telegram";
}

static void tgprpl_tooltip_text (PurpleBuddy *buddy, PurpleNotifyUserInfo *info, gboolean full) {
  debug ("tgprpl_tooltip_text()");
  assert (buddy->name);
  
  tgl_peer_t *P = find_peer_by_name (get_conn_from_buddy (buddy)->TLS, buddy->name);
  if (!P) {
    warning ("Peer %s not found in tree.", buddy->name);
    return;
  }
  gchar *status = tgp_format_user_status (&P->user.status);
  purple_notify_user_info_add_pair (info, "last online: ", status);
  g_free (status);
}

static GList *tgprpl_status_types (PurpleAccount * acct) {
  debug ("tgprpl_status_types()");
  GList *types = NULL;
  PurpleStatusType *type;
  
  type = purple_status_type_new_full (PURPLE_STATUS_AVAILABLE, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);
  
  type = purple_status_type_new_full (PURPLE_STATUS_MOBILE, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);

  type = purple_status_type_new_full (PURPLE_STATUS_OFFLINE, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);
  
  /*
    The states below are only registered internally so that we get notified about 
    state changes to away and unavailable. This is useful for deciding when to send 
    No other peer should ever have those states.
   */
  type = purple_status_type_new_full (PURPLE_STATUS_AWAY, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);
  
  type = purple_status_type_new_full (PURPLE_STATUS_UNAVAILABLE, NULL, NULL, FALSE, TRUE, FALSE);
  types = g_list_prepend (types, type);
  
  return g_list_reverse (types);
}

static void create_secret_chat_done (struct tgl_state *TLS, void *callback_extra, int success, struct tgl_secret_chat *E) {
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  write_secret_chat_file (TLS);
}

static void start_secret_chat (PurpleBlistNode *node, gpointer data) {
  PurpleBuddy *buddy = data;
  
  connection_data *conn = get_conn_from_buddy (buddy);
  const char *name = purple_buddy_get_name (buddy);
  tgl_do_create_secret_chat (conn->TLS, TGL_MK_USER(atoi (name)), create_secret_chat_done, 0);
}

static void create_chat_link_done (struct tgl_state *TLS, void *extra, int success, const char *url) {
  connection_data *conn = TLS->ev_base;
  tgl_peer_t *C = extra;
  
  if (success) {
    assert (tgl_get_peer_type (C->id) == TGL_PEER_CHAT);
    tgp_chat_show (TLS, &C->chat);
    
    char *msg = g_strdup_printf (_("Invite link: %s"), url);
    serv_got_chat_in (conn->gc, tgl_get_peer_id(C->id), "WebPage", PURPLE_MESSAGE_SYSTEM,
                      msg, time(NULL));
    g_free (msg);
  } else {
    tgp_notify_on_error_gw (TLS, NULL, success);
  }
}

static void create_chat_link (PurpleBlistNode *node, gpointer data) {
  PurpleChat *chat = (PurpleChat*)node;
  connection_data *conn = purple_connection_get_protocol_data (
                            purple_account_get_connection (purple_chat_get_account (chat)));
  export_chat_link_checked (conn->TLS, purple_chat_get_name (chat));
}

void export_chat_link_checked (struct tgl_state *TLS, const char *name) {
  tgl_peer_t *C = tgl_peer_get_by_name (TLS, name);
  if (! C) {
    failure ("Chat \"%s\" not found, not exporting link.", name);
    return;
  }
  if (C->chat.admin_id != tgl_get_peer_id (TLS->our_id)) {
    purple_notify_error (_telegram_protocol, _("Failure"), _("Creating Chat Link Failed"),
                         _("You need to be admin of the group to do that."));
    return;
  }
  tgl_do_export_chat_link (TLS, C->id, create_chat_link_done, C);
}

void leave_and_delete_chat (PurpleBlistNode *node, gpointer data) {
  PurpleChat *PC = (PurpleChat*)node;
  connection_data *conn = purple_connection_get_protocol_data (
                            purple_account_get_connection (purple_chat_get_account (PC)));
  
  tgl_peer_t *P = tgl_peer_get (conn->TLS, p2tgl_chat_get_id (PC));
  if (P && P->chat.users_num) {
    tgl_do_del_user_from_chat (conn->TLS, P->id, conn->TLS->our_id,
                               tgp_notify_on_error_gw, NULL);
    serv_got_chat_left (conn->gc, tgl_get_peer_id (P->id));
  }
  
  purple_blist_remove_chat (PC);
}

static void import_chat_link_done (struct tgl_state *TLS, void *extra, int success) {
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  purple_notify_info (_telegram_protocol, _("Success"), _("Chat joined"), _("Chat added to roomlist"));
}

void import_chat_link_checked (struct tgl_state *TLS, const char *link) {
  tgl_do_import_chat_link (TLS, link, (int) strlen(link), import_chat_link_done, NULL);
}

static GList* tgprpl_blist_node_menu (PurpleBlistNode *node) {
  debug ("tgprpl_blist_node_menu()");

  GList* menu = NULL;
  if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
    // Add encrypted chat option to the right click menu of all buddies
    PurpleBuddy* buddy = (PurpleBuddy*)node;
    PurpleMenuAction* action = purple_menu_action_new ("Start secret chat ...",
                                 PURPLE_CALLBACK(start_secret_chat), buddy, NULL);
    menu = g_list_append (menu, (gpointer)action);
  }
  if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
     // Generate Public Link
    PurpleMenuAction* action = purple_menu_action_new ("Invite users by link ...",
                                 PURPLE_CALLBACK(create_chat_link), NULL, NULL);
    menu = g_list_append (menu, (gpointer)action);
  }
  if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
    // Delete self from chat
    PurpleMenuAction* action = purple_menu_action_new ("Delete and exit ...",
                                 PURPLE_CALLBACK(leave_and_delete_chat), NULL, NULL);
    menu = g_list_append (menu, (gpointer)action);
  }
  return menu;
}

static void tgprpl_login (PurpleAccount * acct) {
  debug ("tgprpl_login()");
  
  PurpleConnection *gc = purple_account_get_connection (acct);
  
  struct tgl_state *TLS = tgl_state_alloc ();
  connection_data *conn = connection_data_init (TLS, gc, acct);
  purple_connection_set_protocol_data (gc, conn);

  TLS->base_path = get_config_dir (TLS, purple_account_get_username (acct));
  tgl_set_download_directory (TLS, get_download_dir(TLS));
  if (!assert_file_exists (gc, pk_path, _("Error, server public key not found at %s."
                      " Make sure that Telegram-Purple is installed properly."))) {
    /* Already reported. */
    return;
  }
  debug ("base configuration path: '%s'", TLS->base_path);
  
  struct rsa_pubkey the_pubkey;
  if (! read_pubkey_file (pk_path, &the_pubkey)) {
    char *cause = g_strdup_printf (_("Unable to sign on as %s: Missing file %s."),
                    purple_account_get_username (acct), pk_path);
    purple_connection_error_reason (gc, PURPLE_CONNECTION_ERROR_INVALID_SETTINGS, cause);
    purple_notify_message (_telegram_protocol, PURPLE_NOTIFY_MSG_ERROR, cause,
                           _("Make sure telegram-purple is installed properly,\n"
                            "including the .tglpub file."), NULL, NULL, NULL);
    g_free (cause);
    return;
  }

  tgl_set_verbosity (TLS, 4);
  tgl_set_rsa_key_direct (TLS, the_pubkey.e, the_pubkey.n_len, the_pubkey.n_raw);
  
  tgl_set_ev_base (TLS, conn);
  tgl_set_net_methods (TLS, &tgp_conn_methods);
  tgl_set_timer_methods (TLS, &tgp_timers);
  tgl_set_callback (TLS, &tgp_callback);
  tgl_register_app_id (TLS, TGP_APP_ID, TGP_APP_HASH); 
  
  tgl_init (TLS);

  if (! tgp_startswith (purple_account_get_username (acct), "+")) {
        char *cause = g_strdup_printf (_("Unable to sign on as %s, phone number lacks country prefix."),
                        purple_account_get_username (acct));
        purple_connection_error_reason (gc, PURPLE_CONNECTION_ERROR_INVALID_SETTINGS, cause);
        purple_notify_message (_telegram_protocol, PURPLE_NOTIFY_MSG_ERROR, cause,
                                _("Numbers must start with the full international\n"
                                "prefix code, e.g. +49 for Germany."), NULL, NULL, NULL);
        g_free (cause);
        return;
  }

  purple_connection_set_state (conn->gc, PURPLE_CONNECTING);
  
  telegram_login (TLS);
}

static void tgprpl_close (PurpleConnection * gc) {
  debug ("tgprpl_close()");
  connection_data *conn = purple_connection_get_protocol_data (gc);
  connection_data_free (conn);
}

static int tgprpl_send_im (PurpleConnection * gc, const char *who, const char *message, PurpleMessageFlags flags) {
  debug ("tgprpl_send_im()");
  connection_data *conn = purple_connection_get_protocol_data(gc);
 
  // this is part of a workaround to support clients without
  // the request API (request.h), see telegram-base.c:request_code()
  if (conn->in_fallback_chat) {

    // OTR plugins may try to insert messages that don't contain the code
    if (tgp_startswith (message, "?OTR")) {
        info ("Fallback SMS auth, skipping OTR messsage: '%s'", message);
        return -1;
    }
    request_code_entered (conn->TLS, message);
    conn->in_fallback_chat = 0;
    return 1;
  }
  
  /* 
     Make sure that we only send messages to an existing peer by
     searching it in the peer tree. This allows us to give immediate feedback 
     by returning an error-code in case the peer doesn't exist
   */
  tgl_peer_t *peer = find_peer_by_name (conn->TLS, who);
  if (peer) {
    if (tgl_get_peer_type (peer->id) == TGL_PEER_ENCR_CHAT && peer->encr_chat.state != sc_ok) {
      warning ("secret chat not ready for sending messages or deleted");
      return -1;
    }
    return tgp_msg_send (conn->TLS, message, peer->id);
  }
  
  warning ("peer not found");
  return -1;
}

static unsigned int tgprpl_send_typing (PurpleConnection * gc, const char *who, PurpleTypingState typing) {
  debug ("tgprpl_send_typing()");
  int id = atoi (who);
  connection_data *conn = purple_connection_get_protocol_data(gc);
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

static void tgprpl_get_info (PurpleConnection * gc, const char *who) {
  debug ("tgprpl_get_info()");
  connection_data *conn = purple_connection_get_protocol_data(gc);
  
  tgl_peer_t *peer = find_peer_by_name (conn->TLS, who);
  if (! peer) { return; }
  
  get_user_info_data* info_data = get_user_info_data_new (1, peer->id);
  
  switch (tgl_get_peer_type (peer->id)) {
    case TGL_PEER_USER:
    case TGL_PEER_CHAT:
      tgl_do_get_user_info (conn->TLS, peer->id, 0, on_user_get_info, info_data);
      break;
      
    case TGL_PEER_ENCR_CHAT: {
      tgl_peer_t *parent_peer = tgp_encr_chat_get_partner(conn->TLS, &peer->encr_chat);
      if (parent_peer) {
        tgl_do_get_user_info (conn->TLS, parent_peer->id, 0, on_user_get_info, info_data);
      }
      break;
    }
  }
}

static void tgprpl_set_status (PurpleAccount * acct, PurpleStatus * status) {
  debug ("tgprpl_set_status(%s)", purple_status_get_name (status));
  debug ("tgprpl_set_status(currstatus=%s)", purple_status_get_name(purple_account_get_active_status(acct)));

  PurpleConnection *gc = purple_account_get_connection(acct);
  if (!gc) { return; }
  connection_data *conn = purple_connection_get_protocol_data (gc);

  int present = p2tgl_status_is_present (status);
  if (present && p2tgl_send_notifications (acct)) {
    pending_reads_send_all (conn->pending_reads, conn->TLS);
  }
}

static void tgprpl_add_buddy (PurpleConnection * gc, PurpleBuddy * buddy, PurpleGroup * group) {
  connection_data *conn = purple_connection_get_protocol_data(gc);
  
  tgl_peer_t *peer = tgl_peer_get (conn->TLS, TGL_MK_USER (atoi (buddy->name)));
  if (peer) {
    _update_buddy (conn->TLS, peer, TGL_UPDATE_NAME | TGL_UPDATE_PHOTO);
  }
}

static void tgprpl_remove_buddy (PurpleConnection *gc, PurpleBuddy *buddy, PurpleGroup *group) {
  debug ("tgprpl_remove_buddy()");
  if (!buddy) {
    return;
  }

  connection_data *conn = purple_connection_get_protocol_data (gc);
  tgl_peer_t *peer = find_peer_by_name (conn->TLS, buddy->name);
  if (!peer) {
    return;
  }
  
  if (tgl_get_peer_type(peer->id) == TGL_PEER_ENCR_CHAT) {
    /* TODO: implement the api call cancel secret chats. Currently the chat will only be marked as
     deleted on our side so that it won't be added on startup
     (when the secret chat file is loaded) */
    bl_do_peer_delete (conn->TLS, peer->encr_chat.id);
  }
}

static void tgprpl_chat_invite (PurpleConnection * gc, int id, const char *message, const char *name) {
  debug ("tgprpl_chat_invite()");

  connection_data *conn = purple_connection_get_protocol_data (gc);
  tgl_peer_t *chat = tgl_peer_get(conn->TLS, TGL_MK_CHAT (id));
  tgl_peer_t *user = tgl_peer_get(conn->TLS, TGL_MK_USER (atoi(name)));
  
  if (! chat || ! user) {
    purple_notify_error (_telegram_protocol, _("Not found"), _("Cannot invite buddy to chat."), _("Specified user is not existing."));
    return;
  }
  
  tgl_do_add_user_to_chat (conn->TLS, chat->id, user->id, 0, tgp_notify_on_error_gw, chat);
}

static int tgprpl_send_chat (PurpleConnection * gc, int id, const char *message, PurpleMessageFlags flags) {
  debug ("tgprpl_send_chat()");
  connection_data *conn = purple_connection_get_protocol_data (gc);
  int ret = tgp_msg_send (conn->TLS, message, TGL_MK_CHAT(id));
  if (ret != 0) {
    p2tgl_got_chat_in (conn->TLS, TGL_MK_CHAT(id), conn->TLS->our_id, message,
                       PURPLE_MESSAGE_RECV, time(NULL));
  }
  return ret;
}

/*
static void tgprpl_set_buddy_icon (PurpleConnection * gc, PurpleStoredImage * img) {
  debug ("tgprpl_set_buddy_icon()");
  
  connection_data *conn = purple_connection_get_protocol_data (gc);
  if (purple_imgstore_get_filename (img)) {
    char* filename = g_strdup_printf ("%s/icons/%s", purple_user_dir(), purple_imgstore_get_filename (img));
    debug (filename);
    
    tgl_do_set_profile_photo (conn->TLS, filename, tgp_notify_on_error_gw, NULL);
    
    g_free (filename);
  }
}
*/

static gboolean tgprpl_can_receive_file (PurpleConnection * gc, const char *who) {
  return TRUE;
}

PurplePlugin *_telegram_protocol = NULL;

static PurplePluginProtocolInfo prpl_info = {
  OPT_PROTO_NO_PASSWORD | OPT_PROTO_IM_IMAGE,
  NULL,                    // user_¡splits, initialized in tgprpl_init()
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
  tgprpl_blist_node_menu,
  tgprpl_chat_join_info,
  tgprpl_chat_info_defaults,
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
  NULL,                    // add_buddies
  tgprpl_remove_buddy,
  NULL,                    // remove_buddies
  NULL,                    // add_permit
  NULL,                    // add_deny
  NULL,                    // rem_permit
  NULL,                    // rem_deny
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
  NULL,                    // group_buddy
  NULL,                    // rename_group
  NULL,                    // buddy_free
  NULL,                    // convo_closed
  NULL,                    // normalize
  NULL,                    // tgprpl_set_buddy_icon
  NULL,                    // remove_group
  NULL,
  NULL,                    // set_chat_topic
  NULL,                    // find_blist_chat
  tgprpl_roomlist_get_list,
  tgprpl_roomlist_cancel,
  NULL,                    // roomlist_expand_category
  tgprpl_can_receive_file,
  tgprpl_send_file,       
  tgprpl_new_xfer,        
  NULL,                    // offline_message
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
  
  // set domain name that is used for translation
#if ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
  
#define ADD_VALUE(list, desc, v) { \
  PurpleKeyValuePair *kvp = g_new0(PurpleKeyValuePair, 1); \
  kvp->key = g_strdup((desc)); \
  kvp->value = g_strdup((v)); \
  list = g_list_prepend(list, kvp); \
}
  
  // Login
  opt = purple_account_option_string_new (_("Password (two factor authentication)"),
                                          TGP_KEY_PASSWORD_TWO_FACTOR, NULL);
  purple_account_option_set_masked (opt, TRUE);
  prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, opt);
  
  opt = purple_account_option_bool_new (
          _("Fallback SMS verification\n(Helps when not using Pidgin and you aren't being prompted for the code)"),
          "compat-verification", 0);
  prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, opt);
 

  // Messaging
  
  GList *verification_values = NULL;
  ADD_VALUE(verification_values, "Ask", "ask");
  ADD_VALUE(verification_values, "Always", "always");
  ADD_VALUE(verification_values, "Never", "never");
  
  opt = purple_account_option_list_new (_("Accept Secret Chats"),
                                        TGP_KEY_ACCEPT_SECRET_CHATS,
                                        verification_values);
  prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, opt);
  
  opt = purple_account_option_int_new (_("Display peers offline after (days)"),
                                      TGP_KEY_INACTIVE_DAYS_OFFLINE,
                                      TGP_DEFAULT_INACTIVE_DAYS_OFFLINE);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);
  
  opt = purple_account_option_bool_new (_("Fetch past history on first login"),
                                        TGP_KEY_HISTORY_SYNC_ALL,
                                        TGP_DEFAULT_HISTORY_SYNC_ALL);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);
  
  opt = purple_account_option_int_new (_("Don't fetch messages older than (days)\n"
                                       "(0 for unlimited)"),
                                       TGP_KEY_HISTORY_RETRIEVAL_THRESHOLD,
                                       TGP_DEFAULT_HISTORY_RETRIEVAL_THRESHOLD);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);
  
  // Chats

  opt = purple_account_option_bool_new (_("Add group chats to buddy list"),
                                        TGP_KEY_JOIN_GROUP_CHATS,
                                        TGP_DEFAULT_JOIN_GROUP_CHATS);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);


  // Read notifications
  
  opt = purple_account_option_bool_new (_("Display read notifications"),
                                        TGP_KEY_DISPLAY_READ_NOTIFICATIONS,
                                        TGP_DEFAULT_DISPLAY_READ_NOTIFICATIONS);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);
  
  opt = purple_account_option_bool_new (_("Send read notifications when present."),
                                        TGP_KEY_SEND_READ_NOTIFICATIONS,
                                        TGP_DEFAULT_SEND_READ_NOTIFICATIONS);
  prpl_info.protocol_options = g_list_append (prpl_info.protocol_options, opt);
  
  _telegram_protocol = plugin;
}

static GList *tgprpl_actions (PurplePlugin * plugin, gpointer context) {
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
  PACKAGE_VERSION " libtgl: " TGL_VERSION,
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
