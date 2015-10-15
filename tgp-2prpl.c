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

    Copyright Matthias Jentsch 2014
*/

#include "telegram-purple.h"

#ifdef HAVE_LIBWEBP
#include <webp/decode.h>
#include "lodepng/lodepng.h"
#endif

#include "tgp-2prpl.h"
#include "tgp-structs.h"
#include "telegram-purple.h"
#include "tgp-utils.h"
#include "telegram-base.h"

#include <server.h>
#include <tgl.h>
#include "msglog.h"
#include <assert.h>


PurpleAccount *tg_get_acc (struct tgl_state *TLS) {
  return tg_get_data(TLS)->pa;
}

PurpleConnection *tg_get_conn (struct tgl_state *TLS) {
  return tg_get_data (TLS)->gc;
}

connection_data *tg_get_data (struct tgl_state *TLS) {
  return TLS->ev_base;
}

connection_data *gc_get_conn (PurpleConnection *gc) {
  return purple_connection_get_protocol_data (gc);
}

connection_data *pa_get_conn (PurpleAccount *pa) {
  return purple_connection_get_protocol_data (purple_account_get_connection (pa));
}

connection_data *pbn_get_conn (PurpleBlistNode *node) {
  if (PURPLE_BLIST_NODE_IS_CHAT (node)) {
    return pa_get_conn (purple_chat_get_account ((PurpleChat *)node));
  }
  if (PURPLE_BLIST_NODE_IS_BUDDY (node)) {
    return pa_get_conn (purple_buddy_get_account ((PurpleBuddy *)node));
  }
  return NULL;
}

int p2tgl_status_is_present (PurpleStatus *status) {
  const char *name = purple_status_get_id (status);
  return !(strcmp (name, "unavailable") == 0 || strcmp (name, "away") == 0);
}

int p2tgl_send_notifications (PurpleAccount *acct) {
  int ret = purple_account_get_bool (acct, TGP_KEY_SEND_READ_NOTIFICATIONS, TGP_DEFAULT_SEND_READ_NOTIFICATIONS);
  debug ("sending notifications: %d", ret);
  return ret;
}

void p2tgl_got_chat_left (struct tgl_state *TLS, tgl_peer_id_t chat) {
  serv_got_chat_left (tg_get_conn(TLS), tgl_get_peer_id(chat));
}

void p2tgl_got_chat_in (struct tgl_state *TLS, tgl_peer_id_t chat, tgl_peer_id_t who,
                        const char *message, int flags, time_t when) {
  serv_got_chat_in (tg_get_conn(TLS), tgl_get_peer_id (chat), tgp_blist_peer_get_name (TLS, who), flags, message, when);
}

void p2tgl_got_im_combo (struct tgl_state *TLS, tgl_peer_id_t who, const char *msg, int flags, time_t when) {
  connection_data *conn = TLS->ev_base;
  
  /* 
     Outgoing messages are not well supported in different libpurple clients, 
     purple_conv_im_write should have the best among different versions. Unfortunately
     this causes buggy formatting in Adium, so we don't use this workaround in that case.
   
     NOTE: Outgoing messages will not work in Adium <= 1.6.0, there is no way to print outgoing
     messages in those versions at all.
   */
#ifndef __ADIUM_
  if (flags & PURPLE_MESSAGE_SEND) {
    PurpleConversation *conv = p2tgl_find_conversation_with_account (TLS, who);
    if (!conv) {
      conv = purple_conversation_new (PURPLE_CONV_TYPE_IM, tg_get_acc (TLS),
                                      tgp_blist_peer_get_name (TLS, who));
    }
    purple_conv_im_write (purple_conversation_get_im_data (conv), tgp_blist_peer_get_name (TLS, who),
                          msg, PURPLE_MESSAGE_SEND, when);
    return;
  }
#endif
  serv_got_im (conn->gc, tgp_blist_peer_get_name (TLS, who), msg, flags, when);
}

