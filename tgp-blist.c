/* This file is part of telegram-purple 
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

#include "tgp-blist.h"
#include "tgp-structs.h"
#include "tgp-2prpl.h"

#include <glib.h>
#include <blist.h>
#include <assert.h>

/*  
  Functions for handling telegram users in the buddy list
 
  Purple prefers human-readable names for buddy usernames, while Telegram uses numerical user ids.
  In older versions of this plugin, the user id was used as username and the print name as the users
  alias. This means that getting a useful username in the interface relied on the alias resolution,
  which unfortunately doesn't work when a user isn't in the buddy list, or in Adium group chats.
  Because of that, this plugin now uses the unique print names provided by libtgl as username instead.
*/

tgl_peer_t *tgp_blist_peer_get (struct tgl_state *TLS, tgl_peer_id_t id) {
  connection_data *conn = TLS->ev_base;
  return g_hash_table_lookup (conn->id_to_tgl_peer, GINT_TO_POINTER(tgl_get_peer_id (id)));
}

const char *tgp_blist_peer_get_name (struct tgl_state *TLS, tgl_peer_id_t id) {
  tgl_peer_t *P = tgp_blist_peer_get (TLS, id);
  if (! P) {
    assert (0);
    return NULL;
  }
  return P->print_name;
}

int tgp_blist_peer_exists (struct tgl_state *TLS, tgl_peer_id_t id) {
  return tgp_blist_peer_get (TLS, id) != NULL;
}

void tgp_blist_peer_add (struct tgl_state *TLS, tgl_peer_t *peer) {
  g_hash_table_insert (tg_get_data (TLS)->id_to_tgl_peer, GINT_TO_POINTER(tgl_get_peer_id (peer->id)), peer);
}

/*
  To make this new approach robust to names changes, it is necessarry to store the user ID in each
  blist node to allow reliable buddy list lookups by user ids. Since users should be able to just
  upgrade to this version without having to drop their history or buddy list, it is also necessary
  to migrate all old buddy list nodes to the new format while preserving existing history.
*/

PurpleBuddy *tgp_blist_buddy_new  (struct tgl_state *TLS, tgl_peer_t *user) {
  PurpleBuddy *buddy = purple_buddy_new (tg_get_acc (TLS), tgp_blist_peer_get_name (TLS, user->id), NULL);
  tgp_blist_buddy_set_id (buddy, user->id);
  return buddy;
}

void tgp_blist_buddy_update_name (struct tgl_state *TLS, PurpleBuddy *buddy, struct tgl_user *user) {
  PurpleBuddyIcon *icon = purple_buddy_get_icon (buddy);
  purple_buddy_icon_ref (icon);
  purple_blist_remove_buddy (buddy);
  
  buddy = purple_buddy_new (tg_get_acc (TLS), user->print_name, NULL);
  tgp_blist_buddy_set_id (buddy, user->id);
  purple_buddy_set_icon (buddy, icon);
  purple_blist_add_buddy (buddy, NULL, tgp_blist_group_init ("Telegram"), NULL);
  
  purple_buddy_icon_unref (icon);
}

void tgp_blist_buddy_set_id (PurpleBuddy *buddy, tgl_peer_id_t id) {
  int uid = tgl_get_peer_id (id),
     type = tgl_get_peer_type (id);
  assert (uid == TGL_PEER_ENCR_CHAT || type == TGL_PEER_USER);
  
  purple_blist_node_set_int (&buddy->node, TGP_BUDDY_KEY_PEER_ID, uid);
  purple_blist_node_set_int (&buddy->node, TGP_BUDDY_KEY_PEER_TYPE, type);
}

tgl_peer_id_t tgp_blist_buddy_get_id (PurpleBuddy *buddy) {
  int id = purple_blist_node_get_int (&buddy->node, TGP_BUDDY_KEY_PEER_ID),
    type = purple_blist_node_get_int (&buddy->node, TGP_BUDDY_KEY_PEER_TYPE);

  if (type == TGL_PEER_USER) {
    return TGL_MK_USER (id);
  } else if (type == TGL_PEER_ENCR_CHAT) {
    return TGL_MK_ENCR_CHAT (id);
  } else {
    assert (FALSE);
  }
}

PurpleBuddy *tgp_blist_buddy_find (struct tgl_state *TLS, tgl_peer_id_t user) {
  PurpleBlistNode *node = purple_blist_get_root ();
  while (node) {
    if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
      PurpleBuddy *buddy = PURPLE_BUDDY(node);
      if (purple_buddy_get_account (buddy) == tg_get_acc (TLS)) {
        if (purple_blist_node_get_int (node, TGP_BUDDY_KEY_PEER_ID) == tgl_get_peer_id (user)) {
          assert (tgl_get_peer_type (user) == purple_blist_node_get_int (node, TGP_BUDDY_KEY_PEER_TYPE));
          return buddy;
        }
      }
    }
    node = purple_blist_node_next (node, FALSE);
  }
  return NULL;
}

PurpleChat *tgp_blist_chat_find (struct tgl_state *TLS, tgl_peer_id_t user) {
  PurpleBlistNode *node = purple_blist_get_root ();
  while (node) {
    if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
      PurpleChat *chat = PURPLE_CHAT(node);
      if (purple_chat_get_account (chat) == tg_get_acc (TLS)) {
        const char *id = g_hash_table_lookup (purple_chat_get_components (chat), "id");
        if (id && *id && atoi (id) == tgl_get_peer_id (user)) {
          return chat;
        }
      }
    }
    node = purple_blist_node_next (node, FALSE);
  }
  return NULL;
}

PurpleGroup *tgp_blist_group_init (const char *name) {
  PurpleGroup *grp = purple_find_group (name);
  if (grp == NULL) {
    grp = purple_group_new (name);
    purple_blist_add_group (grp, NULL);
  }
  return grp;
}
