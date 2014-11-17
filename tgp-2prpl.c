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
#include "tgp-2prpl.h"

#include <server.h>
#include <tgl.h>
#include <msglog.h>
#include <assert.h>

static void sanitize_alias(char *buffer) {
  size_t len = strlen(buffer);
  gchar *curr;
  while ((curr = g_utf8_strchr(buffer, len, '\n'))) {
    *curr = 0x20;
  }
}

static int user_get_alias (tgl_peer_t *user, char *buffer, int maxlen) {
  char* last_name  = (user->user.last_name  && strlen(user->user.last_name)) ? user->user.last_name  : "";
  char* first_name = (user->user.first_name && strlen(user->user.first_name)) ? user->user.first_name : "";
  last_name = strdup (last_name);
  first_name = strdup (first_name);
  sanitize_alias (last_name);
  sanitize_alias (first_name);
  int res;
  if (strlen (first_name) && strlen(last_name)) {
    res = snprintf (buffer, maxlen, "%s %s", first_name, last_name);
  } else if (strlen (first_name)) {
    res = snprintf (buffer, maxlen, "%s", first_name);
  } else if (strlen (last_name)) {
    res = snprintf (buffer, maxlen, "%s", last_name);
  } else {
    res = snprintf (buffer, maxlen, "%d", tgl_get_peer_id (user->id));
  }
  free (last_name);
  free (first_name);
  return res;
}

PurpleAccount *tg_get_acc (struct tgl_state *TLS) {
  return (PurpleAccount *) ((telegram_conn *)TLS->ev_base)->pa;
}

PurpleConnection *tg_get_conn (struct tgl_state *TLS) {
  return (PurpleConnection *) ((telegram_conn *)TLS->ev_base)->gc;
}

static char *peer_strdup_id(tgl_peer_id_t user) {
  return g_strdup_printf("%d", tgl_get_peer_id(user));
}

char *p2tgl_strdup_alias(tgl_peer_t *user) {
  char *alias = malloc(64);

  // snprintf returs number of bytes it wanted to write
  // negative return points on write error
  // should never happen
  int r = user_get_alias(user, alias, 64);
  if (r >= 64) {
    warning ("user name too long");
    alias[63] = 0;
  }
  assert (r >= 0);

  gchar *g_alias = g_strdup(alias);
  free (alias);
  return g_alias;
}

static PurpleChat *blist_find_chat_by_hasht_cond(PurpleConnection *gc,
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
  return !strcmp(g_hash_table_lookup(hasht, "id"), *((char **)data));
}


PurpleConversation *p2tgl_got_joined_chat (struct tgl_state *TLS, struct tgl_chat *chat) {
  telegram_conn *conn = TLS->ev_base;
  gchar *alias = p2tgl_strdup_alias((tgl_peer_t *)chat);
  
  PurpleConversation *conv = serv_got_joined_chat(conn->gc, tgl_get_peer_id(chat->id), alias);
  
  g_free(alias);
  return conv;
}

void p2tgl_got_chat_left (struct tgl_state *TLS, tgl_peer_id_t chat) {
  serv_got_chat_left(tg_get_conn(TLS), tgl_get_peer_id(chat));
}

void p2tgl_got_chat_in (struct tgl_state *TLS, tgl_peer_id_t chat, tgl_peer_id_t who, const char *message, int flags, time_t when) {
  char *name = peer_strdup_id(who);
  
  serv_got_chat_in(tg_get_conn(TLS), tgl_get_peer_id (chat), name, flags, message, when);
  
  g_free (name);
}

void p2tgl_got_alias (struct tgl_state *TLS, tgl_peer_id_t who, const char *alias) {
  char *name = peer_strdup_id(who);
  
  serv_got_alias(tg_get_conn(TLS), name, alias);
  
  g_free (name);
}

void p2tgl_got_im (struct tgl_state *TLS, tgl_peer_id_t who, const char *msg, int flags, time_t when) {
  char *name = peer_strdup_id(who);
  
  serv_got_im(tg_get_conn(TLS), name, msg,flags, when);
  
  g_free (name);
}

