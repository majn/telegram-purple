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
 
 Copyright Matthias Jentsch 2016
 */

#include "tgp-info.h"

// load photo

static void tgp_info_update_photo_id (PurpleBlistNode *node, long long photo) {
  char *llid = g_strdup_printf ("%" G_GINT64_FORMAT, (gint64) photo);
  debug ("tgp_info_update_photo_id %s", llid);
  purple_blist_node_set_string (node, TGP_INFO_PHOTO_ID, llid);
  g_free (llid);
}

static void tgp_info_load_photo_done (struct tgl_state *TLS, void *extra, int success, const char *filename) {
  tgl_peer_t *P = extra;
  
  g_return_if_fail(success);
  
  gchar *img = NULL;
  size_t len;
  GError *err = NULL;
  g_file_get_contents (filename, &img, &len, &err);
  if (err) {
    failure ("getting file contents for %s failed: %s", filename, err->message);
    return;
  }
  
  if (tgl_get_peer_type (P->id) == TGL_PEER_USER || tgl_get_peer_type (P->id) == TGL_PEER_ENCR_CHAT) {
    PurpleBuddy *B = tgp_blist_buddy_find (TLS, P->id);
    g_return_if_fail(B);
    
    purple_buddy_icons_set_for_user (tls_get_pa (TLS), purple_buddy_get_name (B),
       (guchar*) img, len, NULL);
    tgp_info_update_photo_id (&B->node, P->user.photo_big.local_id);
  } else {
    PurpleChat *C = tgp_blist_chat_find (TLS, P->id);
    g_return_if_fail(C);
    
    purple_buddy_icons_node_set_custom_icon (&C->node, (guchar*) img, len);
    tgp_info_update_photo_id (&C->node, P->user.photo_big.local_id);
  }
}

// update photo

void tgp_info_update_photo (PurpleBlistNode *node, tgl_peer_t *P) {
  tgl_peer_t *parent = NULL;

  // FIXME: test if this works for encrypted chats
  long long photo = P->user.photo_big.local_id;

  const char *old = purple_blist_node_get_string (node, TGP_INFO_PHOTO_ID);
  if (old) {
    long long id = 0;
    id = atoll (old);
    if (id == photo) {
      debug ("photo id for %s hasn't changed %lld", parent ? parent->print_name : P->print_name, id);
      return;
    }
  }

  if (photo != 0 && pbn_get_data (node) != NULL) {  // FIXME: Monkey-patched condition, I have no idea why this is NULL sometimes.
    tgl_do_load_file_location (pbn_get_data (node)->TLS, &P->user.photo_big, tgp_info_load_photo_done, P);
  } else {
    // set empty photo
    purple_buddy_icons_node_set_custom_icon_from_file (node, NULL);
    tgp_info_update_photo_id (node, photo);
  }
}


// show user info

static void tgp_info_load_channel_done (struct tgl_state *TLS, void *extra, int success, struct tgl_channel *C) {
  g_return_if_fail(success);
  
  PurpleNotifyUserInfo *info = purple_notify_user_info_new ();
  
  if (str_not_empty (C->about)) {
    purple_notify_user_info_add_pair (info, _("Description"), C->about);
  }
  
  if (str_not_empty (C->username)) {
    char *link = g_strdup_printf ("https://telegram.me/%s", C->username);
    purple_notify_user_info_add_pair (info, _("Link"), link);
    g_free (link);
  }
  
  if (str_not_empty (C->print_title)) {
    purple_notify_user_info_add_pair (info, _("Print Title"), C->print_title);
  }
  
  char *admins = g_strdup_printf ("%d", C->admins_count);
  purple_notify_user_info_add_pair (info, _("Administrators"), admins);
  g_free (admins);
  
  char *participants = g_strdup_printf ("%d", C->participants_count);
  purple_notify_user_info_add_pair (info, _("Participants"), participants);
  g_free (participants);
  
  char *kicked = g_strdup_printf ("%d", C->kicked_count);
  purple_notify_user_info_add_pair (info, _("Kicked"), kicked);
  g_free (kicked);
  
  purple_notify_userinfo (tls_get_conn (TLS), tgp_blist_lookup_purple_name (TLS, C->id), info, NULL, NULL);
}

