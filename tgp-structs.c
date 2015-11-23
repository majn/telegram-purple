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

#include "telegram-base.h"

static void pending_reads_free_cb (gpointer data) {
  free (data);
}

static gint pending_reads_compare (gconstpointer a, gconstpointer b) {
  return !memcmp ((tgl_peer_id_t *)a, (tgl_peer_id_t *)b, sizeof(tgl_peer_id_t));
}

void pending_reads_send_all (struct tgl_state *TLS) {
  debug ("send all pending ack");
  if (! p2tgl_status_is_present (purple_account_get_active_status (tls_get_pa (TLS))) ||
      ! p2tgl_send_notifications (tls_get_pa (TLS))) {
    return;
  }
  
  tgl_peer_id_t *pending;
  GQueue *queue = tls_get_data (TLS)->pending_reads;
  while ((pending = (tgl_peer_id_t*) g_queue_pop_head(queue))) {
    tgl_do_mark_read (TLS, *pending, tgp_notify_on_error_gw, NULL);
    debug ("tgl_do_mark_read (%d)", tgl_get_peer_id (*pending));
    free (pending);
  }
}

void pending_reads_add (GQueue *queue, tgl_peer_id_t id) {
  tgl_peer_id_t *copy = malloc (sizeof(tgl_peer_id_t));
  *copy = id;
  if (! g_queue_find_custom (queue, copy, pending_reads_compare)) {
    g_queue_push_tail (queue, copy);
  }
}

static void used_image_free (gpointer data) {
  int id = GPOINTER_TO_INT(data);
  purple_imgstore_unref_by_id (id);
}

void used_images_add (connection_data *data, gint imgid) {
  data->used_images = g_list_append (data->used_images, GINT_TO_POINTER(imgid));
}

void tgp_msg_loading_free (gpointer data) {
  struct tgp_msg_loading *C = data;
  free (C);
}

struct tgp_msg_loading *tgp_msg_loading_init (struct tgl_message *M) {
  struct tgp_msg_loading *C = malloc (sizeof (struct tgp_msg_loading));
  C->pending = 0;
  C->msg = M;
  C->data = NULL;
  return C;
}

struct tgp_msg_sending *tgp_msg_sending_init (struct tgl_state *TLS, gchar *M, tgl_peer_id_t to) {
  struct tgp_msg_sending *C = malloc (sizeof (struct tgp_msg_sending));
  C->TLS = TLS;
  C->msg = M;
  C->to = to;
  return C;
}

void tgp_msg_sending_free (gpointer data) {
  struct tgp_msg_sending *C = data;
  g_free (C->msg);
  free (C);
}

connection_data *connection_data_init (struct tgl_state *TLS, PurpleConnection *gc, PurpleAccount *pa) {
  connection_data *conn = g_new0 (connection_data, 1);
  conn->TLS = TLS;
  conn->gc = gc;
  conn->pa = pa;
  conn->new_messages = g_queue_new ();
  conn->out_messages = g_queue_new ();
  conn->pending_reads = g_queue_new ();
  conn->pending_chat_info = g_hash_table_new (g_direct_hash, g_direct_equal);
  conn->id_to_purple_name = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);
  return conn;
}

void *connection_data_free (connection_data *conn) {
  if (conn->write_timer) { purple_timeout_remove (conn->write_timer); }
  if (conn->login_timer) { purple_timeout_remove (conn->login_timer); }
  if (conn->out_timer) { purple_timeout_remove (conn->out_timer); }
  
  tgp_g_queue_free_full (conn->pending_reads, pending_reads_free_cb);
  tgp_g_queue_free_full (conn->new_messages, tgp_msg_loading_free);
  tgp_g_queue_free_full (conn->out_messages, tgp_msg_sending_free);
  tgp_g_list_free_full (conn->used_images, used_image_free);
  g_hash_table_destroy (conn->pending_chat_info);
  g_hash_table_destroy (conn->id_to_purple_name);
  tgprpl_xfer_free_all (conn);
  tgl_free_all (conn->TLS);
  g_free (conn->TLS->base_path);
  free (conn);
  return NULL;
}

get_user_info_data* get_user_info_data_new (int show_info, tgl_peer_id_t peer) {
  get_user_info_data *info_data = malloc (sizeof(get_user_info_data));
  info_data->show_info = show_info;
  info_data->peer = peer;
  return info_data;
}

