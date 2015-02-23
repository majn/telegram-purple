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

#include "tgp-utils.h"
#include "tgp-ft.h"
#include "tgp-structs.h"
#include "msglog.h"

#include <purple.h>
#include <glib.h>
#include <glib/gstdio.h>

static void tgprpl_xfer_recv_on_finished (struct tgl_state *TLS, void *_data, int success, char *filename) {
  debug ("tgprpl_xfer_recv_on_finished()");
  struct tgp_xfer_send_data *data = _data;

  if (success) {
    if (!data->done) {
      purple_xfer_set_completed (data->xfer, TRUE);
    }
    
    g_unlink (purple_xfer_get_local_filename (data->xfer));
    g_rename (filename, purple_xfer_get_local_filename(data->xfer));

  } else {
    failure ("ERROR xfer failed");
  }
  
  if (data->timer) { purple_timeout_remove(data->timer); }
  data->timer = 0;
}

static void tgprpl_xfer_on_finished (struct tgl_state *TLS, void *_data, int success, struct tgl_message *M) {
  debug ("tgprpl_xfer_on_finished()");
  struct tgp_xfer_send_data *data = _data;
  
  if (success) {
    if (!data->done) {
      purple_xfer_set_completed (data->xfer, TRUE);
    }
  } else {
    failure ("ERROR xfer failed");
  }
  
  if (data->timer) { purple_timeout_remove(data->timer); }
  data->timer = 0;
}

static void tgprpl_xfer_canceled (PurpleXfer *X) {
  struct tgp_xfer_send_data *data = X->data;
  data->done = TRUE;
  
  if (data->timer) { purple_timeout_remove (data->timer); }
  data->timer = 0;
}

static gboolean tgprpl_xfer_upload_progress (gpointer _data) {
  PurpleXfer *X = _data;
  struct tgp_xfer_send_data *data = X->data;
  connection_data *conn = data->conn;
  
  PurpleXferType type = purple_xfer_get_type(X);
  switch (type) {
    case PURPLE_XFER_SEND:
      purple_xfer_set_size (X, conn->TLS->cur_uploading_bytes);
      purple_xfer_set_bytes_sent (X, conn->TLS->cur_uploaded_bytes);
      purple_xfer_update_progress (X);
      
      if (conn->TLS->cur_uploaded_bytes == conn->TLS->cur_uploading_bytes) {
        data->timer = 0;
        return FALSE;
      }
      break;
      
    case PURPLE_XFER_RECEIVE:
      debug ("PURPLE_XFER_RECEIVER progress ...");
      purple_xfer_set_size (X, conn->TLS->cur_downloading_bytes);
      purple_xfer_set_bytes_sent (X, conn->TLS->cur_downloaded_bytes);
      purple_xfer_update_progress (X);
      
      debug ("PURPLE_XFER_RECEIVER progress %d", conn->TLS->cur_downloaded_bytes);
      if (conn->TLS->cur_downloading_bytes == conn->TLS->cur_downloaded_bytes) {
        data->timer = 0;
        return FALSE;
      }
      break;
      
    default:
    case PURPLE_XFER_UNKNOWN:
      failure ("ERROR: tgprpl_xfer_upload_progress xfer type PURPLE_XFER_UNKNOWN.");
      return FALSE;
      break;
  }
  return TRUE;
}

static void tgprpl_xfer_recv_init (PurpleXfer *X) {
  debug ("tgprpl_xfer_recv_init");
  struct tgp_xfer_send_data *data = X->data;
  
  purple_xfer_start (X, -1, NULL, 0);
  
  const char *who = purple_xfer_get_remote_user (X);
  debug ("who: %s", who);
  tgl_peer_t *P = find_peer_by_name (data->conn->TLS, who);
  
  if (P) {
    tgl_do_load_document (data->conn->TLS, data->document, tgprpl_xfer_recv_on_finished, data);
  } else {
    warning ("User not found, not downloading...");
  }
  
  data->timer = purple_timeout_add (100, tgprpl_xfer_upload_progress, X);
}

static void tgprpl_xfer_send_init (PurpleXfer *X) {
  struct tgp_xfer_send_data *data = X->data;
  
  purple_xfer_start (X, -1, NULL, 0);
  
  const char *file = purple_xfer_get_filename (X);
  const char *localfile = purple_xfer_get_local_filename (X);
  const char *who = purple_xfer_get_remote_user (X);
  debug ("xfer_on_init (file=%s, local=%s, who=%s)", file, localfile, who);
  
  tgl_peer_t *P = find_peer_by_name (data->conn->TLS, who);
  if (P) {
    tgl_do_send_document (data->conn->TLS, -2, P->id, (char*)localfile, tgprpl_xfer_on_finished, data);
  }
  
  data->timer = purple_timeout_add (100, tgprpl_xfer_upload_progress, X);
}

static void tgprpl_xfer_init_data (PurpleXfer *X, connection_data *conn, struct tgl_document *D) {
  if (!X->data) {
    // TODO: free this somewhere
    struct tgp_xfer_send_data *data = g_malloc0 (sizeof (struct tgp_xfer_send_data));
    data->xfer = X;
    data->conn = conn;
    data->document = D;
    X->data = data;
    conn->transfers = g_list_append (conn->transfers, data);
  }
}

PurpleXfer *tgprpl_new_xfer (PurpleConnection * gc, const char *who) {
  debug ("tgprpl_new_xfer()");
  
  connection_data *conn = purple_connection_get_protocol_data (gc);
  
  PurpleXfer *X = purple_xfer_new (conn->pa, PURPLE_XFER_SEND, who);
  
  return (PurpleXfer *)X;
}

void tgprpl_recv_file (PurpleConnection * gc, const char *who, struct tgl_document *D) {
  debug ("tgprpl_recv_file()");
  connection_data *conn = purple_connection_get_protocol_data (gc);
  
  PurpleXfer *X = purple_xfer_new (conn->pa, PURPLE_XFER_RECEIVE, who);

  purple_xfer_set_filename (X, D->caption);
  purple_xfer_set_init_fnc (X, tgprpl_xfer_recv_init);
  purple_xfer_set_cancel_recv_fnc (X, tgprpl_xfer_canceled);
  
  tgprpl_xfer_init_data (X, purple_connection_get_protocol_data (gc), D);
  
  purple_xfer_request (X);
}

void tgprpl_send_file (PurpleConnection * gc, const char *who, const char *file) {
  debug ("tgprpl_send_file()");
  
  PurpleXfer *X = tgprpl_new_xfer (gc, who);
  purple_xfer_set_init_fnc (X, tgprpl_xfer_send_init);
  purple_xfer_set_cancel_send_fnc (X, tgprpl_xfer_canceled);
  
  if (file) {
    purple_xfer_request_accepted (X, file);
    debug ("starting xfer...");
  } else {
    purple_xfer_request (X);
  }
  
  tgprpl_xfer_init_data (X, purple_connection_get_protocol_data (gc), NULL);
}

