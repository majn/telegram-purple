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

#ifndef __telegram_adium__tgp_chat__
#define __telegram_adium__tgp_chat__

#include "telegram-purple.h"

struct tgp_channel_member {
  tgl_peer_id_t id;
  int flags;
};

struct tgp_channel_loading {
  tgl_peer_t *P;
  GList *members;
  GList *callbacks;
  GList *extras;
  int remaining;
  PurpleChat *CH;
};

tgl_peer_id_t tgp_chat_get_id (PurpleChat *C);
int tgp_chat_has_id (PurpleChat *C);

void tgp_chat_set_last_server_id (PurpleChat *C, long long id);
long long tgp_chat_get_last_server_id (PurpleChat *C);

PurpleChat *tgp_chat_blist_store (struct tgl_state *TLS, tgl_peer_t *P, const char *group);

PurpleConversation *tgp_chat_show (struct tgl_state *TLS, tgl_peer_t *P);
int tgprpl_send_chat (PurpleConnection *gc, int id, const char *message, PurpleMessageFlags flags);
char *tgprpl_get_chat_name (GHashTable *data);
void tgprpl_chat_join (PurpleConnection *gc, GHashTable *data);
GList *tgprpl_chat_join_info (PurpleConnection *gc);
PurpleRoomlist *tgprpl_roomlist_get_list (PurpleConnection *gc);
void tgprpl_roomlist_cancel (PurpleRoomlist *list);
GHashTable *tgprpl_chat_info_defaults (PurpleConnection *gc, const char *chat_name);
void tgp_chat_join_all_pending (struct tgl_state *TLS);

void tgp_channel_load (struct tgl_state *TLS, tgl_peer_t *P,
         void (*callback) (struct tgl_state *, void *, int, tgl_peer_t *),
         void *extra);
int tgp_channel_loaded (struct tgl_state *TLS, tgl_peer_id_t id);

void update_channel_handler (struct tgl_state *TLS, struct tgl_channel *C, unsigned flags);
void update_chat_handler (struct tgl_state *TLS, struct tgl_chat *C, unsigned flags);

#endif
