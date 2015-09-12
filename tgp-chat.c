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

#include "tgp-chat.h"
#include "msglog.h"
#include "tgp-utils.h"
#include "telegram-base.h"

#include <tgl.h>
#include <assert.h>


GHashTable *tgp_chat_info_new (struct tgl_state *TLS, struct tgl_chat *chat) {
  gchar *admin = g_strdup_printf ("%d", chat->admin_id);
  gchar *title = g_strdup (chat->print_title);
  gchar *name  = p2tgl_strdup_id (chat->id);
  
  GHashTable *ht = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_free);
  g_hash_table_insert (ht, "subject", title);
  g_hash_table_insert (ht, "id", name);
  g_hash_table_insert (ht, "owner", admin);
  
  return ht;
}

PurpleChat *p2tgl_chat_new (struct tgl_state *TLS, struct tgl_chat *chat) {
  return purple_chat_new (tg_get_acc(TLS), chat->title, tgp_chat_info_new (TLS, chat));
}

void p2tgl_chat_update (PurpleChat *chat, tgl_peer_id_t id, int admin_id, const char *subject) {
  GHashTable *ht = purple_chat_get_components (chat);
  
  g_hash_table_replace (ht, g_strdup ("id"), g_strdup_printf ("%d", tgl_get_peer_id (id)));
  g_hash_table_replace (ht, g_strdup ("owner"), g_strdup_printf ("%d", admin_id));
  g_hash_table_replace (ht, g_strdup ("subject"), g_strdup (subject));
}

void tgp_chat_on_loaded_chat_full (struct tgl_state *TLS, struct tgl_chat *C) {
  connection_data *conn = TLS->ev_base;
  
  PurpleChat *PC = p2tgl_chat_find (TLS, C->id);
  if (!PC) {
    PC = p2tgl_chat_new (TLS, C);
    if (purple_account_get_bool (conn->pa, TGP_KEY_JOIN_GROUP_CHATS, TGP_DEFAULT_JOIN_GROUP_CHATS)) {
      purple_blist_add_chat (PC, NULL, NULL);
    }
  }
  p2tgl_chat_update (PC, C->id, C->admin_id, C->print_title);
}

static void tgp_chat_on_loaded_chat_full_joining (struct tgl_state *TLS, void *_,
                                                  int success, struct tgl_chat *C) {
  debug ("tgp_chat_on_loaded_chat_full_joining()");
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  
  tgp_chat_on_loaded_chat_full (TLS, C);
  tgp_chat_show (TLS, C);
  
  // Check if the users attempts to join an empty chat
  if (! C->user_list_size) {
    p2tgl_got_chat_in (TLS, C->id, C->id, "You have already left this chat.",
                       PURPLE_MESSAGE_SYSTEM, time (NULL));
  }
}

static void tgp_chat_add_all_users (struct tgl_state *TLS, PurpleConversation *conv, struct tgl_chat *C) {
  GList *users = NULL,
        *flags = NULL;
  
  int i = 0;
  for (; i < C->user_list_size; i++) {
    struct tgl_chat_user *uid = (C->user_list + i);
    users = g_list_append (users, g_strdup_printf ("%d", uid->user_id));
    flags = g_list_append (flags, GINT_TO_POINTER(C->admin_id == uid->user_id ? PURPLE_CBFLAGS_FOUNDER : PURPLE_CBFLAGS_NONE));
  }
  
  purple_conv_chat_add_users (PURPLE_CONV_CHAT(conv), users, NULL, flags, FALSE);
  while (users) {
    p2tgl_conv_add_user_rename (tgl_peer_get (TLS, TGL_MK_USER(atoi (users->data))), conv);
    users = g_list_next (users);
  }
  
  g_list_free_full (users, g_free);
  g_list_free (flags);
}

void tgp_chat_users_update (struct tgl_state *TLS, struct tgl_chat *C) {
  PurpleConversation *pc = purple_find_chat (tg_get_conn (TLS), tgl_get_peer_id (C->id));
  if (pc) {
    purple_conv_chat_clear_users (purple_conversation_get_chat_data (pc));
    tgp_chat_add_all_users (TLS, pc, C);
  }
}

PurpleConversation *tgp_chat_show (struct tgl_state *TLS, struct tgl_chat *C) {
  connection_data *conn = TLS->ev_base;
  
  PurpleConversation *convo = purple_find_chat (conn->gc, tgl_get_peer_id (C->id));
  PurpleConvChat *chat = purple_conversation_get_chat_data (convo);
  
  if (! convo || (chat && purple_conv_chat_has_left (chat))) {
    convo = p2tgl_got_joined_chat (conn->TLS, C);
    tgp_chat_users_update (conn->TLS, C);
  }
  return convo;
}

