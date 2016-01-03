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

static void tgp_info_load_photo_done (struct tgl_state *TLS, void *extra, int success,
    const char *filename) {
  struct tgp_info_load_photo_data *data = extra;

  if (! success) {
    data->callback (TLS, data->extra, NULL, 0, 0);
  } else {
    gchar *img = NULL;
    size_t len;
    GError *err = NULL;
    g_file_get_contents (filename, &img, &len, &err);

    if (err) {
      failure ("getting file contents for %s failed: %s", filename, err->message);
      data->callback (TLS, data->extra, NULL, 0, FALSE);
      return;
    }
    data->callback (TLS, data->extra, img, len, success);
  }
  free (data);
}

static void tgp_info_update_photo_chat_done (struct tgl_state *TLS, void *extra, int success, struct tgl_chat *C) {
  g_return_if_fail(success);
  g_return_if_fail(C->photo);
  tgl_do_load_photo (TLS, C->photo, tgp_info_load_photo_done, extra);
}

static void tgp_info_update_photo_user_done (struct tgl_state *TLS, void *extra, int success, struct tgl_user *U) {
  g_return_if_fail(success);
  g_return_if_fail(U->photo);
  tgl_do_load_photo (TLS, U->photo, tgp_info_load_photo_done, extra);
}

static void tgp_info_update_photo_channel_done (struct tgl_state *TLS, void *extra, int success, struct tgl_channel *CH) {
  g_return_if_fail(success);
  g_return_if_fail(CH->photo);
  tgl_do_load_photo (TLS, CH->photo, tgp_info_load_photo_done, extra);
}

void tgp_info_load_photo_peer (struct tgl_state *TLS, tgl_peer_t *P, void *extra,
       void (*callback) (struct tgl_state *TLS, void *extra, gchar *img, size_t len, int success)) {
  assert(callback);

  struct tgp_info_load_photo_data *D = talloc0 (sizeof(struct tgp_info_load_photo_data));
  D->callback = callback;
  D->extra = extra;
  D->TLS = TLS;

  switch (tgl_get_peer_type (P->id)) {
    case TGL_PEER_CHANNEL:
      tgl_do_get_channel_info (TLS, P->id, FALSE, tgp_info_update_photo_channel_done, D);
      break;

    case TGL_PEER_USER:
      tgl_do_get_user_info (TLS, P->id, FALSE, tgp_info_update_photo_user_done, D);
      break;

    case TGL_PEER_CHAT:
      tgl_do_get_chat_info (TLS, P->id, FALSE, tgp_info_update_photo_chat_done, D);
      break;

    case TGL_PEER_ENCR_CHAT: {
      tgl_peer_t *parent = tgp_encr_chat_get_partner (TLS, &P->encr_chat);
      g_return_if_fail(parent);
      tgl_do_get_user_info (TLS, parent->id, FALSE, tgp_info_update_photo_user_done, D);
      break;
    }

    default:
      g_warn_if_reached();
      break;
  }
}


// update photo

static void tgp_info_update_photo_done (struct tgl_state *TLS, void *extra, gchar *img, size_t len, int success) {
  PurpleBuddy *node = extra;
  g_return_if_fail(success);
  purple_buddy_icons_set_for_user (tls_get_pa (TLS), purple_buddy_get_name (node), img, len, NULL);
}

void tgp_info_update_photo (PurpleBuddy *buddy, tgl_peer_t *P) {
  PurpleBlistNode *node = &buddy->node;
  tgl_peer_t *parent = NULL;

  long long photo = 0;
  switch (tgl_get_peer_type (P->id)) {
    case TGL_PEER_USER:
      photo = P->user.photo_id;
      break;

    case TGL_PEER_CHANNEL:
      photo = P->channel.photo_id;
      break;

    case TGL_PEER_ENCR_CHAT: {
      parent = tgp_encr_chat_get_partner (pbn_get_data (node)->TLS, &P->encr_chat);
      photo = parent->photo_id;
      break;
    }

    default:
      return;
  }

  const char *old = purple_blist_node_get_string (node, TGP_INFO_PHOTO_ID);
  if (old) {
    long long id = 0;
    id = atoll (old);
    if (id == photo) {
      debug ("photo id for %s hasn't changed %lld", buddy->name, id);
      return;
    }
  }

  if (photo != 0) {
    tgp_info_load_photo_peer (pbn_get_data (node)->TLS, parent ? parent : P, node, tgp_info_update_photo_done);
  } else {
    purple_buddy_icons_set_for_user (purple_buddy_get_account (buddy), purple_buddy_get_name (buddy), NULL,
        0, NULL);
  }

  // FIXME: call this in tgp_info_update_photo_done, right now just hope for the best
  char *llid = g_strdup_printf ("%lld", photo);
  debug ("tgl_info_update_photo %s", llid);
  purple_blist_node_set_string (node, TGP_INFO_PHOTO_ID, llid);
  g_free (llid);
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
