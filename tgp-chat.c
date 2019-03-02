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
 
 Copyright Matthias Jentsch, Ben Wiederhake 2014-2016
 */

#include "tgp-chat.h"

GHashTable *tgp_chat_info_new (struct tgl_state *TLS, tgl_peer_t *P) {
  // libpurple chat components own the keys and the values when created from blist be consistent
  GHashTable *ht = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
  g_hash_table_insert (ht, g_strdup ("subject"), g_strdup (P->print_name));
  g_hash_table_insert (ht, g_strdup ("id"), g_strdup_printf ("%d", tgl_get_peer_id (P->id)));
  g_hash_table_insert (ht, g_strdup ("type"), g_strdup_printf ("%d", tgl_get_peer_type (P->id)));
  if (tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL) {
    g_hash_table_insert (ht, g_strdup ("last_server_id"), g_strdup_printf ("%d", 0));
  }
  return ht;
}

int tgp_chat_has_id (PurpleChat *C) {
  const char *id = g_hash_table_lookup (purple_chat_get_components (C), "id");
  return id && *id;
}

tgl_peer_id_t tgp_chat_get_id (PurpleChat *C) {
  const char *I = g_hash_table_lookup (purple_chat_get_components (C), "id");
  const char *T = g_hash_table_lookup (purple_chat_get_components (C), "type");
  int type = TGL_PEER_CHAT;
  if (T && *T) {
    type = atoi (T);
  }
  return tgl_set_peer_id (type, (I && *I) ? atoi (I) : 0);
}

void tgp_chat_set_last_server_id (struct tgl_state *TLS, tgl_peer_id_t chat, int id) {
  info ("setting channel message server_id=%d", id);

  char *key = g_strdup_printf ("last-server-id/%d", tgl_get_peer_id (chat));
  purple_account_set_int (tls_get_pa (TLS), key, id);
  g_free (key);
}

int tgp_chat_get_last_server_id (struct tgl_state *TLS, tgl_peer_id_t chat) {
  char *key = g_strdup_printf ("last-server-id/%d", tgl_get_peer_id (chat));
  int last = purple_account_get_int (tls_get_pa (TLS), key, 0);
  g_free (key);

  return (unsigned int) last;
}

PurpleChat *tgp_chat_blist_store (struct tgl_state *TLS, tgl_peer_t *P, const char *group) {
  g_return_val_if_fail(tgl_get_peer_type (P->id) == TGL_PEER_CHAT || tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL, NULL);

  PurpleChat *PC = tgp_blist_chat_find (TLS, P->id);
  if (! (P->flags & TGLCF_LEFT)) {
    if (! PC) {
      PC = purple_chat_new (tls_get_pa (TLS), P->chat.print_title, tgp_chat_info_new (TLS, P));
      // adding chats to the blist in Adium would cause the bookmarks and auto-joins to fail,
      // as Adium assumes that a chat existing in blist means the user already joined
#ifndef __ADIUM_
      if (purple_account_get_bool (tls_get_pa (TLS), TGP_KEY_JOIN_GROUP_CHATS, TGP_DEFAULT_JOIN_GROUP_CHATS)) {
        purple_blist_add_chat (PC, tgp_blist_group_init (group), NULL);
      }
#endif
    }
    tgp_info_update_photo (&PC->node, tgl_peer_get (TLS, P->id));
  } else {
    if (PC) {
      purple_blist_remove_chat (PC);
      PC = NULL;
    }
  }
  
  if (PC) {
    g_hash_table_replace (purple_chat_get_components (PC), g_strdup ("id"),
        g_strdup_printf ("%d", tgl_get_peer_id (P->id)));
    g_hash_table_replace (purple_chat_get_components (PC), g_strdup ("type"),
        g_strdup_printf ("%d", tgl_get_peer_type (P->id)));
    g_hash_table_replace (purple_chat_get_components (PC), g_strdup ("subject"),
        g_strdup (tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL ? P->channel.title : P->chat.title));
  }

  return PC;
}

