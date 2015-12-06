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

#include "telegram-purple.h"

const char *tgp_blist_peer_get_purple_name (struct tgl_state *TLS, tgl_peer_id_t id) {
  const char *name = g_hash_table_lookup (tls_get_data (TLS)->id_to_purple_name, GINT_TO_POINTER(tgl_get_peer_id (id)));
  if (! name) {
    g_warn_if_reached();
    return NULL;
  }
  return name;
}

void tgp_blist_peer_add_purple_name (struct tgl_state *TLS, tgl_peer_id_t id, const char *purple_name) {
  g_hash_table_replace (tls_get_data (TLS)->id_to_purple_name, GINT_TO_POINTER(tgl_get_peer_id (id)),
      g_strdup (purple_name));
}

tgl_peer_t *tgp_blist_peer_find (struct tgl_state *TLS, const char *purple_name) {
  // buddies will keep the name they had when they were first added to the user list. The print_name of the peer
  // may have changed since then, therefore the ID stored in the buddy is used to fetch the user name.
  PurpleBuddy *buddy = purple_find_buddy (tls_get_pa (TLS), purple_name);
  if (! buddy) {
    // foreign users are not in the buddy list by default, therefore the name used by libpurple and the
    // print name is always identical
    return tgl_peer_get_by_name (TLS, purple_name);
  }
  if (! tgp_blist_buddy_has_id (buddy)) {
    g_warn_if_reached ();
    return NULL;
  }
  return tgl_peer_get (TLS, tgp_blist_buddy_get_id (buddy));
}

PurpleBuddy *tgp_blist_buddy_new  (struct tgl_state *TLS, tgl_peer_t *user) {
  PurpleBuddy *buddy = purple_buddy_new (tls_get_pa (TLS), tgp_blist_peer_get_purple_name (TLS, user->id), NULL);
  tgp_blist_buddy_set_id (buddy, user->id);
  return buddy;
}

PurpleBuddy *tgp_blist_buddy_migrate (struct tgl_state *TLS, PurpleBuddy *buddy, struct tgl_user *user) {
  purple_blist_remove_buddy (buddy);
  buddy = purple_buddy_new (tls_get_pa (TLS), user->print_name, NULL);
  tgp_blist_buddy_set_id (buddy, user->id);
  purple_blist_add_buddy (buddy, NULL, tgp_blist_group_init ("Telegram"), NULL);
  return buddy;
}

void tgp_blist_buddy_set_id (PurpleBuddy *buddy, tgl_peer_id_t id) {
  int uid = tgl_get_peer_id (id),
     type = tgl_get_peer_type (id);
  assert (type == TGL_PEER_ENCR_CHAT || type == TGL_PEER_USER);
  
  purple_blist_node_set_int (&buddy->node, TGP_BUDDY_KEY_PEER_ID, uid);
  purple_blist_node_set_int (&buddy->node, TGP_BUDDY_KEY_PEER_TYPE, type);
}

int tgp_blist_buddy_has_id (PurpleBuddy *buddy) {
  return purple_blist_node_get_int (&buddy->node, TGP_BUDDY_KEY_PEER_ID) != 0;
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
    // avoid compiler errors for missing return value
    return TGL_MK_USER(0);
  }
}

tgl_peer_t *tgp_blist_buddy_get_peer (PurpleBuddy *buddy) {
  if (! tgp_blist_buddy_has_id (buddy)) {
    g_warn_if_reached();
    return NULL;
  }
  return tgl_peer_get (pbn_get_data (&buddy->node)->TLS, tgp_blist_buddy_get_id (buddy));
}

PurpleBuddy *tgp_blist_buddy_find (struct tgl_state *TLS, tgl_peer_id_t user) {
  PurpleBlistNode *node = purple_blist_get_root ();
  while (node) {
    if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
      PurpleBuddy *buddy = PURPLE_BUDDY(node);
      if (purple_buddy_get_account (buddy) == tls_get_pa (TLS)) {
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
      if (purple_chat_get_account (chat) == tls_get_pa (TLS)) {
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
  const char *translation = gettext (name);
  PurpleGroup *grp = purple_find_group (translation);
  if (! grp) {
    grp = purple_group_new (translation);
    purple_blist_add_group (grp, NULL);
  }

  // purple group names weren't translated in earlier versions, to avoid duplicated groups and messing up the buddy
  // list migrate all those groups manually
  if (0 != strcmp (name, translation)) {
    PurpleGroup *old = purple_find_group (name);
    if (old) {
      info ("group: non-translated group detected, migrating from %s to %s", name, translation);
      purple_blist_rename_group (old, translation);
    }
  }
  return grp;
}

char *tgp_blist_create_print_name (struct tgl_state *TLS, tgl_peer_id_t id, const char *a1, const char *a2,
    const char *a3, const char *a4) {
  // libtgl passes 0 for all unused strings, therefore the last passed string will always be followed
  // by a NULL-termination as expected
  gchar *name = g_strjoin (" ", a1, a2, a3, a4, NULL);

  /* Assure that all print_names are unique by checking the following conditions:
     1. No other peer with that print_name should exists. If those names are not unique it will not be possible
        to uniquely refer to users by their print_name and assertions in libtgl will fail.
     2. No BlistNode with that name should exists unless it is already corresponding to this peer ID. The rationale is
        that this prpl uses the first print_name as permanent name for each user. Therefore it must be assured that no
        foreign user will ever take this exact name again, otherwise the current users actions might be associated
        with the old BlistNode. */
  int i = 0;
  gchar *n = NULL;
  tgl_peer_t *B = tgp_blist_peer_find (TLS, name);
  while (B && tgl_get_peer_id (B->id) != tgl_get_peer_id (id)) {
    if (n) {
      g_free (n);
    }
    n = g_strdup_printf ("%s #%d", name, ++ i);
    B = tgp_blist_peer_find (TLS, n);
  }
  if (n) {
    g_free (name);
    name = n;
  }

  // the result is owned and freed by libtgl and must not be allocated by glib functions
  char *S = tstrdup (name);
  g_free (name);
  return S;
}