PurpleConversation *p2tgl_find_conversation_with_account (struct tgl_state *TLS, tgl_peer_id_t peer) {
  int type = PURPLE_CONV_TYPE_IM;
  if (tgl_get_peer_type (peer) == TGL_PEER_CHAT) {
    type = PURPLE_CONV_TYPE_CHAT;
  }
  PurpleConversation *conv = purple_find_conversation_with_account (type,
                                tgp_blist_peer_get_name (TLS, peer), tg_get_acc (TLS));
  return conv;
}

void p2tgl_prpl_got_user_status (struct tgl_state *TLS, tgl_peer_id_t user, struct tgl_user_status *status) {
  connection_data *data = TLS->ev_base;
  
  if (status->online == 1) {
    purple_prpl_got_user_status (tg_get_acc (TLS), tgp_blist_peer_get_name (TLS, user), "available", NULL);
  } else {
    debug ("%d: when=%d", tgl_get_peer_id (user), status->when);
    if (tgp_time_n_days_ago (purple_account_get_int (data->pa, "inactive-days-offline", TGP_DEFAULT_INACTIVE_DAYS_OFFLINE)) > status->when && status->when) {
      debug ("offline");
      purple_prpl_got_user_status (tg_get_acc (TLS), tgp_blist_peer_get_name (TLS, user), "offline", NULL);
    } else {
      debug ("mobile");
      purple_prpl_got_user_status (tg_get_acc (TLS), tgp_blist_peer_get_name (TLS, user), "mobile", NULL);
    }
  }
}

tgl_chat_id_t p2tgl_chat_get_id (PurpleChat *PC) {
  char *name = g_hash_table_lookup (purple_chat_get_components (PC), "id");
  if (! name || ! atoi (name)) {
    warning ("p2tgl_chat_id_get: no id found in chat %s", PC->alias);
    return TGL_MK_CHAT(0);
  }
  return TGL_MK_CHAT(atoi (name));
}

void p2tgl_conv_add_user (struct tgl_state *TLS, PurpleConversation *conv,
                          int user, char *message, int flags, int new_arrival) {
  purple_conv_chat_add_user (purple_conversation_get_chat_data (conv), tgp_blist_peer_get_name (TLS, TGL_MK_USER (user)), message, flags, new_arrival);
}

