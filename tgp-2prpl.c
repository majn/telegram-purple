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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBWEBP
#include <webp/decode.h>
#include "lodepng/lodepng.h"
#endif

#include "telegram-purple.h"
#include "tgp-2prpl.h"
#include "tgp-structs.h"
#include "telegram-purple.h"
#include "tgp-utils.h"
#include "telegram-base.h"

#include <server.h>
#include <tgl.h>
#include <msglog.h>
#include <assert.h>

PurpleAccount *tg_get_acc (struct tgl_state *TLS) {
  return (PurpleAccount *) ((connection_data *)TLS->ev_base)->pa;
}

PurpleConnection *tg_get_conn (struct tgl_state *TLS) {
  return (PurpleConnection *) ((connection_data *)TLS->ev_base)->gc;
}

char *p2tgl_strdup_id (tgl_peer_id_t user) {
  return g_strdup_printf("%d", tgl_get_peer_id(user));
}

gchar *p2tgl_strdup_alias (tgl_peer_t *user) {
  return g_strdup (user->print_name);
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

/* 
   Disclaimer: I stole this function from davidgfnet's whatsapp plugin, all 
   credit for it goes to him
   @see: https://github.com/davidgfnet/whatsapp-purple
 */
static PurpleChat *blist_find_chat_by_hasht_cond (PurpleConnection *gc,
    int (*fn)(GHashTable *hasht, void *data), void *data) {
  PurpleAccount *account = purple_connection_get_account(gc);
  PurpleBlistNode *node = purple_blist_get_root();
  GHashTable *hasht;
  while (node) {
    if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
      PurpleChat *ch = PURPLE_CHAT(node);
      if (purple_chat_get_account(ch) == account) {
        hasht = purple_chat_get_components(ch);
        if (fn(hasht, data))
          return ch;
      }
    }
    node = purple_blist_node_next(node, 0);
  }
  return NULL;
}

static int hasht_cmp_id(GHashTable *hasht, void *data) {
  gpointer id = g_hash_table_lookup(hasht, "id");
  if (!id || !data) {
    return 0;
  }
  return !strcmp(id, ((char *)data));
}


PurpleConversation *p2tgl_got_joined_chat (struct tgl_state *TLS, struct tgl_chat *chat) {
  connection_data *conn = TLS->ev_base;
  gchar *alias = p2tgl_strdup_alias ((tgl_peer_t *)chat);
  
  PurpleConversation *conv = serv_got_joined_chat (conn->gc, tgl_get_peer_id(chat->id), alias);
  
  g_free(alias);
  return conv;
}

void p2tgl_got_chat_left (struct tgl_state *TLS, tgl_peer_id_t chat) {
  serv_got_chat_left(tg_get_conn(TLS), tgl_get_peer_id(chat));
}

void p2tgl_got_chat_in (struct tgl_state *TLS, tgl_peer_id_t chat, tgl_peer_id_t who,
                        const char *message, int flags, time_t when) {
  char *name = p2tgl_strdup_id (who);
  
  serv_got_chat_in (tg_get_conn(TLS), tgl_get_peer_id (chat), name, flags, message, when);
  
  g_free (name);
}

void p2tgl_got_alias (struct tgl_state *TLS, tgl_peer_id_t who, const char *alias) {
  char *name = p2tgl_strdup_id(who);
  
  serv_got_alias(tg_get_conn(TLS), name, alias);
  
  g_free (name);
}

void p2tgl_got_im (struct tgl_state *TLS, tgl_peer_id_t who, const char *msg, int flags, time_t when) {
  char *name = p2tgl_strdup_id(who);
  
  serv_got_im(tg_get_conn(TLS), name, msg,flags, when);
  
  g_free (name);
}

void p2tgl_conversation_write (PurpleConversation *conv, tgl_peer_id_t who, const char *message, int flags, int date) {
  char *name = p2tgl_strdup_id (who);
  
  purple_conversation_write (conv, name, message, flags, date);
  
  g_free (name);
}

void p2tgl_conv_im_write (PurpleConversation *conv, tgl_peer_id_t who, const char *message, int flags, int date) {
  char *name = p2tgl_strdup_id (who);
  
  purple_conv_im_write(purple_conversation_get_im_data(conv), name, message, flags, date);
  
  g_free (name);
}

void p2tgl_got_im_combo (struct tgl_state *TLS, tgl_peer_id_t who, const char *msg, int flags, time_t when) {
  
  /* 
     Outgoing messages are not well supported in different libpurple clients, 
     p2tgl_conv_im_write should have the best among different versions. Unfortunately
     this causes buggy formatting in Adium, so we don't use this workaround in that case.
   
     NOTE: Outgoing messages will not work in Adium <= 1.6.0, there is no way to print outgoing
     messages in those versions at all.
   */
#ifndef __ADIUM_
  if (flags & PURPLE_MESSAGE_SEND) {
    PurpleConversation *conv = p2tgl_find_conversation_with_account (TLS, who);
    if (!conv) {
      conv = p2tgl_conversation_new(TLS, who);
    }
    p2tgl_conv_im_write (conv, who, msg, PURPLE_MESSAGE_SEND, when);
    return;
  }
#endif

  p2tgl_got_im (TLS, who, msg, flags, when);
}


