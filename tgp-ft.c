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

#include <glib/gstdio.h>

#include "telegram-purple.h"

static void tgprpl_xfer_free_data (struct tgp_xfer_send_data *data);

static char *tgp_strdup_determine_filename (const char *mime, const char *caption,
                                            int flags, long long hash) {
  if (caption) {
    return g_strdup (caption);
  }
  const char *type = NULL;
  if (mime) {
    if (flags & TGLDF_VIDEO) {
      // video message
      type = "mp4";
    } else if (flags & TGLDF_AUDIO) {
      // audio message
      type = "ogg";
    } else {
      // document message
      type = tgp_mime_to_filetype (mime);
    }
  }
  if (! str_not_empty(type)) {
    if (flags & TGLDF_IMAGE) {
      type = "png";
    } else if (flags & TGLDF_AUDIO) {
      type = "ogg";
    } else if (flags & TGLDF_VIDEO) {
      type = "mp4";
    } else if (flags & TGLDF_STICKER) {
      type = "webp";
    } else {
      type = "bin";
    }
  }
  return g_strdup_printf ("%" G_GINT64_MODIFIER "d.%s", (gint64) ABS(hash), type);
}

static void tgprpl_xfer_recv_on_finished (struct tgl_state *TLS, void *_data, int success, const char *filename) {
  debug ("tgprpl_xfer_recv_on_finished()");
  struct tgp_xfer_send_data *data = _data;
  char *selected = g_strdup(purple_xfer_get_local_filename (data->xfer));

  if (success) {
    if (!data->done) {
      debug ("purple_xfer_set_completed");
      purple_xfer_set_bytes_sent (data->xfer, purple_xfer_get_size (data->xfer));
      purple_xfer_set_completed (data->xfer, TRUE);
      purple_xfer_end (data->xfer);
    }
  } else {
    tgp_notify_on_error_gw (TLS, NULL, success);
    failure ("ERROR xfer failed");
  }

  data->xfer->data = NULL;
  tgprpl_xfer_free_data (data);

  debug ("moving transferred file from tgl directory %s to selected target %s", selected, filename);
  g_unlink (selected);
  g_rename (filename, selected);
  g_free (selected);
}

static void tgprpl_xfer_on_finished (struct tgl_state *TLS, void *_data, int success, struct tgl_message *M) {
  debug ("tgprpl_xfer_on_finished()");
  struct tgp_xfer_send_data *data = _data;
  
  if (success) {
    if (!data->done) {
      debug ("purple_xfer_set_completed");
      purple_xfer_set_bytes_sent (data->xfer, purple_xfer_get_size (data->xfer));
      purple_xfer_set_completed (data->xfer, TRUE);
      purple_xfer_end(data->xfer);
    }
    write_secret_chat_file (TLS);
  } else {
    tgp_notify_on_error_gw (TLS, NULL, success);
    failure ("ERROR xfer failed");
  }
  
  data->xfer->data = NULL;
  tgprpl_xfer_free_data (data);
}

static void tgprpl_xfer_canceled (PurpleXfer *X) {
  struct tgp_xfer_send_data *data = X->data;
  tgprpl_xfer_free_data (data);
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
      
      debug ("PURPLE_XFER_SEND progress %d / %d", conn->TLS->cur_uploaded_bytes, conn->TLS->cur_uploading_bytes);
      if (conn->TLS->cur_uploaded_bytes == conn->TLS->cur_uploading_bytes) {
        data->timer = 0;
        return FALSE;
      }
      break;
      
    case PURPLE_XFER_RECEIVE:
      purple_xfer_set_size (X, conn->TLS->cur_downloading_bytes);
      purple_xfer_set_bytes_sent (X, conn->TLS->cur_downloaded_bytes);
      purple_xfer_update_progress (X);
      
      debug ("PURPLE_XFER_RECEIVE progress %d / %d", conn->TLS->cur_downloaded_bytes, conn->TLS->cur_downloading_bytes);
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
  struct tgl_state *TLS = data->conn->TLS;
  struct tgl_message *M = data->msg;
  struct tgl_document *D = M->media.document;
  tgl_peer_t *P = NULL;
  
  purple_xfer_start (X, -1, NULL, 0);
  const char *who = purple_xfer_get_remote_user (X);
  P = tgp_blist_peer_find (TLS, who);
  if (P) {
    switch (M->media.type) {
      case tgl_message_media_document:
        tgl_do_load_document (TLS, D, tgprpl_xfer_recv_on_finished, data);
        break;
        
      case tgl_message_media_document_encr:
        tgl_do_load_encr_document (TLS, M->media.encr_document, tgprpl_xfer_recv_on_finished, data);
        break;
      
      case tgl_message_media_audio:
        tgl_do_load_audio (TLS, D, tgprpl_xfer_recv_on_finished, data);
        break;
        
      case tgl_message_media_video:
        tgl_do_load_video (TLS, D, tgprpl_xfer_recv_on_finished, data);
        break;

      default:
        failure ("Unknown message media type: %d, XFER not possible.", M->media.type);
        break;
    }
  } else {
    warning ("User not found, not downloading...");
  }
  data->timer = purple_timeout_add (100, tgprpl_xfer_upload_progress, X);
}

