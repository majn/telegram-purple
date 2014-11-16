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
#include <tgp-2prpl.h>
#include <tgp-net.h>
#include <tgp-timers.h>
#include <telegram-base.h>
#include <telegram-purple.h>
#include <msglog.h>

static PurplePlugin *_telegram_protocol = NULL;
PurpleGroup *tggroup;
const char *config_dir = ".telegram-purple";
const char *pk_path = "/etc/telegram-purple/server.pub";

void tgprpl_login_on_connected();
void on_chat_get_info (struct tgl_state *TLS, void *extra, int success, struct tgl_chat *C);
void on_user_get_info (struct tgl_state *TLS, void *show_info, int success, struct tgl_user *U);
static void add_all_users_to_chat (PurpleConversation *pc, struct tgl_chat *chat);

static telegram_conn *get_conn_from_buddy (PurpleBuddy *buddy) {
  telegram_conn *c = purple_connection_get_protocol_data (
      purple_account_get_connection (purple_buddy_get_account (buddy)));
  return c;
}

static const char *format_time(time_t date) {
  struct tm *datetime = localtime(&date);
  return purple_utf8_strftime("%Y.%m.%d %H:%M:%S", datetime);
}

static char *format_status(struct tgl_user_status *status) {
  return status->online ? "Online" : "Mobile";
}

static char *format_img_full(int imgstore) {
  return g_strdup_printf ("<br><img id=\"%u\">", imgstore);
}

static char *format_img_thumb(int imgstore, char *filename) {
  return g_strdup_printf("<a href=\"%s\"><img id=\"%u\"></a><br/><a href=\"%s\">%s</a>",
               filename, imgstore, filename, filename);
}

static void on_update_user_name (struct tgl_state *TLS, tgl_peer_t *user) {
  debug("on_update_user_name(id=%d, type=%d)\n", user->id.id, user->id.type);
  p2tgl_got_alias(TLS, user->id, p2tgl_strdup_alias(user));
}

static void on_update_chat_participants (struct tgl_state *TLS, tgl_peer_t *chat) {
  debug("on_update_chat_participants(chat.id=%d)\n", chat->id.id);
  // Refresh chat participants
  PurpleConversation *pc = purple_find_chat(tg_get_conn(TLS), chat->chat.id.id);
  if (pc) {
    purple_conv_chat_clear_users (purple_conversation_get_chat_data(pc));
    add_all_users_to_chat (pc, chat);
  }
}

static int our_msg (struct tgl_state *TLS, struct tgl_message *M) {
  return tgl_get_peer_id(M->from_id) == TLS->our_id;
}

static PurpleConversation *chat_show (PurpleConnection *gc, int id) {
  debug ("show chat");
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  
  PurpleConversation *convo = purple_find_chat(gc, id);
  if (! convo) {
    // chat exists
    gchar *name = g_strdup_printf ("%d", id);
    if (! g_hash_table_contains (conn->joining_chats, name)) {
      // mark chat as already joining
      g_hash_table_insert(conn->joining_chats, name, 0);
      
      // join chat first
      tgl_do_get_chat_info (conn->TLS, TGL_MK_CHAT(id), 0, on_chat_get_info, NULL);
    }
    g_free(name);
  }
  return convo;
}

static int chat_add_message(struct tgl_state *TLS, struct tgl_message *M) {
  telegram_conn *conn = TLS->ev_base;
  
  if (chat_show (conn->gc, M->to_id.id)) {
    p2tgl_got_chat_in(TLS, M->to_id, M->from_id, M->message, PURPLE_MESSAGE_RECV, M->date);
    return 1;
  } else {
    // add message once the chat was initialised
    g_queue_push_tail (conn->new_messages, M);
    return 0;
  }
}

static void add_all_users_to_chat (PurpleConversation *pc, struct tgl_chat *chat) {
  struct tgl_chat_user *curr =  chat->user_list;
  if (!curr) {
    warning ("add_all_users_to_chat: chat contains no user list, cannot add users\n.");
    return;
  }
  
  int i;
  for (i = 0; i < chat->user_list_size; i++) {
    struct tgl_chat_user *uid = (curr + i);
    int flags = (chat->admin_id == uid->user_id ? PURPLE_CBFLAGS_FOUNDER : PURPLE_CBFLAGS_NONE);
    p2tgl_conv_add_user(pc, *uid, NULL, flags, 0);
  }
}

static const char *tgprpl_list_icon(PurpleAccount * acct, PurpleBuddy * buddy) {
  return "telegram";
}

