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
 
 Copyright Matthias Jentsch 2015
 */

#ifndef tgp_blist_h
#define tgp_blist_h

#include <tgl.h>
#include <purple.h>

#define TGP_BUDDY_KEY_PEER_ID "user_id"
#define TGP_BUDDY_KEY_PEER_TYPE "peer_type"

const char *tgp_blist_peer_get_name (struct tgl_state *TLS, tgl_peer_id_t id);
void tgp_blist_peer_add_name (struct tgl_state *TLS, tgl_peer_id_t id, const char *name);
void tgp_blist_buddy_set_id (PurpleBuddy *buddy, tgl_peer_id_t id);
tgl_peer_id_t tgp_blist_peer_find_id (struct tgl_state *TLS, const char *who);
tgl_peer_t *tgp_blist_peer_find (struct tgl_state *TLS, const char *who);

tgl_peer_id_t tgp_blist_buddy_get_id (PurpleBuddy *buddy);
tgl_peer_t *tgp_blist_buddy_get_peer (PurpleBuddy *peer);
PurpleBuddy *tgp_blist_buddy_new (struct tgl_state *TLS, tgl_peer_t *user);
PurpleBuddy *tgp_blist_buddy_update_name (struct tgl_state *TLS, PurpleBuddy *buddy, struct tgl_user *user);
PurpleBuddy *tgp_blist_buddy_find (struct tgl_state *TLS, tgl_peer_id_t user);
PurpleChat *tgp_blist_chat_find (struct tgl_state *TLS, tgl_peer_id_t user);
PurpleGroup *tgp_blist_group_init (const char *name);

#endif