static void tgp_chat_on_loaded_chat_full_joining (struct tgl_state *TLS, void *_, int success, struct tgl_chat *C) {
  debug ("tgp_chat_on_loaded_chat_full_joining()");
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  
  tgp_chat_blist_store (TLS, tgl_peer_get (TLS, C->id), _("Telegram Chats"));
  tgp_chat_show (TLS, tgl_peer_get (TLS, C->id));

  // Check if the users attempts to join an empty chat
  if (C->flags & TGLCF_LEFT) {
    tgp_chat_got_in (TLS, tgl_peer_get (TLS, C->id), C->id, _("You have already left this chat."), PURPLE_MESSAGE_SYSTEM,
        time (NULL));
  }
}

static void tgp_chat_on_loaded_channel_full_joining (struct tgl_state *TLS, void *extra, int success, tgl_peer_t *P) {
  debug ("tgp_chat_on_loaded_channel_full_joining()");
  if (! success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  tgp_chat_blist_store (TLS, P, _("Telegram Channels"));
  tgp_chat_show (TLS, P);
}

static void tgp_chat_add_all_users (struct tgl_state *TLS, PurpleConversation *conv, tgl_peer_t *P) {
  debug ("tgp_chat_add_all_users()");

  GList *users = NULL,
        *flags = NULL;

  switch (tgl_get_peer_type (P->id)) {
    case TGL_PEER_CHAT: {
      struct tgl_chat *C = &P->chat;

      int i;
      for (i = 0; i < C->user_list_size; i ++) {
        struct tgl_chat_user *uid = (C->user_list + i);
        const char *name = tgp_blist_lookup_purple_name (TLS, TGL_MK_USER(uid->user_id));
        if (name) {
          users = g_list_append (users, g_strdup (name));
          flags = g_list_append (flags, GINT_TO_POINTER(C->admin_id == uid->user_id ? PURPLE_CBFLAGS_FOUNDER : PURPLE_CBFLAGS_NONE));
        }
      }
      break;
    }

    case TGL_PEER_CHANNEL: {
      // fetch users
      GList *MS = g_hash_table_lookup (tls_get_data (TLS)->channel_members, GINT_TO_POINTER(tgl_get_peer_id (P->id)));
      
      while (MS) {
        struct tgp_channel_member *M = MS->data;
        const char *name = tgp_blist_lookup_purple_name (TLS, M->id);
        if (name) {
          users = g_list_append (users, g_strdup (name));
          flags = g_list_append (flags, GINT_TO_POINTER(M->flags));
        }
        MS = g_list_next (MS);
      };
      break;
    }

    default:
      g_return_if_reached();
      break;
  }

  purple_conv_chat_add_users (PURPLE_CONV_CHAT(conv), users, NULL, flags, FALSE);
  tgp_g_list_free_full (users, g_free);
  g_list_free (flags);
}

PurpleConversation *tgp_chat_show (struct tgl_state *TLS, tgl_peer_t *P) {
  PurpleConvChat *chat = NULL;
  
  // check if chat is already shown
  PurpleConversation *conv = purple_find_chat (tls_get_conn (TLS), tgl_get_peer_id (P->id));
  if (conv) {
    chat = purple_conversation_get_chat_data (conv);
    if (chat && ! purple_conv_chat_has_left (chat)) {
      return conv;
    }
  }

  // join the chat now
  const char *name = NULL;
  if (tgl_get_peer_type (P->id) == TGL_PEER_CHAT) {
    name = P->chat.print_title;
  } else if (tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL) {
    name = P->channel.print_title;
  }
  g_return_val_if_fail(name, NULL);
  
  conv = serv_got_joined_chat (tls_get_conn (TLS), tgl_get_peer_id (P->id), name);
  g_return_val_if_fail(conv, NULL);

  purple_conv_chat_clear_users (purple_conversation_get_chat_data (conv));
  tgp_chat_add_all_users (TLS, conv, P);

  return conv;
}

int tgprpl_send_chat (PurpleConnection *gc, int id, const char *message, PurpleMessageFlags flags) {
  debug ("tgprpl_send_chat()");
  
  tgl_peer_t *P = tgl_peer_get (gc_get_tls (gc), TGL_MK_CHAT(id));
  if (! P) {
    P = tgl_peer_get (gc_get_tls (gc), TGL_MK_CHANNEL(id));
  }
  g_return_val_if_fail(P != NULL, -1);
  
  // when the group receives a message it is obvious that the previous messages were read
  pending_reads_send_user (gc_get_tls (gc), P->id);
  
  return tgp_msg_send (gc_get_tls (gc), message, P->id);
}

unsigned int tgprpl_send_chat_typing (PurpleConversation *conv, PurpleTypingState typing, gpointer ignored)
{
  PurpleConnection *gc = purple_conversation_get_gc (conv);
  tgl_peer_t *P;
  PurpleConvChat *chat;
  int id;

  if (!PURPLE_CONNECTION_IS_CONNECTED (gc))
    return 0;

  if (g_strcmp0(purple_plugin_get_id (purple_connection_get_prpl (gc)), PLUGIN_ID))
    return 0;
  
  debug ("tgprpl_send_chat_typing()");
  
  chat = purple_conversation_get_chat_data (conv);
  id = purple_conv_chat_get_id (chat);
  
  P = tgl_peer_get (gc_get_tls (gc), TGL_MK_CHAT(id));
  if (! P) {
    P = tgl_peer_get (gc_get_tls (gc), TGL_MK_CHANNEL(id));
  }
  g_return_val_if_fail(P != NULL, -1);
  
  tgl_do_send_typing (gc_get_tls (gc), P->id, typing == PURPLE_TYPING ? tgl_typing_typing : tgl_typing_cancel,
        0, 0);

  // when the group receives a typing notification it is obvious that the previous messages were read
  pending_reads_send_user (gc_get_tls (gc), P->id);
  
  return 2;
}

void update_chat_typing (struct tgl_state *TLS, struct tgl_user *U, struct tgl_chat *C, enum tgl_typing_status status) {
  debug ("update_chat_typing()");
  
  PurpleConvChat *chat = NULL;
  PurpleConversation *conv = purple_find_chat (tls_get_conn (TLS), tgl_get_peer_id (C->id));
  if (conv) {
    chat = purple_conversation_get_chat_data (conv);
  }
  g_return_if_fail(chat != NULL);
  
  const char *name = tgp_blist_lookup_purple_name (TLS, U->id);
  g_return_if_fail(name != NULL);
  
  PurpleConvChatBuddyFlags flags = purple_conv_chat_user_get_flags (chat, name);
  
  if (status == tgl_typing_typing) {
    flags |= PURPLE_CBFLAGS_TYPING;
  } else {
    flags &= ~PURPLE_CBFLAGS_TYPING;
  }
  
  purple_conv_chat_user_set_flags(chat, name, flags);
}

void tgprpl_kick_from_chat (PurpleConnection *gc, int id, const char *who) {
  debug ("tgprpl_kick_from_chat()");
  
  tgl_peer_t *P = tgl_peer_get (gc_get_tls (gc), TGL_MK_CHAT(id));
  if (! P) {
    P = tgl_peer_get (gc_get_tls (gc), TGL_MK_CHANNEL(id));
  }
  g_return_if_fail(P != NULL);
  
  tgl_peer_t *other_id = tgp_blist_lookup_peer_get (gc_get_tls (gc), who);
  if (other_id == NULL) {
    return;
  }
  
  tgl_do_del_user_from_chat (gc_get_tls (gc), P->id, other_id->id, tgp_notify_on_error_gw, NULL);
}

GList *tgprpl_chat_join_info (PurpleConnection *gc) {
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
      return tgp_chat_info_new (gc_get_tls (gc), P);
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
    tgl_peer_t *P = tgl_peer_get (gc_get_tls (gc), TGL_MK_CHAT(atoi (value)));
    if (! P) {
      P = tgl_peer_get (gc_get_tls (gc), TGL_MK_CHANNEL(atoi (value)));
    }
    if (P) {
      debug ("type=%d", tgl_get_peer_type (P->id));
      if (tgl_get_peer_type (P->id) == TGL_PEER_CHAT) {
        debug ("joining chat by id %d ...", tgl_get_peer_id (P->id));
        tgl_do_get_chat_info (gc_get_tls (gc), P->id, FALSE, tgp_chat_on_loaded_chat_full_joining, NULL);
      } else {
        g_return_if_fail(tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL);
        debug ("joining channel by id %d ...", tgl_get_peer_id (P->id));
        tgp_channel_load (gc_get_tls (gc), P, tgp_chat_on_loaded_channel_full_joining, NULL);
      }
    } else {
      warning ("Cannot join chat %d, peer not found...", atoi (value));
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
    if (! P) {
      // user creates a new chat by providing its subject the chat join window
      request_create_chat (gc_get_tls (gc), subject);
      return;
    }
    
    // handle joining chats by print_names as used by the Adium plugin
    if (tgl_get_peer_type (P->id) == TGL_PEER_CHAT) {
      debug ("joining chat by subject %s ...", subject);
      tgl_do_get_chat_info (gc_get_tls (gc), P->id, FALSE, tgp_chat_on_loaded_chat_full_joining, NULL);
      return;
    } else if (tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL) {
      debug ("joining channel by subject %s ...", subject);
      tgp_channel_load (gc_get_tls (gc), P, tgp_chat_on_loaded_channel_full_joining, NULL);
      return;
    } else {
      warning ("Cannot join chat %s, wrong peer type", subject);
      purple_serv_got_join_chat_failed (gc, data);
    }
  }
}

char *tgprpl_get_chat_name (GHashTable * data) {
  return g_strdup (g_hash_table_lookup (data, "subject"));
}

static void tgp_chat_roomlist_add (tgl_peer_t *P, void *extra) {
  connection_data *conn = extra;
  
  if ((tgl_get_peer_type (P->id) == TGL_PEER_CHAT || tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL)
      && !(P->flags & TGLPF_LEFT)) {
    char *id = g_strdup_printf ("%d", tgl_get_peer_id (P->id));
    
    PurpleRoomlistRoom *room = purple_roomlist_room_new (PURPLE_ROOMLIST_ROOMTYPE_ROOM, P->chat.print_title, NULL);
    purple_roomlist_room_add_field (conn->roomlist, room, id);
    if (tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL) {
      purple_roomlist_room_add_field (conn->roomlist, room, GINT_TO_POINTER(0));
      purple_roomlist_room_add_field (conn->roomlist, room, (P->channel.flags & TGLCHF_MEGAGROUP) ? _("Supergroup") : _("Channel"));
    } else {
      purple_roomlist_room_add_field (conn->roomlist, room, GINT_TO_POINTER(P->chat.users_num));
      purple_roomlist_room_add_field (conn->roomlist, room, _("Group"));
    }
    purple_roomlist_room_add (conn->roomlist, room);
    
    g_free (id);
  }
}

void tgp_chat_roomlist_populate (struct tgl_state *TLS) {
  connection_data *conn = tls_get_data (TLS);
  g_return_if_fail(purple_roomlist_get_in_progress (conn->roomlist));
  
  GList *fields = NULL;
  PurpleRoomlistField *f = purple_roomlist_field_new (PURPLE_ROOMLIST_FIELD_STRING, "", "id", TRUE);
  fields = g_list_append (fields, f);
  
  f = purple_roomlist_field_new (PURPLE_ROOMLIST_FIELD_INT, _("Users in chat"), "users", FALSE);
  fields = g_list_append (fields, f);
  
  f = purple_roomlist_field_new (PURPLE_ROOMLIST_FIELD_STRING, _("Type"), "type", FALSE);
  fields = g_list_append (fields, f);
  
  purple_roomlist_set_fields (conn->roomlist, fields);
  tgl_peer_iterator_ex (conn->TLS, tgp_chat_roomlist_add, conn);
  
  purple_roomlist_set_in_progress (conn->roomlist, FALSE);
}

PurpleRoomlist *tgprpl_roomlist_get_list (PurpleConnection *gc) {
  debug ("tgprpl_roomlist_get_list()");
  connection_data *conn = gc_get_data (gc);

  if (conn->roomlist) {
    purple_roomlist_unref (conn->roomlist);
  }

  conn->roomlist = purple_roomlist_new (purple_connection_get_account (gc));
  
  purple_roomlist_set_in_progress (conn->roomlist, TRUE);
  
  // spectrum2 calls this function when the account is still connection
  // all roomlists in progress are being initialised by tgp_on_ready
  if (conn->dialogues_ready) {
    tgp_chat_roomlist_populate (conn->TLS);
  }
  
  return conn->roomlist;
}

void tgprpl_roomlist_cancel (PurpleRoomlist *list) {
  g_return_if_fail(list->account);

  purple_roomlist_set_in_progress (list, FALSE);
  if (pa_get_data (list->account)->roomlist == list) {
    pa_get_data (list->account)->roomlist = NULL;
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

static void tgp_channel_loading_free (struct tgp_channel_loading *D) {
  if (D->callbacks) {
    g_list_free (D->callbacks);
  }
  if (D->extras) {
    g_list_free (D->extras);
  }
  free (D);
}

static void tgp_channel_load_finish (struct tgl_state *TLS, struct tgp_channel_loading *D, int success) {
  GList *cb = D->callbacks;
  GList *extra = D->extras;

  if (! g_list_length (D->members)) {
    struct tgp_channel_member *M = talloc0 (sizeof(struct tgp_channel_member));
    M->id = TLS->our_id;
    D->members = g_list_append (D->members, M);
  }

  g_hash_table_replace (tls_get_data (TLS)->channel_members,
      GINT_TO_POINTER(tgl_get_peer_id (D->P->id)), D->members);

  while (cb) {
    if (cb->data) {
      ((void (*) (struct tgl_state *, void *, int, tgl_peer_t *)) cb->data) (TLS, extra->data, success, D->P);
    }
    cb = g_list_next(cb);
    extra = g_list_next(extra);
  }

  tgp_channel_loading_free (D);
}

static void tgp_channel_load_admins_done (struct tgl_state *TLS, void *extra, int success, int users_num,
      struct tgl_user **users) {
  debug ("tgp_channel_load_admins_done()");
  
  struct tgp_channel_loading *D = extra;
  
  if (success) {
    GHashTable *HT = g_hash_table_new (g_direct_hash, g_direct_equal);
    int i;
    for (i = 0; i < users_num; i ++) {
      g_hash_table_insert (HT, GINT_TO_POINTER(tgl_get_peer_id (users[i]->id)), GINT_TO_POINTER(1));
    }
    
    GList *MS = D->members;
    do {
      struct tgp_channel_member *M = MS->data;
      if (g_hash_table_lookup (HT, GINT_TO_POINTER(tgl_get_peer_id (M->id)))) {
        M->flags |= PURPLE_CBFLAGS_OP;
      }
    } while ((MS = g_list_next (MS)));
    g_hash_table_destroy (HT);
  }
  
  tgp_channel_load_finish (TLS, D, success);
}

static void tgp_channel_get_members_done (struct tgl_state *TLS, void *extra, int success, int users_num,
      struct tgl_user **users) {
  debug ("tgp_channel_load_members_done()");
  struct tgp_channel_loading *D = extra;
  
  if (! success) {
    tgp_channel_load_finish (TLS, D, FALSE);
    return;
  }
  
  int i;
  for (i = 0; i < users_num; i ++) {
    struct tgp_channel_member *M = talloc0 (sizeof(struct tgp_channel_member));
    M->id = users[i]->id;
    D->members = g_list_append (D->members, M);
  }
  
  tgl_do_channel_get_members (TLS, D->P->id,
      purple_account_get_int (tls_get_pa (TLS), TGP_KEY_CHANNEL_MEMBERS, TGP_DEFAULT_CHANNEL_MEMBERS),
      0, 1, tgp_channel_load_admins_done, D);
}

static gint tgp_channel_find_higher_id (gconstpointer a, gconstpointer b) {
  return ((struct tgp_msg_loading *)a)->msg->server_id < GPOINTER_TO_INT(b);
}

static void tgp_channel_get_history_done (struct tgl_state *TLS, void *extra, int success, int size,
                struct tgl_message **list) {
  struct tgp_channel_loading *D = extra;

  if (success) {
    if (size > 0 && tgp_chat_get_last_server_id (TLS, D->P->id) < list[size - 1]->server_id) {
      tgp_chat_set_last_server_id (TLS, D->P->id, (int) list[size - 1]->server_id);
    }
    
    GList *where = g_queue_find_custom (tls_get_data (TLS)->new_messages,
                       GINT_TO_POINTER(tgp_chat_get_last_server_id (TLS, D->P->id)), tgp_channel_find_higher_id);
    int i;
    for (i = size - 1; i >= 0; -- i) {
      if (list[i]->server_id > tgp_chat_get_last_server_id (TLS, D->P->id)) {
        tgp_msg_recv (TLS, list[i], where);
      }
    }
  } else {
    g_warn_if_reached(); // gap in history
  }

  if (D->P->flags & (TGLCHF_ADMIN | TGLCHF_MEGAGROUP)) {
    tgl_do_channel_get_members (TLS, D->P->id,
        purple_account_get_int (tls_get_pa (TLS), TGP_KEY_CHANNEL_MEMBERS, TGP_DEFAULT_CHANNEL_MEMBERS),
        0, 0, tgp_channel_get_members_done, extra);
  } else {
    tgp_channel_load_finish (TLS, D, success);
  }
}

void tgp_channel_load (struct tgl_state *TLS, tgl_peer_t *P,
        void (*callback) (struct tgl_state *, void *, int, tgl_peer_t *),
        void *extra) {
  g_return_if_fail(tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL);
  
  gpointer ID = GINT_TO_POINTER(tgl_get_peer_id (P->id));
  if (! g_hash_table_lookup (tls_get_data (TLS)->pending_channels, ID)) {
    
    struct tgp_channel_loading *D = talloc0 (sizeof(struct tgp_channel_loading));
    D->P = P;
    D->callbacks = g_list_append (NULL, callback);
    D->extras = g_list_append (NULL, extra);
    D->remaining = 2;

    tgl_do_get_history_range (TLS, P->id, (int) tgp_chat_get_last_server_id (TLS, P->id), 0,
        TGP_CHANNEL_HISTORY_LIMIT, tgp_channel_get_history_done, D);
    g_hash_table_replace (tls_get_data (TLS)->pending_channels, ID, D);

  } else {
    if (! tgp_channel_loaded (TLS, P->id)) {
      struct tgp_channel_loading *D = g_hash_table_lookup (tls_get_data (TLS)->pending_channels, ID);
      D->callbacks = g_list_append (D->callbacks, callback);
      D->extras = g_list_append (D->extras, extra);
    } else {
      callback (TLS, extra, TRUE, P);
    }
  }
}

int tgp_channel_loaded (struct tgl_state *TLS, tgl_peer_id_t id) {
  return NULL != g_hash_table_lookup (tls_get_data (TLS)->channel_members,
             GINT_TO_POINTER(tgl_get_peer_id (id)));
}

static void update_chat (struct tgl_state *TLS, tgl_peer_t *C, unsigned flags, const char *group) {
  if (flags & TGL_UPDATE_CREATED) {
    tgp_blist_lookup_add (TLS, C->id, C->print_name);
    tgp_chat_blist_store (TLS, tgl_peer_get (TLS, C->id), group);
  } else {
    PurpleChat *PC = tgp_blist_chat_find (TLS, C->id);
    if (PC) {
      if (flags & TGL_UPDATE_TITLE) {
        purple_blist_alias_chat (PC, C->print_name);
      }
      if (flags & TGL_UPDATE_DELETED) { // TODO: test if this is actually executed on deletion
        purple_blist_remove_chat (PC);
      }
      if (flags & TGL_UPDATE_PHOTO) {
        tgp_info_update_photo (&PC->node, tgl_peer_get (TLS, C->id));
      }
    }
  }
}

void update_channel_handler (struct tgl_state *TLS, struct tgl_channel *C, unsigned flags) {
  debug ("update_channel_handler() (%s)", print_flags_update (flags));
  
  update_chat (TLS, tgl_peer_get (TLS, C->id), flags, _("Telegram Channels"));
}

void update_chat_handler (struct tgl_state *TLS, struct tgl_chat *C, unsigned flags) {
  debug ("update_chat_handler() (%s)", print_flags_update (flags));
  
  update_chat (TLS, tgl_peer_get (TLS, C->id), flags, _("Telegram Chats"));
}