static void tgprpl_tooltip_text(PurpleBuddy * buddy, PurpleNotifyUserInfo * info, gboolean full) {
  debug ("tgprpl_tooltip_text()\n", buddy->name);
  
  tgl_peer_id_t *peer = purple_buddy_get_protocol_data(buddy);
  if (!peer) {
    purple_notify_user_info_add_pair_plaintext(info, "Status", "Offline");
    return;
  }
  tgl_peer_t *P = tgl_peer_get (get_conn_from_buddy(buddy)->TLS, *peer);
  if (!P) {
    warning ("tgprpl_tooltip_text: warning peer with id %d not found in tree.\n", peer->id);
    return;
  }
  purple_notify_user_info_add_pair_plaintext(info, "Status", format_status(&P->user.status));
  purple_notify_user_info_add_pair_plaintext(info, "Last seen: ", format_time(P->user.status.when));
}

gboolean queries_timerfunc (gpointer data) {
   debug ("queries_timerfunc()\n");
   telegram_conn *conn = data;

   if (conn->updated) {
     debug ("State updated, storing current session...\n");
     conn->updated = 0;
     write_state_file (conn->TLS);
   }
   return 1;
}

void telegram_on_ready (struct tgl_state *TLS) {
  debug ("telegram_on_ready().\n");
  telegram_conn *conn = TLS->ev_base;
  purple_connection_set_state(conn->gc, PURPLE_CONNECTED);
  purple_connection_set_display_name(conn->gc, purple_account_get_username(conn->pa));
  purple_blist_add_account(conn->pa);
  tggroup = purple_find_group("Telegram");
  if (tggroup == NULL) {
  purple_debug_info (PLUGIN_ID, "PurpleGroup = NULL, creating");
  tggroup = purple_group_new ("Telegram");
  purple_blist_add_group (tggroup, NULL);
  }
 
  tgl_do_get_difference (TLS, 0, 0, 0);
  tgl_do_get_dialog_list (TLS, 0, 0); 
  tgl_do_update_contact_list (TLS, 0, 0);
  
  conn->timer = purple_timeout_add (5000, queries_timerfunc, conn);
}

static void message_received_handler (struct tgl_state *TLS, struct tgl_message *M);
static void user_update_handler (struct tgl_state *TLS, struct tgl_user *U, unsigned flags);
static void chat_update_handler (struct tgl_state *TLS, struct tgl_chat *C, unsigned flags);
void on_user_typing (struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status);
struct tgl_update_callback tgp_callback = {
  .logprintf = debug,
  .new_msg = message_received_handler, 
  .msg_receive = message_received_handler,
  .user_update = user_update_handler,
  .chat_update = chat_update_handler,
  .type_notification = on_user_typing
};

static void tgprpl_login(PurpleAccount * acct) {
  purple_debug_info (PLUGIN_ID, "tgprpl_login()\n");
  PurpleConnection *gc = purple_account_get_connection(acct);
  char const *username = purple_account_get_username(acct);
  
  struct tgl_state *TLS = tgl_state_alloc ();
  
  const char *dir = config_dir;
  struct passwd *pw = getpwuid(getuid());
  int len = strlen (dir) + strlen (pw->pw_dir) + 2 + strlen (username);  
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
  conn->new_messages = g_queue_new();
  conn->joining_chats = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
  purple_connection_set_protocol_data (gc, conn);
  
  tgl_set_ev_base (TLS, conn);
  tgl_set_net_methods (TLS, &tgp_conn_methods);
  tgl_set_timer_methods (TLS, &tgp_timers); 
  tgl_set_callback (TLS, &tgp_callback);

  tgl_init (TLS);
  purple_connection_set_state (conn->gc, PURPLE_CONNECTING);


  telegram_login (TLS);
}

/**
 * This must be implemented.
 */
static void tgprpl_close(PurpleConnection * gc) {
  purple_debug_info(PLUGIN_ID, "tgprpl_close()\n");
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  purple_timeout_remove(conn->timer);
  tgl_free_all (conn->TLS);
}

