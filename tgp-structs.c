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

#include "tgp-structs.h"
#include "purple.h"
#include "msglog.h"

#include <glib.h>
#include <tgl.h>

/*
 tgp-structs.c: Structs that are only used internally by the protocol plugin
 */

void pending_reads_free_cb (gpointer data)
{
  free (data);
}

static void pending_reads_cb (struct tgl_state *TLS, void *extra, int success)
{
  debug ("ack state: %d", success);
}

static gint pending_reads_compare (gconstpointer a, gconstpointer b)
{
  return !memcmp ((tgl_peer_id_t *)a, (tgl_peer_id_t *)b, sizeof(tgl_peer_id_t));
}

void pending_reads_send_all (GQueue *queue, struct tgl_state *TLS)
{
  debug ("send all pending ack");
  
  tgl_peer_id_t *pending;
  
  while ((pending = (tgl_peer_id_t*) g_queue_pop_head(queue))) {
    tgl_do_mark_read (TLS, *pending, pending_reads_cb, queue);
    debug ("tgl_do_mark_read (%d)", pending->id);
    free (pending);
  }
}

void pending_reads_add (GQueue *queue, tgl_peer_id_t id)
{
  tgl_peer_id_t *copy = malloc (sizeof(tgl_peer_id_t));
  *copy = id;
  if (! g_queue_find_custom (queue, copy, pending_reads_compare)) {
    g_queue_push_tail (queue, copy);
  }
}

struct message_text *message_text_init (struct tgl_message *M, gchar *text)
{
  struct message_text *mt = malloc (sizeof (struct message_text));
  mt->M = M;
  mt->text = text ? g_strdup (text) : text;
  return mt;
}

void message_text_free (gpointer data)
{
  struct message_text *mt = (struct message_text*)data;
  if (mt->text) {
    g_free (mt->text);
  }
  free (mt);
}

static void used_image_free (gpointer data)
{
  int id = GPOINTER_TO_INT(data);
  purple_imgstore_unref_by_id (id);
  debug ("used_image: unref %d", id);
}

void used_images_add (connection_data *data, gint imgid)
{
  data->used_images = g_list_append (data->used_images, GINT_TO_POINTER(imgid));
  debug ("used_image: add %d", imgid);
}

connection_data *connection_data_init (struct tgl_state *TLS, PurpleConnection *gc, PurpleAccount *pa)
{
  connection_data *conn = g_new0 (connection_data, 1);
  conn->TLS = TLS;
  conn->gc = gc;
  conn->pa = pa;
  conn->new_messages = g_queue_new ();
  conn->pending_reads = g_queue_new ();
  conn->joining_chats = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  return conn;
}

void *connection_data_free (connection_data *conn)
{
  purple_timeout_remove(conn->timer);
  g_queue_free_full (conn->pending_reads, pending_reads_free_cb);
  g_queue_free_full (conn->new_messages, message_text_free);
  g_hash_table_destroy (conn->joining_chats);
  g_list_free_full (conn->used_images, used_image_free);
  tgl_free_all (conn->TLS);
  free (conn);
  return NULL;
}

