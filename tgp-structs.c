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

static void tgl_do_mark_read_gw (gpointer key, gpointer value, gpointer data) {
  tgl_peer_id_t to = * (tgl_peer_id_t *)value;
  info ("tgl_do_mark_read (%d)", tgl_get_peer_id (to));
  tgl_do_mark_read ((struct tgl_state *) data, to, tgp_notify_on_error_gw, NULL);
}

void pending_reads_send_all (struct tgl_state *TLS) {
  if (! purple_account_get_bool (tls_get_pa (TLS), TGP_KEY_SEND_READ_NOTIFICATIONS,
      TGP_DEFAULT_SEND_READ_NOTIFICATIONS)) {
    debug ("automatic read recipes disabled, not sending recipes");
    return;
  }
  if (! p2tgl_status_is_present (purple_account_get_active_status (tls_get_pa (TLS)))) {
    debug ("user is not present, not sending recipes");
    return;
  }
  debug ("sending all pending recipes");
  g_hash_table_foreach (tls_get_data (TLS)->pending_reads, tgl_do_mark_read_gw, TLS);
  g_hash_table_remove_all (tls_get_data (TLS)->pending_reads);
}

void pending_reads_send_user (struct tgl_state *TLS, tgl_peer_id_t id) {
  if (g_hash_table_remove (tls_get_data (TLS)->pending_reads, GINT_TO_POINTER (tgl_get_peer_id (id)))) {
    info ("tgl_do_mark_read (%d)", tgl_get_peer_id (id));
    tgl_do_mark_read (TLS, id, tgp_notify_on_error_gw, NULL);
  }
}

void pending_reads_add (struct tgl_state *TLS, struct tgl_message *M) {
  tgl_peer_id_t *copy = g_new (tgl_peer_id_t, 1);
  if (tgl_get_peer_type (M->to_id) == TGL_PEER_USER) {
    *copy = M->from_id;
  } else {
    *copy = M->to_id;
  }
  g_hash_table_replace (tls_get_data (TLS)->pending_reads, GINT_TO_POINTER (tgl_get_peer_id (*copy)), copy);
}

static void used_image_free (gpointer data) {
  purple_imgstore_unref_by_id (GPOINTER_TO_INT(data));
}

void used_images_add (connection_data *data, gint imgid) {
  data->used_images = g_list_append (data->used_images, GINT_TO_POINTER(imgid));
}

void tgp_msg_loading_free (gpointer data) {
  struct tgp_msg_loading *C = data;
  free (C);
}

struct tgp_msg_loading *tgp_msg_loading_init (struct tgl_message *M) {
  struct tgp_msg_loading *C = talloc0 (sizeof (struct tgp_msg_loading));
  C->pending = 0;
  C->msg = M;
  C->data = NULL;
  return C;
}

struct tgp_msg_sending *tgp_msg_sending_init (struct tgl_state *TLS, char *M, tgl_peer_id_t to) {
  struct tgp_msg_sending *C = malloc (sizeof (struct tgp_msg_sending));
  C->TLS = TLS;
  C->msg = M;
  C->to = to;
  return C;
}

void tgp_msg_sending_free (gpointer data) {
  struct tgp_msg_sending *C = data;
  if (C->msg) {
    g_free (C->msg);
  }
  free (C);
}

connection_data *connection_data_init (struct tgl_state *TLS, PurpleConnection *gc, PurpleAccount *pa) {
  connection_data *conn = g_new0 (connection_data, 1);
  conn->TLS = TLS;
  conn->gc = gc;
  conn->pa = pa;
  conn->new_messages = g_queue_new ();
  conn->out_messages = g_queue_new ();
  conn->pending_reads = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);
  conn->pending_chat_info = g_hash_table_new (g_direct_hash, g_direct_equal);
  conn->pending_channels = g_hash_table_new (g_direct_hash, g_direct_equal);
  conn->id_to_purple_name = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);
  conn->purple_name_to_id = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
  conn->channel_members = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, (void (*) (gpointer)) g_list_free);
  
  return conn;
}

void *connection_data_free (connection_data *conn) {
  if (conn->write_timer) { purple_timeout_remove (conn->write_timer); }
  if (conn->login_timer) { purple_timeout_remove (conn->login_timer); }
  if (conn->out_timer) { purple_timeout_remove (conn->out_timer); }

  tgp_g_queue_free_full (conn->new_messages, tgp_msg_loading_free);
  tgp_g_queue_free_full (conn->out_messages, tgp_msg_sending_free);
  tgp_g_list_free_full (conn->used_images, used_image_free);
  tgp_g_list_free_full (conn->pending_joins, g_free);
  g_hash_table_destroy (conn->pending_reads);
  g_hash_table_destroy (conn->pending_chat_info);
  g_hash_table_destroy (conn->pending_channels);
  g_hash_table_destroy (conn->id_to_purple_name);
  g_hash_table_destroy (conn->purple_name_to_id);
  g_hash_table_destroy (conn->channel_members);
  g_free (conn->download_dir);
  g_free (conn->download_uri);

  tgprpl_xfer_free_all (conn);
  g_free (conn->TLS->base_path);
  tgl_free_all (conn->TLS);
 
  free (conn);
  return NULL;
}