PurpleNotifyUserInfo *p2tgl_notify_user_info_new (struct tgl_user *U) {
  PurpleNotifyUserInfo *info = purple_notify_user_info_new();
  
  if (str_not_empty (U->first_name) && str_not_empty (U->last_name)) {
    purple_notify_user_info_add_pair (info, _("First Name"), U->first_name);
    purple_notify_user_info_add_pair (info, _("Last Name"), U->last_name);
  } else {
    purple_notify_user_info_add_pair (info, _("Name"), U->print_name);
  }
  
  if (str_not_empty (U->username)) {
    char *username = g_strdup_printf ("@%s", U->username);
    purple_notify_user_info_add_pair (info, _("User Name"), username);
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
 
  return info;
}

PurpleNotifyUserInfo *p2tgl_notify_encrypted_chat_info_new (struct tgl_state *TLS, struct tgl_secret_chat *secret,
                                                            struct tgl_user *U) {
  PurpleNotifyUserInfo *info = p2tgl_notify_user_info_new (U);
  
  if (secret->state == sc_waiting) {
    purple_notify_user_info_add_pair (info, "", _("Waiting for the user to get online ..."));
    return info;
  }
  
  const char *ttl_key = _("Self destructiom timer");
  if (secret->ttl) {
    char *ttl = g_strdup_printf ("%d", secret->ttl);
    purple_notify_user_info_add_pair (info, ttl_key, ttl);
    g_free (ttl);
  } else {
    purple_notify_user_info_add_pair (info, ttl_key, _("Off"));
  }
  
  if (secret->first_key_sha[0]) {
    int sha1key_store_id = tgp_visualize_key (TLS, secret->first_key_sha);
    if (sha1key_store_id != -1) {
      char *ident_icon = tgp_format_img (sha1key_store_id);
      purple_notify_user_info_add_pair (info, _("Secret key"), ident_icon);
      g_free(ident_icon);
    }
  }
  
  return info;
}

PurpleNotifyUserInfo *p2tgl_notify_peer_info_new (struct tgl_state *TLS, tgl_peer_t *P) {
  switch (tgl_get_peer_type (P->id)) {
    case TGL_PEER_ENCR_CHAT: {
      struct tgl_secret_chat *chat = &P->encr_chat;
      tgl_peer_t *partner = tgp_encr_chat_get_partner (TLS, chat);
      return p2tgl_notify_encrypted_chat_info_new (TLS, chat, &partner->user);
      break;
    }
      
    case TGL_PEER_USER:
      return p2tgl_notify_user_info_new (&P->user);
      break;
      
    default:
      return purple_notify_user_info_new ();
  }
}

int p2tgl_imgstore_add_with_id (const char* filename) {
  gchar *data = NULL;
  size_t len;
  GError *err = NULL;
  g_file_get_contents (filename, &data, &len, &err);
  
  int id = purple_imgstore_add_with_id (data, len, NULL);
  return id;
}

#ifdef HAVE_LIBWEBP
int p2tgl_imgstore_add_with_id_webp (const char *filename) {
  
  const uint8_t *data = NULL;
  size_t len;
  GError *err = NULL;
  g_file_get_contents (filename, (gchar **) &data, &len, &err);
  if (err) { warning ("cannot open file %s: %s.", filename, err->message); return 0; }
  
  // downscale oversized sticker images displayed in chat, otherwise it would harm readabillity
  WebPDecoderConfig config;
  WebPInitDecoderConfig (&config);
  if (! WebPGetFeatures(data, len, &config.input) == VP8_STATUS_OK) {
    warning ("error reading webp bitstream: %s", filename);
    g_free ((gchar *)data);
    return 0;
  }
  int H = config.input.height;
  int W = config.input.width;
  while (H > 256 || W > 256) {
    H /= 2;
    W /= 2;
  }
  config.options.use_scaling = 1;
  config.options.scaled_width = W;
  config.options.scaled_height = H;
  config.output.colorspace = MODE_RGBA;
  if (! WebPDecode(data, len, &config) == VP8_STATUS_OK) {
    warning ("error decoding webp: %s", filename);
    g_free ((gchar *)data);
    return 0;
  }
  g_free ((gchar *)data);
  const uint8_t *decoded = config.output.u.RGBA.rgba;

  // convert to png
  unsigned char* png = NULL;
  size_t pnglen;
  unsigned error = lodepng_encode32 (&png, &pnglen, decoded, config.options.scaled_width, config.options.scaled_height);
  WebPFreeDecBuffer (&config.output);
  if (error) {
    warning ("error encoding webp as png: %s", filename);
    return 0;
  }
  
  // will be owned by libpurple imgstore, which uses glib functions for managing memory
  void *pngdub = g_memdup (png, (guint)pnglen);
  free (png);
  
  int imgStoreId = purple_imgstore_add_with_id (pngdub, pnglen, NULL);
  return imgStoreId;
}
#endif

void p2tgl_buddy_icons_set_for_user (PurpleAccount *pa, tgl_peer_id_t id, const char* filename) {
  connection_data *conn = purple_connection_get_protocol_data (purple_account_get_connection (pa));

  gchar *data = NULL;
  size_t len;
  GError *err = NULL;
  g_file_get_contents (filename, &data, &len, &err);
  purple_buddy_icons_set_for_user (conn->pa, tgp_blist_peer_get_name (conn->TLS, id), data, len, NULL);
}

