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

/*
 Functions for managing telegram contacts in the buddy list and performing id to purple-username and
 purple-username to id lookups.
 
 Purple prefers human-readable names for buddy usernames, while Telegram uses numerical user ids.
 In older versions of this plugin, the user id was used as username and the print name as the users
 alias. This means that getting a useful username in the interface relied on the alias resolution,
 which unfortunately doesn't work when a user isn't in the buddy list, or in Adium group chats.
 Because of that, this plugin now uses the unique print names provided by libtgl as username instead.
 */

const char *tgp_blist_peer_get_purple_name (struct tgl_state *TLS, tgl_peer_id_t id);
void tgp_blist_peer_add_purple_name (struct tgl_state *TLS, tgl_peer_id_t id, const char *purple_name);
tgl_peer_t *tgp_blist_peer_find (struct tgl_state *TLS, const char *purple_name);

/*
 To make this new approach robust to names changes, it is necessarry to store the user ID in each
 blist node to allow reliable buddy list lookups by user ids.
 */

void tgp_blist_buddy_set_id (PurpleBuddy *buddy, tgl_peer_id_t id);
int tgp_blist_buddy_has_id (PurpleBuddy *buddy);
tgl_peer_id_t tgp_blist_buddy_get_id (PurpleBuddy *buddy);
tgl_peer_t *tgp_blist_buddy_get_peer (PurpleBuddy *peer);
PurpleBuddy *tgp_blist_buddy_new (struct tgl_state *TLS, tgl_peer_t *user);
PurpleBuddy *tgp_blist_buddy_migrate (struct tgl_state *TLS, PurpleBuddy *buddy, struct tgl_user *user);
PurpleBuddy *tgp_blist_buddy_find (struct tgl_state *TLS, tgl_peer_id_t user);
PurpleChat *tgp_blist_chat_find (struct tgl_state *TLS, tgl_peer_id_t user);
PurpleGroup *tgp_blist_group_init (const char *name);

#endif