void p2tgl_got_typing (struct tgl_state *TLS, tgl_peer_id_t user, int timeout) {
  char *who = g_strdup_printf("%d", tgl_get_peer_id(user));
  
  serv_got_typing(tg_get_conn(TLS), who, timeout, PURPLE_TYPING);
  
  g_free(who);
}


PurpleBuddy *p2tgl_buddy_find (struct tgl_state *TLS, tgl_peer_id_t user) {
  gchar *name = p2tgl_strdup_id(user);
  
  PurpleBuddy *b = purple_find_buddy (tg_get_acc(TLS), name);
  
  g_free (name);
  return b;
}

PurpleConversation *p2tgl_find_conversation_with_account (struct tgl_state *TLS, tgl_peer_id_t peer) {
  int type = PURPLE_CONV_TYPE_IM;
  if (tgl_get_peer_type (peer) == TGL_PEER_CHAT) {
    type = PURPLE_CONV_TYPE_CHAT;
  }
  char *who = g_strdup_printf("%d", tgl_get_peer_id(peer));
  
  PurpleConversation *conv = purple_find_conversation_with_account (type, who, tg_get_acc(TLS));
  
  g_free (who);
  return conv;
}

PurpleConversation *p2tgl_conversation_new (struct tgl_state *TLS, tgl_peer_id_t who) {
  int type =  tgl_get_peer_type (who) == TGL_PEER_CHAT ? PURPLE_CONV_TYPE_CHAT : PURPLE_CONV_TYPE_IM;
  
  char *name = p2tgl_strdup_id (who);
  PurpleConversation *conv = purple_conversation_new(type, tg_get_acc(TLS), name);
  g_free (name);
  
  return conv;
}


PurpleBuddy *p2tgl_buddy_new  (struct tgl_state *TLS, tgl_peer_t *user) {
  char *alias = p2tgl_strdup_alias (user);
  char *name  = p2tgl_strdup_id (user->id);
  
  PurpleBuddy *b = purple_buddy_new (tg_get_acc(TLS), name, alias);
  
  g_free (alias);
  g_free (name);
  return b;
}

PurpleBuddy *p2tgl_buddy_update (struct tgl_state *TLS, tgl_peer_t *user, unsigned flags) {
  PurpleBuddy *b = p2tgl_buddy_find (TLS, user->id);
  if (!b) {
    b = p2tgl_buddy_new (TLS, user);
  }
  if (flags & (TGL_UPDATE_NAME | TGL_UPDATE_REAL_NAME | TGL_UPDATE_USERNAME)) {
    debug ("Update username for id%d (name %s %s)", tgl_get_peer_id (user->id), user->user.first_name, user->user.last_name);
    char *alias = p2tgl_strdup_alias (user);
    purple_blist_alias_buddy(b, alias);
    g_free (alias);
  }
  return b;
}

void p2tgl_prpl_got_set_status_mobile (struct tgl_state *TLS, tgl_peer_id_t user) {
  char *name = p2tgl_strdup_id (user);
  
  purple_prpl_got_user_status (tg_get_acc(TLS), name, "mobile", NULL);
  
  g_free (name);
}

void p2tgl_prpl_got_set_status_offline (struct tgl_state *TLS, tgl_peer_id_t user) {
  char *name = p2tgl_strdup_id (user);
  
  purple_prpl_got_user_status (tg_get_acc(TLS), name, "offline", NULL);
  
  g_free (name);
}

void p2tgl_prpl_got_set_status_online (struct tgl_state *TLS, tgl_peer_id_t user) {
  char *name = p2tgl_strdup_id (user);
  
  purple_prpl_got_user_status (tg_get_acc(TLS), name, "available", NULL);
  
  g_free (name);
}

void p2tgl_prpl_got_user_status (struct tgl_state *TLS, tgl_peer_id_t user, struct tgl_user_status *status) {
  connection_data *data = TLS->ev_base;
  
  if (status->online == 1) {
    p2tgl_prpl_got_set_status_online (TLS, user);
  } else {
    debug ("%d: when=%d", user.id, status->when);
    if (tgp_time_n_days_ago (purple_account_get_int (data->pa, "inactive-days-offline", TGP_DEFAULT_INACTIVE_DAYS_OFFLINE)) > status->when && status->when) {
      debug ("offline");
      p2tgl_prpl_got_set_status_offline (TLS, user);
    }
    else {
      debug ("mobile");
      p2tgl_prpl_got_set_status_mobile (TLS, user);
    }
  }
}

PurpleChat *p2tgl_blist_find_chat(struct tgl_state *TLS, tgl_peer_id_t chat) {
  char *name = p2tgl_strdup_id(chat);
  
  PurpleChat *c = purple_blist_find_chat(tg_get_acc(TLS), name);
  
  g_free (name);
  return c;
}

