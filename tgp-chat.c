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

void chat_add_all_users (PurpleConversation *pc, struct tgl_chat *chat) {
  int i;
  for (i = 0; i < chat->user_list_size; i++) {
    struct tgl_chat_user *uid = (chat->user_list + i);
    int flags = (chat->admin_id == uid->user_id ? PURPLE_CBFLAGS_FOUNDER : PURPLE_CBFLAGS_NONE);
    p2tgl_conv_add_user (pc, *uid, NULL, flags, 0);
  }
}

void chat_users_update (struct tgl_state *TLS, struct tgl_chat *chat) {
  PurpleConversation *pc = purple_find_chat(tg_get_conn(TLS), tgl_get_peer_id(chat->id));
  if (pc) {
    purple_conv_chat_clear_users (purple_conversation_get_chat_data(pc));
    chat_add_all_users (pc, chat);
  }
}

PurpleConversation *chat_show (PurpleConnection *gc, int id) {
  connection_data *conn = purple_connection_get_protocol_data(gc);
  PurpleConversation *convo = purple_find_chat (gc, id);
  tgl_peer_t *P = tgl_peer_get (conn->TLS, TGL_MK_CHAT(id));
  
  if (! convo) {
    if (! P) {
      warning ("Chat %d not existing, not showing...", id);
      return NULL;
    }
    convo = p2tgl_got_joined_chat (conn->TLS, &P->chat);
    chat_users_update (conn->TLS, &P->chat);
    if (PURPLE_CONNECTION_IS_CONNECTED(gc) && ! chat_is_member(conn->TLS->our_id, &P->chat)) {
      p2tgl_conversation_write (convo, TGL_MK_CHAT(id), "You are no longer a member of this chat.",
                                PURPLE_MESSAGE_SYSTEM | PURPLE_MESSAGE_NO_LOG, (int)time (NULL));
    }
  }

  return convo;
}

int chat_is_member (int who, struct tgl_chat *chat) {
  int i;
  for (i = 0; i < chat->user_list_size; i++) if ((chat->user_list + i)->user_id) {
    return TRUE;
  }
  return FALSE;
}
