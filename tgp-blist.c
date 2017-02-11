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

#include "telegram-purple.h"

// utilities

PurpleBlistNode *tgp_blist_iterate (struct tgl_state *TLS,
      int (* callback) (PurpleBlistNode *node, void *extra), void *extra) {
  PurpleBlistNode *node = purple_blist_get_root ();
  while (node) {
    if ((PURPLE_BLIST_NODE_IS_BUDDY(node) && purple_buddy_get_account ((PurpleBuddy *)node) == tls_get_pa (TLS))
        || (PURPLE_BLIST_NODE_IS_CHAT(node) && purple_chat_get_account ((PurpleChat *)node) == tls_get_pa (TLS))) {
      if (callback (node, extra)) {
        return node;
      }
    }
    node = purple_blist_node_next (node, FALSE);
  }
  return NULL;
}

// lookup

const char *tgp_blist_lookup_purple_name (struct tgl_state *TLS, tgl_peer_id_t id) {
  const char *name = g_hash_table_lookup (tls_get_data (TLS)->id_to_purple_name,
      GINT_TO_POINTER(tgl_get_peer_id (id)));
  g_warn_if_fail(name);
  return name;
}

void tgp_blist_lookup_add (struct tgl_state *TLS, tgl_peer_id_t id, const char *purple_name) {
  
  // to avoid issues with differences in string normalization, always store in composed form this helps to avoid
  // issues with clients like Adium, that will store strings in decomposed format by default
  const char *name = g_utf8_normalize (purple_name, -1, G_NORMALIZE_DEFAULT_COMPOSE);

  g_hash_table_replace (tls_get_data (TLS)->id_to_purple_name, GINT_TO_POINTER(tgl_get_peer_id (id)),
      g_strdup (name));
  g_hash_table_replace (tls_get_data (TLS)->purple_name_to_id, g_strdup (name),
      g_memdup (&id, sizeof(tgl_peer_id_t)));
}

static tgl_peer_id_t *tgp_blist_lookup_get_id (struct tgl_state *TLS, const char *purple_name) {
  return g_hash_table_lookup (tls_get_data (TLS)->purple_name_to_id,
      g_utf8_normalize (purple_name, -1, G_NORMALIZE_DEFAULT_COMPOSE));
}

tgl_peer_t *tgp_blist_lookup_peer_get (struct tgl_state *TLS, const char *purple_name) {
  tgl_peer_id_t *id = tgp_blist_lookup_get_id (TLS, purple_name);
  g_return_val_if_fail(id, NULL);
  return tgl_peer_get (TLS, *id);
}

static int tgp_blist_lookup_init_cb (PurpleBlistNode *node, void *extra) {
  if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
    PurpleBuddy *buddy = (PurpleBuddy *) node;
    if (tgl_get_peer_type (tgp_blist_buddy_get_id (buddy)) != TGL_PEER_UNKNOWN) {
      tgp_blist_lookup_add (pbn_get_data (node)->TLS, tgp_blist_buddy_get_id (buddy),
          purple_buddy_get_name (buddy));
    }
  }
  if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
    PurpleChat *chat = (PurpleChat *) node;
    if (tgp_chat_has_id (chat)) {
      tgp_blist_lookup_add (pbn_get_data (node)->TLS, tgp_chat_get_id (chat),
          purple_chat_get_name (chat));
    }
  }
  return FALSE;
}

void tgp_blist_lookup_init (struct tgl_state *TLS) {
  info ("loading known ids from buddy list ...");
  tgp_blist_iterate (TLS, tgp_blist_lookup_init_cb, 0);
}

// buddies

PurpleBuddy *tgp_blist_buddy_new  (struct tgl_state *TLS, tgl_peer_t *user) {
  PurpleBuddy *buddy = purple_buddy_new (tls_get_pa (TLS), tgp_blist_lookup_purple_name (TLS, user->id), NULL);
  tgp_blist_buddy_set_id (buddy, user->id);
  return buddy;
}

PurpleBuddy *tgp_blist_buddy_migrate (struct tgl_state *TLS, PurpleBuddy *buddy, struct tgl_user *user) {
  purple_blist_remove_buddy (buddy);
  buddy = purple_buddy_new (tls_get_pa (TLS), user->print_name, NULL);
  tgp_blist_buddy_set_id (buddy, user->id);
  purple_blist_add_buddy (buddy, NULL, tgp_blist_group_init (_("Telegram")), NULL);
  return buddy;
}

void tgp_blist_buddy_set_id (PurpleBuddy *buddy, tgl_peer_id_t id) {
  int uid = tgl_get_peer_id (id),
     type = tgl_get_peer_type (id);
  assert (type == TGL_PEER_ENCR_CHAT || type == TGL_PEER_USER || type == TGL_PEER_CHANNEL);
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
  } else if (type == TGL_PEER_CHANNEL) {
    return TGL_MK_CHANNEL (id);
  } else {
    return tgl_set_peer_id (TGL_PEER_UNKNOWN, 0);
  }
}