void message_received_handler(struct tgl_state *TLS, struct tgl_message *M) {
  if (M->service) {
    // TODO: handle service messages properly, currently adding them
    // causes a segfault for an unknown reason
    debug ("service message, skipping...\n");
    return;
  }
  if (M->flags & (FLAG_MESSAGE_EMPTY | FLAG_DELETED)) {
    return;
  }
  if (! (M->flags & FLAG_CREATED)) {
    return;
  }
  if (! tgl_get_peer_type (M->to_id)) {
    warning ("Bad msg\n");
    return;
  }

  switch (tgl_get_peer_type (M->to_id)) {
    case TGL_PEER_CHAT:
      debug ("PEER_CHAT\n");
#ifdef ADIUM_PLUGIN
      // don't add our own chat messages in Adium, or
      // else they will show up twice
      // if (our_msg(TLS, M)) { return; }
#endif
      chat_add_message (TLS, M);
      break;
      
    case TGL_PEER_USER:
      debug ("PEER_USER\n");
      if (our_msg(TLS, M)) {
        p2tgl_got_im (TLS, M->to_id, M->message, PURPLE_MESSAGE_SEND, M->date);
      } else {
        p2tgl_got_im (TLS, M->from_id, M->message, PURPLE_MESSAGE_RECV, M->date);
      }
      break;
      
    case TGL_PEER_ENCR_CHAT:
      break;
      
    case TGL_PEER_GEO_CHAT:
      break;
  }
  
  telegram_conn *conn = TLS->ev_base;
  conn->updated = 1;
}

void on_new_user_status (struct tgl_state *TLS, void *peer) {
  tgl_peer_t *p = peer;
  p2tgl_prpl_got_user_status(TLS, p->id, &p->user.status);
}

void on_user_typing (struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status) {
  if (status == tgl_typing_typing) {
    p2tgl_got_typing(TLS, U->id, 2);
  }
}

void user_update_handler (struct tgl_state *TLS, struct tgl_user *user, unsigned flags) {
  if (!(flags & FLAG_CREATED)) { return; }
 
  if (TLS->our_id == tgl_get_peer_id (user->id)) {
    p2tgl_connection_set_display_name (TLS, (tgl_peer_t *)user);
  } else {
    PurpleBuddy *buddy = p2tgl_buddy_find(TLS, user->id);
    if (!buddy) {
      buddy = p2tgl_buddy_new (TLS, (tgl_peer_t *)user);
      purple_blist_add_buddy (buddy, NULL, tggroup, NULL);
      tgl_do_get_user_info (TLS, user->id, 0, on_user_get_info, 0);
    }
    purple_buddy_set_protocol_data (buddy, (gpointer)user);
    p2tgl_prpl_got_user_status (TLS, user->id, &user->status);
  }
}