void p2tgl_got_typing (struct tgl_state *TLS, tgl_peer_id_t user, int timeout) {
  char *who = g_strdup_printf("%d", tgl_get_peer_id(user));
  
  serv_got_typing(tg_get_conn(TLS), who, timeout, PURPLE_TYPING);
  
  g_free(who);
}


PurpleBuddy *p2tgl_buddy_find (struct tgl_state *TLS, tgl_peer_id_t user) {
  gchar *name = peer_strdup_id(user);
  
  PurpleBuddy *b = purple_find_buddy (tg_get_acc(TLS), name);
  
  g_free (name);
  return b;
}

PurpleBuddy *p2tgl_buddy_new  (struct tgl_state *TLS, tgl_peer_t *user) {
  char *alias = p2tgl_strdup_alias (user);
  char *name  = peer_strdup_id (user->id);
  
  PurpleBuddy *b = purple_buddy_new(tg_get_acc(TLS), name, alias);
  
  g_free (alias);
  g_free (name);
  return b;
}

PurpleBuddy *p2tgl_buddy_update (struct tgl_state *TLS, tgl_peer_t *user, unsigned flags) {
  PurpleBuddy *b = p2tgl_buddy_find (TLS, user->id);
  if (!b) {
    b = p2tgl_buddy_new (TLS, user);
  }
  if (flags & TGL_UPDATE_NAME) {
    debug ("Update username for id%d (name %s %s)\n", tgl_get_peer_id (user->id), user->user.first_name, user->user.last_name);
    char *alias = p2tgl_strdup_alias (user);
    purple_blist_alias_buddy(b, alias);
    g_free (alias);
  }
  return b;
}

void p2tgl_prpl_got_user_status (struct tgl_state *TLS, tgl_peer_id_t user, struct tgl_user_status *status) {
  
  if (status->online == 1) {
    char *name = peer_strdup_id (user);
    purple_prpl_got_user_status (tg_get_acc(TLS), name, "available", NULL);
    g_free (name);
  } else {
    char *name = peer_strdup_id (user);
    char *when;
    switch (status->online) {
    case -1:
      when = g_strdup_printf("%d", status->when);
      break;
    case -2:
      when = g_strdup_printf("recently");
      break;
    case -3:
      when = g_strdup_printf("last week");
      break;
    case -4:
      when = g_strdup_printf("last month");
      break;
    default:
      when = g_strdup ("unknown");
      break;
    }
  
    purple_prpl_got_user_status (tg_get_acc(TLS), name, "mobile", "last online", when, NULL);
  
    g_free(name);
    g_free(when);
  }
}

PurpleChat *p2tgl_blist_find_chat(struct tgl_state *TLS, tgl_peer_id_t chat) {
  char *name = peer_strdup_id(chat);
  
  PurpleChat *c = purple_blist_find_chat(tg_get_acc(TLS), name);
  
  g_free (name);
  return c;
}


PurpleChat *p2tgl_chat_new (struct tgl_state *TLS, struct tgl_chat *chat) {
  gchar *admin = g_strdup_printf("%d", chat->admin_id);
  gchar *title = g_strdup(chat->title);
  gchar *name  = peer_strdup_id (chat->id);
  
  GHashTable *ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
  g_hash_table_insert(ht, g_strdup("subject"), title);
  g_hash_table_insert(ht, g_strdup("id"), name);
  g_hash_table_insert(ht, g_strdup("owner"), admin);
  
  return purple_chat_new(tg_get_acc(TLS), chat->title, ht);
}

PurpleChat *p2tgl_chat_find (struct tgl_state *TLS, tgl_peer_id_t id) {
  char *name = peer_strdup_id(id);
  PurpleChat *c = blist_find_chat_by_hasht_cond(tg_get_conn(TLS), hasht_cmp_id, &name);
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
  char *name = peer_strdup_id(user);
  void *handle = 0;
  
  handle = purple_notify_userinfo(tg_get_conn(TLS), name, user_info, cb, user_data);
  
  g_free(name);
  return handle;
}