tgl_peer_t *tgp_blist_buddy_get_peer (PurpleBuddy *buddy) {
  if (! tgp_blist_buddy_has_id (buddy)) {
    g_warn_if_reached();
    return NULL;
  }
  return tgl_peer_get (pbn_get_data (&buddy->node)->TLS, tgp_blist_buddy_get_id (buddy));
}

static int tgp_blist_buddy_find_cb (PurpleBlistNode *node, void *extra) {
  return PURPLE_BLIST_NODE_IS_BUDDY(node)
      && purple_blist_node_get_int (node, TGP_BUDDY_KEY_PEER_ID) == GPOINTER_TO_INT(extra);
}

PurpleBuddy *tgp_blist_buddy_find (struct tgl_state *TLS, tgl_peer_id_t user) {
  return (PurpleBuddy *) tgp_blist_iterate (TLS, tgp_blist_buddy_find_cb, GINT_TO_POINTER(tgl_get_peer_id (user)));
}

// contacts

void tgp_blist_contact_add (struct tgl_state *TLS, struct tgl_user *U) {
  PurpleBuddy *buddy = tgp_blist_buddy_find (TLS, U->id);
  
  if (! buddy) {
    tgl_peer_t *P = tgl_peer_get (TLS, U->id);
    
    info ("Adding contact '%s' to buddy list", tgp_blist_lookup_purple_name (TLS, U->id));
    buddy = tgp_blist_buddy_new (TLS, P);
    purple_blist_add_buddy (buddy, NULL, tgp_blist_group_init (_("Telegram")), NULL);
    
    tgp_info_update_photo (&buddy->node, P);
  }
  p2tgl_prpl_got_user_status (TLS, U->id, &U->status);
}

// chats

static int tgp_blist_chat_find_cb (PurpleBlistNode *node, void *extra) {
  if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
    PurpleChat *chat = PURPLE_CHAT(node);
    const char *id = g_hash_table_lookup (purple_chat_get_components (chat), "id");
    if (id && *id && atoi (id) == GPOINTER_TO_INT(extra)) {
      return TRUE;
    }
  }
  return FALSE;
}

PurpleChat *tgp_blist_chat_find (struct tgl_state *TLS, tgl_peer_id_t user) {
  return (PurpleChat *) tgp_blist_iterate (TLS, tgp_blist_chat_find_cb, GINT_TO_POINTER(tgl_get_peer_id (user)));
}

// groups

PurpleGroup *tgp_blist_group_init (const char *name) {
  PurpleGroup *grp = purple_find_group (name);
  if (! grp) {
    grp = purple_group_new (name);
    purple_blist_add_group (grp, NULL);
  }
  return grp;
}


// names

char *tgp_blist_create_print_name (struct tgl_state *TLS, tgl_peer_id_t id, const char *a1, const char *a2,
    const char *a3, const char *a4) {

  // libtgl passes 0 for all unused strings, therefore the last passed string will always be followed
  // by a NULL-termination as expected
  gchar *name = g_strjoin (" ", a1, a2, a3, a4, NULL);

  // When the user doesn't provide some input (like last name) ugly trailing or leading
  // whitespaces may occur due to empty strings in the g_strjoin arguments
  name = g_strstrip(name);

  /* Assure that all print_names are unique by checking the following conditions:
     1. No other peer with that print_name should exists. If those names are not unique it will not be possible
        to uniquely refer to users by their print_name and assertions in libtgl will fail.
     2. No BlistNode with that name should exists unless it is already corresponding to this peer ID. The rationale is
        that this prpl uses the first print_name as permanent name for each user. Therefore it must be assured that no
        foreign user will ever take this exact name again, otherwise the current users actions might be associated
        with the old BlistNode.
     3. Assure that the print name isn't already stored in the peer_by_name_tree.
   */
  int i = 0;
  gchar *n = NULL;

  tgl_peer_id_t *id2 = tgp_blist_lookup_get_id (TLS, name);
  if ( !id2) {
    tgl_peer_t *tmpP = tgl_peer_get_by_name (TLS, name);
    if (tmpP) {
      id2 = &tmpP->id;
    }
  }

  while (id2 && tgl_get_peer_id (*id2) != tgl_get_peer_id (id)) {
    if (n) {
      g_free (n);
    }
    n = g_strdup_printf ("%s #%d", name, ++ i);
    debug ("resolving duplicate for %s, assigning: %s", name, n);
    
    id2 = tgp_blist_lookup_get_id (TLS, n);
    if ( !id2) {
      tgl_peer_t *tmpP = tgl_peer_get_by_name(TLS, n);
      if (tmpP) {
        id2 = &tmpP->id;
      }
    }
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