GList *tgprpl_chat_join_info (PurpleConnection * gc) {
  struct proto_chat_entry *pce;
  pce = g_new0 (struct proto_chat_entry, 1);
  pce->label = "_Subject:";
  pce->identifier = "subject";
  pce->required = FALSE;
  GList *info = g_list_append (NULL, pce);
  
  pce = g_new0 (struct proto_chat_entry, 1);
  pce->label = "_Join by Link:";
  pce->identifier = "link";
  pce->required = FALSE;
  return g_list_append (info, pce);
}

GHashTable *tgprpl_chat_info_defaults (PurpleConnection *gc, const char *chat_name) {
  debug ("tgprpl_chat_info_defaults(%s)", chat_name);
  
  connection_data *conn = purple_connection_get_protocol_data (gc);
  
  tgl_peer_t *P = tgl_peer_get_by_name (conn->TLS, chat_name);
  if (P) {
    return tgp_chat_info_new (conn->TLS, &P->chat);
  }
  warning ("Chat not found, returning empty defaults...");
  return g_hash_table_new_full(g_str_hash, *g_str_equal, NULL, g_free);
}

void tgprpl_chat_join (PurpleConnection * gc, GHashTable *data) {
  debug ("tgprpl_chat_join()");
  connection_data *conn = purple_connection_get_protocol_data (gc);
  
  // join existing chat by id when the user clicks on a chat in the buddy list
  void *value = g_hash_table_lookup (data, "id");
  if (value && atoi (value)) {
    tgl_peer_id_t cid = TGL_MK_CHAT(atoi (value));
    tgl_peer_t *P = tgl_peer_get (conn->TLS, cid);
    if (P) {
      debug ("joining chat by id %d ...", tgl_get_peer_id (cid));
      tgl_do_get_chat_info (conn->TLS, cid, FALSE, tgp_chat_on_loaded_chat_full_joining, NULL);
    } else {
      warning ("Cannot join chat %d, peer not found...", tgl_get_peer_id (cid));
      purple_serv_got_join_chat_failed (gc, data);
    }
    return;
  }
  
  // join chat by invite link provided in the chat join window
  const char *link = g_hash_table_lookup (data, "link");
  if (str_not_empty (link)) {
    tgl_do_import_chat_link (conn->TLS, link, (int)strlen (link), tgp_notify_on_error_gw, NULL);
    return;
  }
  
  // if a chat with this name doesn't exist yet, prompt to create one
  const char *subject = g_hash_table_lookup (data, "subject");
  if (str_not_empty (subject)) {
    tgl_peer_t *P = tgl_peer_get_by_name (conn->TLS, subject);
    
    // handle joining chats by print_names as used by the Adium plugin
    if (P && tgl_get_peer_type (P->id) == TGL_PEER_CHAT) {
      debug ("joining chat by subject %s ...", subject);
      tgl_do_get_chat_info (conn->TLS, P->id, FALSE, tgp_chat_on_loaded_chat_full_joining, NULL);
      return;
    }
    
    // user creates a new chat by providing its subject the chat join window
    request_create_chat (conn->TLS, subject);
  }
}

char *tgprpl_get_chat_name (GHashTable * data) {
  return g_strdup (g_hash_table_lookup (data, "subject"));
}

static void tgp_chat_roomlist_it (tgl_peer_t *P, void *extra) {
  connection_data *conn = extra;
  PurpleRoomlistRoom *room;
  
  if (tgl_get_peer_type (P->id) == TGL_PEER_CHAT && P->chat.users_num) {
    room = purple_roomlist_room_new (PURPLE_ROOMLIST_ROOMTYPE_ROOM, P->chat.print_title, NULL);
    purple_roomlist_room_add_field (conn->roomlist, room, GINT_TO_POINTER(P->chat.users_num));
    purple_roomlist_room_add (conn->roomlist, room);
  }
}

PurpleRoomlist *tgprpl_roomlist_get_list (PurpleConnection *gc) {
  debug ("tgprpl_roomlist_get_list()");
  connection_data *conn = purple_connection_get_protocol_data (gc);
  GList *fields = NULL;
  
  if (conn->roomlist) {
    purple_roomlist_unref (conn->roomlist);
  }
  
  conn->roomlist = purple_roomlist_new (purple_connection_get_account (gc));
  
  PurpleRoomlistField *f = purple_roomlist_field_new (PURPLE_ROOMLIST_FIELD_INT, "Users", "users", FALSE);
  
  fields = g_list_append (fields, f);
  purple_roomlist_set_fields (conn->roomlist, fields);
  
  tgl_peer_iterator_ex (conn->TLS, tgp_chat_roomlist_it, conn);
  
  return conn->roomlist;
}

void tgprpl_roomlist_cancel (PurpleRoomlist *list) {
  PurpleConnection *gc = purple_account_get_connection (list->account);
  if (! gc) {
    return;
  }
  
  connection_data *conn = purple_connection_get_protocol_data (gc);
  purple_roomlist_set_in_progress (list, FALSE);
  if (conn->roomlist == list) {
    conn->roomlist = NULL;
    purple_roomlist_unref (list);
  }
}