PurpleChat *p2tgl_chat_new (struct tgl_state *TLS, struct tgl_chat *chat) {
  gchar *admin = g_strdup_printf("%d", chat->admin_id);
  gchar *title = g_strdup(chat->print_title);
  gchar *name  = p2tgl_strdup_id (chat->id);
  
  GHashTable *ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
  g_hash_table_insert(ht, g_strdup("subject"), title);
  g_hash_table_insert(ht, g_strdup("id"), name);
  g_hash_table_insert(ht, g_strdup("owner"), admin);
  
  PurpleChat *C = purple_chat_new(tg_get_acc(TLS), chat->title, ht);
  return C;
}

PurpleChat *p2tgl_chat_find (struct tgl_state *TLS, tgl_peer_id_t id) {
  char *name = p2tgl_strdup_id(id);
  PurpleChat *c = blist_find_chat_by_hasht_cond(tg_get_conn(TLS), hasht_cmp_id, name);
  g_free(name);
  return c;
}

void p2tgl_conv_add_user (PurpleConversation *conv, struct tgl_chat_user user, char *message, int flags, int new_arrival) {
  PurpleConvChat *cdata = purple_conversation_get_chat_data(conv);
  char *name = g_strdup_printf("%d", user.user_id);
  
  purple_conv_chat_add_user(cdata, name, message, flags, new_arrival);
  
  g_free(name);
}

void p2tgl_connection_set_display_name(struct tgl_state *TLS, tgl_peer_t *user) {
  char *name = p2tgl_strdup_alias(user);
  purple_connection_set_display_name(tg_get_conn(TLS), name);
  g_free(name);
}


void *p2tgl_notify_userinfo(struct tgl_state *TLS, tgl_peer_id_t user, PurpleNotifyUserInfo *user_info, PurpleNotifyCloseCallback cb, gpointer user_data) {
  char *name = p2tgl_strdup_id(user);
  void *handle = 0;
  
  handle = purple_notify_userinfo(tg_get_conn(TLS), name, user_info, cb, user_data);
  
  g_free(name);
  g_free(user_info);
  return handle;
}

void p2tgl_blist_alias_buddy (PurpleBuddy *buddy, struct tgl_user *user) {
  char *name = p2tgl_strdup_alias ((tgl_peer_t *) user);
  
  purple_blist_alias_buddy (buddy, name);
  
  g_free(name);
}

PurpleNotifyUserInfo *p2tgl_notify_user_info_new (struct tgl_user *U) {
  PurpleNotifyUserInfo *info = purple_notify_user_info_new();
  
  if (str_not_empty(U->first_name) && str_not_empty(U->last_name)) {
    purple_notify_user_info_add_pair (info, "First name", U->first_name);
    purple_notify_user_info_add_pair (info, "Last name", U->last_name);
  } else {
    purple_notify_user_info_add_pair (info, "Name", U->print_name);
  }
  
  if (str_not_empty (U->username)) {
    purple_notify_user_info_add_pair (info, "Username", U->username);
  }
  
  char *status = tgp_format_user_status (&U->status);
  purple_notify_user_info_add_pair (info, "Last seen", status);
  g_free (status);

  if (str_not_empty (U->phone)) {
    char *phone = g_strdup_printf("+%s", U->phone);
    purple_notify_user_info_add_pair (info, "Phone", phone);
    g_free (phone);
  }
 
  return info;
}

PurpleNotifyUserInfo *p2tgl_notify_encrypted_chat_info_new (struct tgl_state *TLS,
                                                            struct tgl_secret_chat *secret, struct tgl_user *U) {
  
  PurpleNotifyUserInfo *info = p2tgl_notify_user_info_new (U);
  
  if (secret->state == sc_waiting) {
    purple_notify_user_info_add_pair (info, "", "Waiting for user to get online ...");
    return info;
  }
  
  const char *ttl_key = "Self destructiom timer";
  if (secret->ttl) {
    char *ttl = g_strdup_printf ("%d", secret->ttl);
    purple_notify_user_info_add_pair (info, ttl_key, ttl);
    g_free (ttl);
  } else {
    purple_notify_user_info_add_pair (info, ttl_key, "Off");
  }
  
  if (secret->first_key_sha[0]) {
    int sha1key_store_id = tgp_visualize_key (TLS, secret->first_key_sha);
    if (sha1key_store_id != -1) {
      char *ident_icon = format_img_full (sha1key_store_id);
      purple_notify_user_info_add_pair (info, "Secret key", ident_icon);
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

void p2tgl_buddy_icons_set_for_user (PurpleAccount *pa, tgl_peer_id_t *id, const char* filename) {
  char *who = g_strdup_printf("%d", tgl_get_peer_id(*id));

  gchar *data = NULL;
  size_t len;
  GError *err = NULL;
  g_file_get_contents (filename, &data, &len, &err);
  
  purple_buddy_icons_set_for_user (pa, who, data, len, NULL);
  
  g_free (who);
}

