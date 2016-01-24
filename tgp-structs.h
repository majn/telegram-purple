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
 
 Copyright Matthias Jentsch 2014-2015
 */

#ifndef __telegram_adium__tgp_data__
#define __telegram_adium__tgp_data__

#include "purple.h"

#include <tgl.h>
#include <glib.h>

typedef struct {
  struct tgl_state *TLS;
  char *hash;
  PurpleAccount *pa;
  PurpleConnection *gc;
  int updated;
  GQueue *new_messages;
  GQueue *out_messages;
  GHashTable *pending_reads;
  GList *used_images;
  guint write_timer;
  guint login_timer;
  guint out_timer;
  struct request_values_data *request_code_data;
  int password_retries;
  int login_retries;
  PurpleRoomlist *roomlist;
  GHashTable *pending_chat_info;
  GHashTable *id_to_purple_name;
  GHashTable *purple_name_to_id;
  GList *pending_joins;
  int dialogues_ready;
} connection_data;

struct tgp_xfer_send_data {
  int timer;
  int loading;
  PurpleXfer *xfer;
  connection_data *conn;
  struct tgl_message *msg;
};

struct tgp_msg_loading {
  int pending;
  struct tgl_message *msg;
  void *data;
  int error;
  char *error_msg;
};

struct tgp_msg_sending {
  struct tgl_state *TLS;
  tgl_peer_id_t to;
  gchar *msg;
};

void pending_reads_send_all (struct tgl_state *TLS);
void pending_reads_add (struct tgl_state *TLS, struct tgl_message *M);
void pending_reads_send_user (struct tgl_state *TLS, tgl_peer_id_t id);

void used_images_add (connection_data *data, gint imgid);
void *connection_data_free (connection_data *conn);
connection_data *connection_data_init (struct tgl_state *TLS, PurpleConnection *gc, PurpleAccount *pa);
struct tgp_msg_loading *tgp_msg_loading_init (struct tgl_message *M);
struct tgp_msg_sending *tgp_msg_sending_init (struct tgl_state *TLS, char *M, tgl_peer_id_t to);
void tgp_msg_loading_free (gpointer data);
void tgp_msg_sending_free (gpointer data);
#endif