static void tgprpl_xfer_send_init (PurpleXfer *X) {
  struct tgp_xfer_send_data *data;
  const char *file, *localfile, *who;
  tgl_peer_t *P;

  data = X->data;
  purple_xfer_start (X, -1, NULL, 0);
  
  file = purple_xfer_get_filename (X);
  localfile = purple_xfer_get_local_filename (X);
  who = purple_xfer_get_remote_user (X);
  debug ("xfer_on_init (file=%s, local=%s, who=%s)", file, localfile, who);

  P = tgp_blist_peer_find (data->conn->TLS, who);
  g_return_if_fail (P);

  if (tgl_get_peer_type (P->id) == TGL_PEER_ENCR_CHAT) {
    purple_xfer_error (PURPLE_XFER_SEND, data->conn->pa, who,
        _("Sorry, sending documents to encrypted chats not yet supported."));
    return;
  }

  unsigned long long int flags = TGL_SEND_MSG_FLAG_DOCUMENT_AUTO;
  if (tgl_get_peer_type (P->id) == TGL_PEER_CHANNEL) {
    flags |= TGLMF_POST_AS_CHANNEL;
  }

  tgl_do_send_document (data->conn->TLS, P->id, (char*) localfile, NULL, 0, flags, tgprpl_xfer_on_finished, data);
  data->timer = purple_timeout_add (100, tgprpl_xfer_upload_progress, X);
}

static void tgprpl_xfer_init_data (PurpleXfer *X, connection_data *conn, struct tgl_message *msg) {
  if (!X->data) {
    struct tgp_xfer_send_data *data = g_malloc0 (sizeof (struct tgp_xfer_send_data));
    data->xfer = X;
    data->conn = conn;
    data->msg = msg;
    X->data = data;
  }
}

static void tgprpl_xfer_free_data (struct tgp_xfer_send_data *data) {
  if (data->timer) {
    purple_input_remove(data->timer);
  }
  data->timer = 0;
  g_free (data);
}

void tgprpl_xfer_free_all (connection_data *conn) {
  GList *xfers = purple_xfers_get_all();
  while (xfers) {
    PurpleXfer *xfer = xfers->data;
    struct tgp_xfer_send_data *data = xfer->data;
    
    if (data) {
      purple_xfer_cancel_local (xfer);
    }
    xfers = g_list_next(xfers);
  }
}

PurpleXfer *tgprpl_new_xfer (PurpleConnection *gc, const char *who) {
  debug ("tgprpl_new_xfer()");
  PurpleXfer *X = purple_xfer_new (purple_connection_get_account (gc), PURPLE_XFER_SEND, who);
  if (X) {
    purple_xfer_set_init_fnc (X, tgprpl_xfer_send_init);
    purple_xfer_set_cancel_send_fnc (X, tgprpl_xfer_canceled);
    tgprpl_xfer_init_data (X, purple_connection_get_protocol_data (gc), NULL);
  }
  return X;
}

static PurpleXfer *tgprpl_new_xfer_recv (PurpleConnection *gc, const char *who) {
  PurpleXfer *X = purple_xfer_new (purple_connection_get_account (gc), PURPLE_XFER_RECEIVE, who);
  purple_xfer_set_init_fnc (X, tgprpl_xfer_recv_init);
  purple_xfer_set_cancel_recv_fnc (X, tgprpl_xfer_canceled);
  return X;
}

void tgprpl_recv_file (PurpleConnection *gc, const char *who, struct tgl_message *M) {
  debug ("tgprpl_recv_file()");
  g_return_if_fail (who);
  
  PurpleXfer *X = tgprpl_new_xfer_recv (gc, who);
  const char *mime_type, *caption;
  long long access_hash;
  int flags, size;
  
  if (M->media.type == tgl_message_media_document_encr) {
    mime_type = M->media.encr_document->mime_type;
    caption = M->media.encr_document->caption;
    access_hash = M->media.encr_document->access_hash;
    flags = M->media.encr_document->flags;
    size = M->media.encr_document->size;
  } else {
    mime_type = M->media.document->mime_type;
    caption = M->media.document->caption;
    access_hash = M->media.document->access_hash;
    flags = M->media.document->flags;
    size = M->media.document->size;
  }

  char *filename = tgp_strdup_determine_filename (mime_type, caption, flags, access_hash);
  purple_xfer_set_filename (X, filename);
  g_free (filename);
  
  purple_xfer_set_size (X, size);
  tgprpl_xfer_init_data (X, purple_connection_get_protocol_data (gc), M);
  purple_xfer_request (X);
}

void tgprpl_send_file (PurpleConnection * gc, const char *who, const char *file) {
  debug ("tgprpl_send_file()");
  PurpleXfer *X = tgprpl_new_xfer (gc, who);
  if (file) {
    purple_xfer_request_accepted (X, file);
    debug ("starting xfer...");
  } else {
    purple_xfer_request (X);
  }
}

