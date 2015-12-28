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

GHashTable *tgp_chat_info_new (struct tgl_state *TLS, struct tgl_chat *chat) {
  gchar *title = g_strdup (chat->print_title);
  
  GHashTable *ht = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_free);
  g_hash_table_insert (ht, "subject", title);
  g_hash_table_insert (ht, "id", g_strdup_printf ("%d", tgl_get_peer_id (chat->id)));
  
  return ht;
}

PurpleChat *p2tgl_chat_new (struct tgl_state *TLS, struct tgl_chat *chat) {
  return purple_chat_new (tls_get_pa (TLS), chat->title, tgp_chat_info_new (TLS, chat));
}

void p2tgl_chat_update (struct tgl_state *TLS, PurpleChat *chat, tgl_peer_id_t id, int admin_id, const char *subject) {
  GHashTable *ht = purple_chat_get_components (chat);
  
  g_hash_table_replace (ht, g_strdup ("id"), g_strdup_printf ("%d", tgl_get_peer_id (id)));
  g_hash_table_replace (ht, g_strdup ("subject"), g_strdup (subject));
}

int tgp_chat_has_id (PurpleChat *C) {
  const char *id = g_hash_table_lookup (purple_chat_get_components (C), "id");
  return id && *id;
}

tgl_peer_id_t tgp_chat_get_id (PurpleChat *C) {
  const char *id = g_hash_table_lookup (purple_chat_get_components (C), "id");
  assert (id && *id);
  return TGL_MK_CHAT(atoi (id));
}

void tgp_chat_on_loaded_chat_full (struct tgl_state *TLS, struct tgl_chat *C) {
  PurpleChat *PC = tgp_blist_chat_find (TLS, C->id);
  if (!PC) {
    PC = p2tgl_chat_new (TLS, C);
    if (purple_account_get_bool (tls_get_pa (TLS), TGP_KEY_JOIN_GROUP_CHATS, TGP_DEFAULT_JOIN_GROUP_CHATS)) {
      purple_blist_add_chat (PC, tgp_blist_group_init ("Telegram Chats"), NULL);
    }
  }
  p2tgl_chat_update (TLS, PC, C->id, C->admin_id, C->print_title);
}

static void tgp_chat_on_loaded_chat_full_joining (struct tgl_state *TLS, void *_, int success, struct tgl_chat *C) {
  debug ("tgp_chat_on_loaded_chat_full_joining()");
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  
  tgp_chat_on_loaded_chat_full (TLS, C);
  tgp_chat_show (TLS, C);
  
  // Check if the users attempts to join an empty chat
  if (! C->user_list_size) {
    p2tgl_got_chat_in (TLS, C->id, C->id, _("You have already left this chat."), PURPLE_MESSAGE_SYSTEM, time (NULL));
  }
}

static void tgp_chat_add_all_users (struct tgl_state *TLS, PurpleConversation *conv, struct tgl_chat *C) {
  GList *users = NULL,
        *flags = NULL;
  debug ("tgp_chat_add_all_users()");
  
  int i = 0;
  for (; i < C->user_list_size; i++) {
    struct tgl_chat_user *uid = (C->user_list + i);
    const char *name = tgp_blist_lookup_purple_name (TLS, TGL_MK_USER(uid->user_id));
    if (! name) {
      g_warn_if_reached();
      continue;
    }
    users = g_list_append (users, g_strdup (name));
    flags = g_list_append (flags, GINT_TO_POINTER(C->admin_id == uid->user_id ? PURPLE_CBFLAGS_FOUNDER : PURPLE_CBFLAGS_NONE));
  }
  purple_conv_chat_add_users (PURPLE_CONV_CHAT(conv), users, NULL, flags, FALSE);
  g_list_free_full (users, g_free);
  g_list_free (flags);
}

void tgp_chat_users_update (struct tgl_state *TLS, struct tgl_chat *C) {
  PurpleConversation *pc = purple_find_chat (tls_get_conn (TLS), tgl_get_peer_id (C->id));
  if (pc) {
    purple_conv_chat_clear_users (purple_conversation_get_chat_data (pc));
    tgp_chat_add_all_users (TLS, pc, C);
  }
}

PurpleConversation *tgp_chat_show (struct tgl_state *TLS, struct tgl_chat *C) {
  PurpleConversation *convo = purple_find_chat (tls_get_conn (TLS), tgl_get_peer_id (C->id));
  PurpleConvChat *chat = purple_conversation_get_chat_data (convo);
  
  if (! convo || (chat && purple_conv_chat_has_left (chat))) {
    convo = serv_got_joined_chat (tls_get_conn (TLS), tgl_get_peer_id (C->id), C->print_title);
    tgp_chat_users_update (TLS, C);
  }
  return convo;
}

GList *tgprpl_chat_join_info (PurpleConnection * gc) {
  struct proto_chat_entry *pce;
  pce = g_new0 (struct proto_chat_entry, 1);
  pce->label = _("Subject:");
  pce->identifier = "subject";
  pce->required = FALSE;
  GList *info = g_list_append (NULL, pce);

  pce = g_new0 (struct proto_chat_entry, 1);
  pce->label = _("Invite link:");
  pce->identifier = "link";
  pce->required = FALSE;
  info = g_list_append (info, pce);

  pce = g_new0 (struct proto_chat_entry, 1);
  pce->label = _("Chat ID:");
  pce->identifier = "id";
  pce->required = FALSE;

  return g_list_append (info, pce);
}