static void tgp_info_load_user_done (struct tgl_state *TLS, void *extra, int success, struct tgl_user *U) {
  g_return_if_fail(success);
  
  // user info
  
  PurpleNotifyUserInfo *info = purple_notify_user_info_new ();
  
  if (str_not_empty (U->first_name) && str_not_empty (U->last_name)) {
    purple_notify_user_info_add_pair (info, _("First name"), U->first_name);
    purple_notify_user_info_add_pair (info, _("Last name"), U->last_name);
  } else {
    purple_notify_user_info_add_pair (info, _("Name"), U->print_name);
  }
  
  if (str_not_empty (U->username)) {
    char *username = g_strdup_printf ("@%s", U->username);
    purple_notify_user_info_add_pair (info, _("Username"), username);
    g_free (username);
  }
  
  char *status = tgp_format_user_status (&U->status);
  purple_notify_user_info_add_pair (info, _("Last seen"), status);
  g_free (status);
  
  if (str_not_empty (U->phone)) {
    char *phone = g_strdup_printf ("+%s", U->phone);
    purple_notify_user_info_add_pair (info, _("Phone"), phone);
    g_free (phone);
  }
  
  // secret chat info
  
  tgl_peer_t *O = extra;
  if (O && tgl_get_peer_type (O->id) == TGL_PEER_ENCR_CHAT) {
    
    struct tgl_secret_chat *secret = &O->encr_chat;
    
    if (secret->state == sc_waiting) {
      purple_notify_user_info_add_pair (info, "", _("Waiting for the user to get online..."));
    } else {
      const char *ttl_key = _("Self destruction timer");
      if (secret->ttl) {
        char *ttl = g_strdup_printf ("%d", secret->ttl);
        purple_notify_user_info_add_pair (info, ttl_key, ttl);
        g_free (ttl);
      } else {
        purple_notify_user_info_add_pair (info, ttl_key, _("Timer is not enabled."));
      }
      
      if (secret->first_key_sha[0]) {
        int sha1key = tgp_visualize_key (TLS, secret->first_key_sha);
        if (sha1key != -1) {
          char *ident_icon = tgp_format_img (sha1key);
          purple_notify_user_info_add_pair (info, _("Secret key"), ident_icon);
          g_free(ident_icon);
        }
      }
    }
  }
  
  const char *who = NULL;
  if (tgl_get_peer_type (O->id) == TGL_PEER_ENCR_CHAT) {
    who = tgp_blist_lookup_purple_name (TLS, O->id);
  } else {
    who = tgp_blist_lookup_purple_name (TLS, U->id);
  }
  
  purple_notify_userinfo (tls_get_conn (TLS), who, info, NULL, NULL);
}

void tgprpl_info_show (PurpleConnection *gc, const char *who) {
  tgl_peer_t *P = tgp_blist_lookup_peer_get (gc_get_data (gc)->TLS, who);
  if (P) {
    switch (tgl_get_peer_type (P->id)) {
      case TGL_PEER_ENCR_CHAT: {
        tgl_peer_t *parent = tgp_encr_chat_get_partner (gc_get_tls (gc), &P->encr_chat);
        if (parent) {
          tgl_do_get_user_info (gc_get_tls (gc), parent->id, 0, tgp_info_load_user_done, P);
        }
        break;
      }
        
      case TGL_PEER_CHANNEL:
        tgl_do_get_channel_info (gc_get_tls (gc), P->id, FALSE, tgp_info_load_channel_done, P);
        break;
        
      case TGL_PEER_USER:
        tgl_do_get_user_info (gc_get_tls (gc), P->id, 0, tgp_info_load_user_done, P);
        break;
    }
  }
}