void chat_update_handler (struct tgl_state *TLS, struct tgl_chat *chat, unsigned flags) {
  if (!(flags & FLAG_CREATED)) { return; }
  
  telegram_conn *conn = TLS->ev_base;
  
  tgl_do_get_chat_info (TLS, chat->id, 0, on_chat_get_info, 0);
  
  PurpleChat *ch = p2tgl_chat_find (TLS, chat->id);
  if (!ch) {
    ch = p2tgl_chat_new (TLS, chat);
    purple_blist_add_chat(ch, NULL, NULL);
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

void on_userpic_loaded (struct tgl_state *TLS, void *extra, int success, char *filename) {
  telegram_conn *conn = TLS->ev_base;

  gchar *data = NULL;
  size_t len;
  GError *err = NULL;
  g_file_get_contents (filename, &data, &len, &err);
  int imgStoreId = purple_imgstore_add_with_id (g_memdup(data, (guint)len), len, NULL);
  
  struct download_desc *dld = extra;
  struct tgl_user *U = dld->data;

  char *who = g_strdup_printf ("%d", tgl_get_peer_id (U->id));
  if (dld->type == 1) {
    PurpleNotifyUserInfo *info = create_user_notify_info(U);
    char *profile_image = profile_image = format_img_full(imgStoreId);
    purple_notify_user_info_add_pair (info, "Profile image", profile_image);
    purple_notify_userinfo (conn->gc, who, info, NULL, NULL);
    g_free (profile_image);
  }
  purple_buddy_icons_set_for_user(conn->pa, who, g_memdup(data, (guint)len), len, NULL);
  g_free(who);
}

void on_user_get_info (struct tgl_state *TLS, void *show_info, int success, struct tgl_user *U)
{
  assert (success);
  
  if (U->photo.sizes_num == 0 && show_info) {
    PurpleNotifyUserInfo *info = create_user_notify_info(U);
    p2tgl_notify_userinfo(TLS, U->id, info, NULL, NULL);
  } else {
    struct download_desc *dld = malloc (sizeof(struct download_desc));
    dld->data = U;
    dld->type = show_info ? 1 : 2;
    tgl_do_load_photo (TLS, &U->photo, on_userpic_loaded, dld);
  }
}

/**
 * This PRPL function should return a positive value on success.
 * If the message is too big to be sent, return -E2BIG.  If
 * the account is not connected, return -ENOTCONN.  If the
 * PRPL is unable to send the message for another reason, return
 * some other negative value.  You can use one of the valid
 * errno values, or just big something.  If the message should
 * not be echoed to the conversation window, return 0.
 */
static int tgprpl_send_im(PurpleConnection * gc, const char *who, const char *message, PurpleMessageFlags flags) {
  debug ("tgprpl_send_im()\n");
  
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  PurpleAccount *pa = conn->pa;
  
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
  tgl_do_send_message (conn->TLS, peer->id, message, (int)strlen (message), 0, 0);
  return 1;
}

/**
 * Send a message to a chat.
 * This PRPL function should return a positive value on success.
 * If the message is too big to be sent, return -E2BIG.  If
 * the account is not connected, return -ENOTCONN.  If the
 * PRPL is unable to send the message for another reason, return
 * some other negative value.  You can use one of the valid
 * errno values, or just big something.
 *
 * @param id    The id of the chat to send the message to.
 * @param message The message to send to the chat.
 * @param flags   A bitwise OR of #PurpleMessageFlags representing
 *        message flags.
 * @return     A positive number or 0 in case of success,
 *        a negative error number in case of failure.
 */
static int tgprpl_send_chat(PurpleConnection * gc, int id, const char *message, PurpleMessageFlags flags) {
  purple_debug_info(PLUGIN_ID, "tgprpl_send_chat()\n");
  telegram_conn *conn = purple_connection_get_protocol_data (gc);
  tgl_do_send_message (conn->TLS, TGL_MK_CHAT(id), message, (int)strlen (message), 0, 0);
  
  p2tgl_got_chat_in(conn->TLS, TGL_MK_CHAT(id), TGL_MK_USER(conn->TLS->our_id), message,
            PURPLE_MESSAGE_RECV, time(NULL));
  return 1;
}

/**
 * Add a buddy to a group on the server.
 *
 * This PRPL function may be called in situations in which the buddy is
 * already in the specified group. If the protocol supports
 * authorization and the user is not already authorized to see the
 * status of \a buddy, \a add_buddy should request authorization.
 *
 * @deprecated Since 2.8.0, add_buddy_with_invite is preferred.
 * @see add_buddy_with_invite
 */
static void tgprpl_add_buddy(PurpleConnection * gc, PurpleBuddy * buddy, PurpleGroup * group) {
  purple_debug_info(PLUGIN_ID, "tgprpl_add_buddy()\n");
}

static void tgprpl_add_buddies(PurpleConnection * gc, GList * buddies, GList * groups) {
  purple_debug_info(PLUGIN_ID, "tgprpl_add_buddies()\n");
}

static void tgprpl_remove_buddy(PurpleConnection * gc, PurpleBuddy * buddy, PurpleGroup * group) {
  purple_debug_info(PLUGIN_ID, "tgprpl_remove_buddy()\n");
}

static void tgprpl_remove_buddies(PurpleConnection * gc, GList * buddies, GList * groups){
  purple_debug_info(PLUGIN_ID, "tgprpl_remove_buddies()\n");

}

static void tgprpl_convo_closed(PurpleConnection * gc, const char *who){
  purple_debug_info(PLUGIN_ID, "tgprpl_convo_closed()\n");

}

static void tgprpl_add_deny(PurpleConnection * gc, const char *name){
  purple_debug_info(PLUGIN_ID, "tgprpl_add_deny()\n");

}

static void tgprpl_rem_deny(PurpleConnection * gc, const char *name){
  purple_debug_info(PLUGIN_ID, "tgprpl_rem_deny()\n");

}

/**
 * @return If this protocol requires the PURPLE_TYPING message to
 *     be sent repeatedly to signify that the user is still
 *     typing, then the PRPL should return the number of
 *     seconds to wait before sending a subsequent notification.
 *     Otherwise the PRPL should return 0.
 */
static unsigned int tgprpl_send_typing(PurpleConnection * gc, const char *who, PurpleTypingState typing) {
  purple_debug_info(PLUGIN_ID, "tgprpl_send_typing()\n");
  /*telegram_conn *conn = purple_connection_get_protocol_data(gc);
  PurpleBuddy *b = purple_find_buddy(conn->pa, who);
  if (b) {
    tgl_peer_id_t *peer = purple_buddy_get_protocol_data(b);
    if (peer) {
      tgl_do_update_typing (conn->tg, *peer);
    }
  }*/
  return 0;
}

/**
 * Set the buddy icon for the given connection to @a img.  The prpl
 * does NOT own a reference to @a img; if it needs one, it must
 * #purple_imgstore_ref(@a img) itself.
 */
static void tgprpl_set_buddy_icon(PurpleConnection * gc, PurpleStoredImage * img) {
  purple_debug_info(PLUGIN_ID, "tgprpl_set_buddy_icon()\n");

}

/**
 * File transfer callback
 */
static gboolean tgprpl_can_receive_file(PurpleConnection * gc, const char *who) {
  purple_debug_info(PLUGIN_ID, "tgprpl_can_receive_file()\n");
  return 0;
}

/**
 *  Checks whether offline messages to @a buddy are supported.

 *  @return @c 1 if @a buddy can be sent messages while they are
 *      offline, or @c 0 if not.
 */
static gboolean tgprpl_offline_message(const PurpleBuddy * buddy) {
  purple_debug_info(PLUGIN_ID, "tgprpl_offline_message()\n");
  return 0;
}

/**
 * Returns a list of #PurpleStatusType which exist for this account;
 * this must be implemented, and must add at least the offline and
 * online states.
 */
static GList *tgprpl_status_types(PurpleAccount * acct) {
  purple_debug_info(PLUGIN_ID, "tgprpl_status_types()\n");
  GList *types = NULL;
  PurpleStatusType *type;
  type = purple_status_type_new_with_attrs(PURPLE_STATUS_AVAILABLE, NULL, NULL,
    1, 1, 0, "message", "Message", purple_value_new(PURPLE_TYPE_STRING), NULL);
  types = g_list_prepend(types, type);
  
  type = purple_status_type_new_with_attrs(PURPLE_STATUS_MOBILE, NULL, NULL, 1,
    1, 0, "message", "Message", purple_value_new(PURPLE_TYPE_STRING), NULL);
  types = g_list_prepend(types, type);

  type = purple_status_type_new(PURPLE_STATUS_OFFLINE, NULL, NULL, 1);
  types = g_list_append(types, type);

  return g_list_reverse(types);
}

static void tgprpl_set_status(PurpleAccount * acct, PurpleStatus * status) {
  purple_debug_info(PLUGIN_ID, "tgprpl_set_status()\n");
}

/**
 * Should arrange for purple_notify_userinfo() to be called with
 * @a who's user info.
 */
static void tgprpl_get_info(PurpleConnection * gc, const char *username) {
  purple_debug_info(PLUGIN_ID, "tgprpl_get_info()\n");
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  tgl_peer_id_t u = TGL_MK_USER(atoi(username));
  tgl_do_get_user_info (conn->TLS, u, 0, on_user_get_info, (void *)1l);
  purple_debug_info(PLUGIN_ID, "tgprpl_get_info ready()\n");
}

/**
 * change a buddy's group on a server list/roster
 */
static void tgprpl_group_buddy(PurpleConnection * gc, const char *who, const char *old_group, const char *new_group) {
  purple_debug_info(PLUGIN_ID, "tgprpl_group_buddy()\n");
}

/**
 * rename a group on a server list/roster
 */
static void tgprpl_rename_group(PurpleConnection * gc, const char *old_name, PurpleGroup * group, GList * moved_buddies) {
  purple_debug_info(PLUGIN_ID, "tgprpl_rename_group()\n");
}

/**
 * Returns a list of #proto_chat_entry structs, which represent
 * information required by the PRPL to join a chat. libpurple will
 * call join_chat along with the information filled by the user.
 *
 * @return A list of #proto_chat_entry structs
 */
static GList *tgprpl_chat_join_info(PurpleConnection * gc) {
  purple_debug_info(PLUGIN_ID, "tgprpl_chat_join_info()\n");
  struct proto_chat_entry *pce;

  pce = g_new0(struct proto_chat_entry, 1);
  pce->label = "_Subject:";
  pce->identifier = "subject";
  pce->required = TRUE;
  return g_list_append(NULL, pce);
}

/**
 * Returns a hashtable which maps #proto_chat_entry struct identifiers
 * to default options as strings based on chat_name. The resulting
 * hashtable should be created with g_hash_table_new_full(g_str_hash,
 * g_str_equal, NULL, g_free);. Use #get_chat_name if you instead need
 * to extract a chat name from a hashtable.
 *
 * @param chat_name The chat name to be turned into components
 * @return Hashtable containing the information extracted from chat_name
 */
static GHashTable *tgprpl_chat_info_deflt(PurpleConnection * gc, const char *chat_name) {
  purple_debug_info(PLUGIN_ID, "tgprpl_chat_info_defaults()\n");
  return NULL;
}

/**
 * Called when the user requests joining a chat. Should arrange for
 * #serv_got_joined_chat to be called.
 *
 * @param components A hashtable containing information required to
 *           join the chat as described by the entries returned
 *           by #chat_info. It may also be called when accepting
 *           an invitation, in which case this matches the
 *           data parameter passed to #serv_got_chat_invite.
 */
static void tgprpl_chat_join(PurpleConnection * gc, GHashTable * data) {
  purple_debug_info(PLUGIN_ID, "tgprpl_chat_join()\n");

  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  const char *groupname = g_hash_table_lookup(data, "subject");

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
  add_all_users_to_chat(conv, C);
  
  struct tgl_message *M = 0;
  while ((M = g_queue_pop_head (conn->new_messages))) {
    int id = tgl_get_peer_id (M->from_id);
    if (!chat_add_message(TLS, M)) {
      warning ("WARNING, chat %d still not existing... \n", tgl_get_peer_id (C->id));
      break;
    }
  }
  
  gchar *name = g_strdup_printf ("%d", tgl_get_peer_id(C->id));
  g_hash_table_remove (conn->joining_chats, name);
  g_free (name);
}

/**
 * Invite a user to join a chat.
 *
 * @param id    The id of the chat to invite the user to.
 * @param message A message displayed to the user when the invitation
 *        is received.
 * @param who   The name of the user to send the invation to.
 */
static void tgprpl_chat_invite(PurpleConnection * gc, int id, const char *message, const char *name) { purple_debug_info(PLUGIN_ID, "tgprpl_chat_invite()\n"); } 
/**
 * Returns a chat name based on the information in components. Use
 * #chat_info_defaults if you instead need to generate a hashtable
 * from a chat name.
 *
 * @param components A hashtable containing information about the chat.
 */
static char *tgprpl_get_chat_name(GHashTable * data) {
  purple_debug_info(PLUGIN_ID, "tgprpl_get_chat_name()\n");
  return g_strdup(g_hash_table_lookup(data, "subject"));
}

/**
 * File transfer callback.
 */
static PurpleXfer *tgprpl_new_xfer(PurpleConnection * gc, const char *who) {
  purple_debug_info(PLUGIN_ID, "tgprpl_new_xfer()\n");
  return (PurpleXfer *)NULL;
}

/**
 * File transfer callback.
 */
static void tgprpl_send_file(PurpleConnection * gc, const char *who, const char *file) {
  purple_debug_info(PLUGIN_ID, "tgprpl_send_file()\n");

}

// SEE prpl.h
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
  NULL,           		   // get_account_text_table
  NULL,                    // initiate_media
  NULL,                    // get_media_caps
  NULL,                    // get_moods
  NULL,                    // set_public_alias
  NULL,                    // get_public_alias
  NULL,                    // add_buddy_with_invite
  NULL                     // add_buddies_with_invite
};

static void tgprpl_init(PurplePlugin *plugin) {
  PurpleAccountOption *option;
  GList *verification_values = NULL;
  
  // Extra Options
 #define ADD_VALUE(list, desc, v) { \
   PurpleKeyValuePair *kvp = g_new0(PurpleKeyValuePair, 1); \
   kvp->key = g_strdup((desc)); \
   kvp->value = g_strdup((v)); \
   list = g_list_prepend(list, kvp); \
 }
  ADD_VALUE(verification_values, "Phone", TELEGRAM_AUTH_MODE_PHONE);
  ADD_VALUE(verification_values, "SMS", TELEGRAM_AUTH_MODE_SMS);

  option = purple_account_option_list_new("Verification type", "verification_type", verification_values);
  prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

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
  TG_VERSION " libtgl: " TGL_VERSION " ",
  "Telegram",
  TG_DESCRIPTION,
  TG_AUTHOR,
  "https://github.com/majn/telegram-purple",
  NULL,                 // on load
  NULL,                 // on unload
  NULL,                 // on destroy
  NULL,                 // ui specific struct
  &prpl_info,
  NULL,                 // prefs info
  tgprpl_actions,
  NULL,                 // reserved
  NULL,                 // reserved
  NULL,                 // reserved
  NULL                  // reserved
};

PURPLE_INIT_PLUGIN(telegram, tgprpl_init, plugin_info)