GHashTable *tgprpl_chat_info_defaults (PurpleConnection *gc, const char *chat_name) {
  debug ("tgprpl_chat_info_defaults()");
  if (chat_name) {
    tgl_peer_t *P = tgl_peer_get_by_name (gc_get_tls (gc), chat_name);
    if (P) {
      debug ("found chat...");
      return tgp_chat_info_new (gc_get_tls (gc), &P->chat);
    }
    warning ("Chat not found, returning empty defaults...");
  }
  return g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_free);
}

void tgprpl_chat_join (PurpleConnection *gc, GHashTable *data) {
  debug ("tgprpl_chat_join()");
  g_return_if_fail(data);
  
  // auto joins will cause chat joins at a time when the dialogue list is not ready, remember
  // those chats and join them once the dialogue list is fetched
  if (! gc_get_data (gc)->dialogues_ready) {
    g_return_if_fail (data);
    const char *id = g_hash_table_lookup (data, "id");
    if (id) {
      debug ("attempting to join chat %s while not ready, queue up for later", id);
      gc_get_data (gc)->pending_joins = g_list_append (gc_get_data (gc)->pending_joins, g_strdup (id));
    }
    return;
  }

  // join existing chat by id when the user clicks on a chat in the buddy list
  void *value = g_hash_table_lookup (data, "id");
  if (value && atoi (value)) {
    tgl_peer_id_t cid = TGL_MK_CHAT(atoi (value));
    tgl_peer_t *P = tgl_peer_get (gc_get_tls (gc), cid);
    if (P) {
      debug ("joining chat by id %d ...", tgl_get_peer_id (cid));
      tgl_do_get_chat_info (gc_get_tls (gc), cid, FALSE, tgp_chat_on_loaded_chat_full_joining, NULL);
    } else {
      warning ("Cannot join chat %d, peer not found...", tgl_get_peer_id (cid));
      purple_serv_got_join_chat_failed (gc, data);
    }
    return;
  }
  
  // join chat by invite link provided in the chat join window
  const char *link = g_hash_table_lookup (data, "link");
  if (str_not_empty (link)) {
    tgl_do_import_chat_link (gc_get_tls (gc), link, (int)strlen (link), tgp_notify_on_error_gw, NULL);
    return;
  }
  
  // if a chat with this name doesn't exist yet, prompt to create one
  const char *subject = g_hash_table_lookup (data, "subject");
  if (str_not_empty (subject)) {
    tgl_peer_t *P = tgl_peer_get_by_name (gc_get_tls (gc), subject);
    
    // handle joining chats by print_names as used by the Adium plugin
    if (P && tgl_get_peer_type (P->id) == TGL_PEER_CHAT) {
      debug ("joining chat by subject %s ...", subject);
      
      tgl_do_get_chat_info (gc_get_tls (gc), P->id, FALSE, tgp_chat_on_loaded_chat_full_joining, NULL);
      return;
    }
    
    // user creates a new chat by providing its subject the chat join window
    request_create_chat (gc_get_tls (gc), subject);
  }
}

char *tgprpl_get_chat_name (GHashTable * data) {
  return g_strdup (g_hash_table_lookup (data, "subject"));
}

static void tgp_chat_roomlist_it (tgl_peer_t *P, void *extra) {
  connection_data *conn = extra;
  
  if (tgl_get_peer_type (P->id) == TGL_PEER_CHAT && P->chat.users_num) {
    char *id = g_strdup_printf ("%d", tgl_get_peer_id (P->id));
    
    PurpleRoomlistRoom *room = purple_roomlist_room_new (PURPLE_ROOMLIST_ROOMTYPE_ROOM, P->chat.print_title, NULL);
    purple_roomlist_room_add_field (conn->roomlist, room, id);
    purple_roomlist_room_add_field (conn->roomlist, room, GINT_TO_POINTER(P->chat.users_num));
    purple_roomlist_room_add (conn->roomlist, room);
    
    g_free (id);
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
  
  PurpleRoomlistField *f = purple_roomlist_field_new (PURPLE_ROOMLIST_FIELD_STRING, "", "id", TRUE);
  fields = g_list_append (fields, f);
  
  f = purple_roomlist_field_new (PURPLE_ROOMLIST_FIELD_INT, _("Users in chat"), "users", FALSE);
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

void tgp_chat_join_all_pending (struct tgl_state *TLS) {
  GList *pending;
  for (pending = tls_get_data (TLS)->pending_joins; pending != NULL; pending = g_list_next(pending)) {
    GHashTable *data = g_hash_table_new (g_str_hash, g_str_equal);
    g_hash_table_insert (data, "id", pending->data);
    tgprpl_chat_join (tls_get_conn (TLS), data);
    g_hash_table_destroy (data);
  }
  if (tls_get_data (TLS)->pending_joins) {
    g_list_free (tls_get_data (TLS)->pending_joins);
    tls_get_data (TLS)->pending_joins = NULL;
  }
}
