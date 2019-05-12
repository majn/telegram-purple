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
 
 Copyright Matthias Jentsch 2014-2016
 */

#include "telegram-purple.h"

#include <errno.h>
#include <locale.h>

#if !GLIB_CHECK_VERSION(2,30,0)
gchar *
g_utf8_substring (const gchar *str,
                  glong        start_pos,
                  glong        end_pos)
{
  gchar *start, *end, *out;

  start = g_utf8_offset_to_pointer (str, start_pos);
  end = g_utf8_offset_to_pointer (start, end_pos - start_pos);

  out = g_malloc (end - start + 1);
  memcpy (out, start, end - start);
  out[end - start] = 0;

  return out;
}
#endif

static char *tgp_msg_service_display (struct tgl_state *TLS, struct tgl_message *M) {
  g_return_val_if_fail(M && M->flags & TGLMF_SERVICE, NULL);

  connection_data *conn = TLS->ev_base;
  char *txt = NULL;
  
  tgl_peer_t *fromPeer = tgl_peer_get (TLS, M->from_id);
  g_return_val_if_fail(fromPeer != NULL, NULL);
  
  const char *txt_user = fromPeer->print_name;

  switch (M->action.type) {
    case tgl_message_action_chat_create:
      txt = g_strdup_printf (_("%2$s created chat %1$s."), M->action.title, txt_user);
      break;
    case tgl_message_action_chat_edit_title:
      txt = g_strdup_printf (_("%2$s changed title to %1$s."), M->action.new_title, txt_user);
      break;
    case tgl_message_action_chat_edit_photo:
      txt = g_strdup_printf (_("%s changed photo."), txt_user);
      break;
    case tgl_message_action_chat_delete_photo:
      txt = g_strdup_printf (_("%s deleted photo."), txt_user);
      break;
    case tgl_message_action_chat_add_user_by_link: {
      tgl_peer_t *actionPeer = tgl_peer_get (TLS, TGL_MK_USER (M->action.user));
      if (actionPeer) {
        char *alias = actionPeer->print_name;
        
        PurpleConversation *conv = purple_find_chat (conn->gc, tgl_get_peer_id (M->to_id));
        txt = g_strdup_printf (_("%1$s added user %2$s by link."), alias, txt_user);
        if (conv) {
          p2tgl_conv_add_user (TLS, conv, tgl_get_peer_id (fromPeer->id), NULL, 0, FALSE);
        }

        return txt;
      }
      break;
    }
    case tgl_message_action_chat_add_users: {
      int i;
      for (i = 0; i < M->action.user_num; ++i) {
        tgl_peer_t *peer = tgl_peer_get (TLS, TGL_MK_USER (M->action.users[i]));
        if (peer) {
          char *alias = peer->print_name;
          txt = g_strdup_printf (_("%2$s added user %1$s."), alias, txt_user);
          PurpleConversation *conv = purple_find_chat (conn->gc, tgl_get_peer_id (M->to_id));
          if (conv) {
            p2tgl_conv_add_user (TLS, conv, M->action.users[i], NULL, 0, FALSE);
          }
        }
      }
      break;
    }
    case tgl_message_action_chat_delete_user: {
      tgl_peer_t *peer = tgl_peer_get (TLS, TGL_MK_USER (M->action.user));
      if (peer) {

        tgl_peer_t *chatPeer = tgl_peer_get (TLS, M->to_id);
        g_return_val_if_fail(tgl_get_peer_type (chatPeer->id) == TGL_PEER_CHAT, NULL);
        
        // make sure that the chat is showing before deleting the user, otherwise the chat will be
        // initialised after removing the chat and the chat will still contain the deleted user
        PurpleConversation *conv = tgp_chat_show (TLS, chatPeer);
        if (conv) {
          char *alias = peer->print_name;
          const char *aliasLeft = tgp_blist_lookup_purple_name (TLS, TGL_MK_USER (M->action.user));

          if (tgl_get_peer_id (M->from_id) != tgl_get_peer_id (peer->id)) {
            txt = g_strdup_printf (_("%2$s deleted user %1$s."), alias, txt_user);
          }
          g_return_val_if_fail (aliasLeft, NULL);

          purple_conv_chat_remove_user (purple_conversation_get_chat_data (conv), aliasLeft, txt);
          if (M->action.user == tgl_get_peer_id (TLS->our_id)) {
            purple_conv_chat_left (purple_conversation_get_chat_data (conv));
          }
          
          // conv_del_user already printed a message, prevent a redundant message
          // from being printed by returning NULL
          g_free (txt);
          return NULL;
        }
        
        char *alias = peer->print_name;
        if (tgl_get_peer_id (M->from_id) != tgl_get_peer_id (peer->id)) {
          txt = g_strdup_printf (_("%2$s deleted user %1$s."), alias, txt_user);
        }
      }
      break;
    }
    case tgl_message_action_set_message_ttl:

      txt = g_strdup_printf (P_("%2$s set self destruction timer to %1$d second.",
                                "%2$s set self destruction timer to %1$d seconds.",
                                M->action.ttl),
                             M->action.ttl, txt_user);
      break;
    case tgl_message_action_read_messages:
      txt = g_strdup_printf (P_("%2$s marked %1$d message read.",
                                "%2$s marked %1$d messages read.",
                                M->action.read_cnt),
                             M->action.read_cnt, txt_user);
      break;
    case tgl_message_action_delete_messages:
      txt = g_strdup_printf (P_("%2$s deleted %1$d message.",
                                "%2$s deleted %1$d messages.",
                                M->action.delete_cnt),
                             M->action.delete_cnt, txt_user);
      break;
    case tgl_message_action_screenshot_messages:
      txt = g_strdup_printf (P_("%2$s made a screenshot of %1$d message.",
                                "%2$s made a screenshot of %1$d messages.",
                                M->action.screenshot_cnt),
                             M->action.screenshot_cnt, txt_user);
      break;
    case tgl_message_action_channel_create: {
      txt = g_strdup_printf (_("Channel %1$s created"), M->action.title);
      
      // FIXME: check if this makes sense
      tgp_chat_blist_store (TLS, fromPeer, _("Telegram Channels"));
      break;
    }
    default:
      g_warn_if_reached();
      break;
  }
  return txt;
}

static char *format_geo_link_osm (double lat, double lon) {
  // assure that the floats are formatted with a dot
  char *loc = setlocale (LC_NUMERIC, NULL);
  setlocale (LC_NUMERIC, "en_US");
  debug ("old locale: %s", loc);

  char *link = g_strdup_printf ("https://www.openstreetmap.org/?mlat=%.6lf&mlon=%.6lf#map=17/%.6lf/%.6lf",
          lat, lon, lat, lon);

  // restore old locale
  setlocale (LC_NUMERIC, loc);
  return link;
}

static char *tgp_msg_file_display (const char *path, const char *filename, const char* caption, const char *mime, long long size) {
  gchar *format;

  gchar *capt = g_markup_escape_text (caption, -1);
  gchar *pth = g_markup_escape_text (path, -1);
  gchar *fle = g_markup_escape_text (filename, -1);
  gchar *mme = g_markup_escape_text (mime, -1);
  gchar *fsize =
#if GLIB_CHECK_VERSION(2,30,0)
    /* 'g_format_size' only exists since 2.30.0. */
    g_format_size (size)
#elif GLIB_CHECK_VERSION(2,16,0)
    /* 'g_format_size_for_display' only exists since 2.16.0.
     * We compile on Windows with glib 2.28.8. */
    g_format_size_for_display (size)
#else /* even older */
  #error "Too outdated glib version!"
#endif
  ;

  format = g_strdup_printf ("[%s <a href=\"file:///%s\">%s</a> %s %s]", capt, pth, fle, mme, fsize);

  g_free (capt);
  g_free (pth);
  g_free (fle);
  g_free (mme);
  g_free (fsize);

  return format;
}

static void tgp_msg_send_done (struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M) {
  if (! success) {
    char *err = _("Sending message failed.");
    warning (err);
    if (M) {
      tgp_msg_special_out (TLS, err, M->to_id, PURPLE_MESSAGE_ERROR | PURPLE_MESSAGE_NO_LOG);
    }
    assert (callback_extra != NULL);
    warning ("Code %d: %s\n", TLS->error_code, TLS->error);
    warning ("Message was: %s\n", (char *) callback_extra);
    return;
  }
  
  write_files_schedule (TLS);
}

static gboolean tgp_msg_send_schedule_cb (gpointer data) {
  connection_data *conn = data;
  conn->out_timer = 0;
  struct tgp_msg_sending *D = NULL;
  while ((D = g_queue_peek_head (conn->out_messages))) {
    g_queue_pop_head (conn->out_messages);

    unsigned long long flags = TGLMF_HTML;
    if (tgl_get_peer_type (D->to) == TGL_PEER_CHANNEL
        && !(tgl_peer_get (conn->TLS, D->to)->channel.flags & TGLCHF_MEGAGROUP)) {
      flags |= TGLMF_POST_AS_CHANNEL;
    }
    
    // secret chats do not unescape html
    if (tgl_get_peer_type (D->to) == TGL_PEER_ENCR_CHAT) {
       gchar *unescaped = purple_unescape_html (D->msg);
       g_free (D->msg);
       D->msg = unescaped;
    }
    
    tgl_do_send_message (D->TLS, D->to, D->msg, (int)strlen (D->msg), flags, NULL, tgp_msg_send_done, D->msg);
    
    tgp_msg_sending_free (D);
  }
  return FALSE;
}

static void tgp_msg_send_schedule (struct tgl_state *TLS, const char *chunk, tgl_peer_id_t to) {
  g_queue_push_tail (tls_get_data (TLS)->out_messages, tgp_msg_sending_init (TLS, g_strdup (chunk), to));
  if (tls_get_data (TLS)->out_timer) {
    purple_timeout_remove (tls_get_data (TLS)->out_timer);
  }
  tls_get_data (TLS)->out_timer = purple_timeout_add (0, tgp_msg_send_schedule_cb, tls_get_data (TLS));
}

void tgp_msg_special_out (struct tgl_state *TLS, const char *msg, tgl_peer_id_t to_id, int flags) {
  if (tgl_get_peer_type (to_id) == TGL_PEER_CHAT) {
    tgp_chat_got_in (TLS, tgl_peer_get (TLS, to_id), to_id, msg, flags, time(0));
  } else {
    // Regular IM conversations will not display specialized message flags like PURPLE_MESSAGE_ERROR or
    // PURPLE_MESSAGE_SYSTEM correctly when using serv_got_in, therefore it is necessary to use the underlying
    // conversation directly.
    const char *name = tgp_blist_lookup_purple_name (TLS, to_id);
    PurpleConversation *conv = p2tgl_find_conversation_with_account (TLS, to_id);
    g_return_if_fail (name);
    if (! conv) {
      // if the conversation isn't open yet, create one with that name
      conv = purple_conversation_new (PURPLE_CONV_TYPE_IM, tls_get_pa (TLS), name);
    }
    purple_conversation_write (conv, name, msg, flags, time(0));
  }
}

void send_inline_picture_done (struct tgl_state *TLS, void *extra, int success, struct tgl_message *msg) {
  if (!success) {
    char *errormsg = g_strdup_printf ("%d: %s", TLS->error_code, TLS->error);
    failure (errormsg);
    purple_notify_message (_telegram_protocol, PURPLE_NOTIFY_MSG_ERROR, _("Sending image failed."),
        errormsg, NULL, NULL, NULL);
    g_free (errormsg);
    return;
  }
}

static GList *tgp_msg_imgs_parse (const char *msg) {
  GList *imgs = NULL;

  int i;
  int len = (int) strlen (msg);
  for (i = 0; i < len; i ++) {
    if (len - i >= 4 && (! memcmp (msg + i, "<IMG", 4) || ! memcmp (msg + i, "<img", 4))) {
      i += 4;

      int e = i;
      while (msg[++ e] != '>' && e < len) {}
      
      gchar *id = NULL;
      if ((id = g_strstr_len (msg + i, e - i, "ID=\"")) || (id = g_strstr_len (msg + i, e - i, "id=\""))) {
        int img = atoi (id + 4);
        debug ("parsed img id %d", img);
        if (img > 0) {
          PurpleStoredImage *psi = purple_imgstore_find_by_id (img);
          if (psi) {
            imgs = g_list_append (imgs, psi);
          } else {
            g_warn_if_reached();
          }
        }
      } else {
        g_warn_if_reached();
      }

      i = e;
    }
  }
  return imgs;
}

static char *tgp_msg_markdown_convert (const char *msg) {
  int len = (int) strlen (msg);
  char *html = g_new0(gchar, 3 * len);

  // strip any known-breaking html tags
  #define STRIP_BROKEN_HTML(PREFIX,SUFFIX) \
    if (g_str_has_prefix (msg, (PREFIX)) && g_str_has_suffix (msg, (SUFFIX))) { \
        msg += sizeof(PREFIX) - 1; \
        len -= (sizeof(PREFIX) - 1) + (sizeof(SUFFIX) - 1); \
    }
  STRIP_BROKEN_HTML("<SPAN style=\"direction:rtl;text-align:right;\">","</SPAN>");
  // more STRIP_BROKEN_HTML invocations here, if necessary
  #undef STRIP_BROKEN_HTML

  int open = FALSE;
  int i, j;
  for (i = 0, j = 0; i < len; i ++) {
    // markdown for bold and italic doesn't seem to work with non-bots, therefore only parse code-tags
    if (len - i < 3 || (memcmp (msg + i, "```", 3))) {
      html[j ++] = msg[i];
    } else {
      i += 2;
      if (! open) {
        assert(j + 6 < 3 * len);
        memcpy(html + j, "<code>", 6);
        j += 6;
      } else {
        assert(j + 7 < 3 * len);
        memcpy(html + j, "</code>", 7);
        j += 7;
      }
      open = ! open;
    }
  }
  html[j] = 0;
  return html;
}

int tgp_msg_send (struct tgl_state *TLS, const char *message, tgl_peer_id_t to) {
  // send all inline images
  GList *imgs = tgp_msg_imgs_parse (message);
  debug ("parsed %d images in messages", g_list_length (imgs));
  while (imgs) {
    PurpleStoredImage *psi = imgs->data;
    gchar *tmp = g_build_filename (g_get_tmp_dir(), purple_imgstore_get_filename (psi), NULL) ;
    GError *err = NULL;
    gconstpointer data = purple_imgstore_get_data (psi);
    g_file_set_contents (tmp, data, purple_imgstore_get_size (psi), &err);
    if (! err) {
      debug ("sending img='%s'", tmp);
      tgl_do_send_document (TLS, to, tmp, NULL, 0,
          TGL_SEND_MSG_FLAG_DOCUMENT_AUTO | (tgl_get_peer_type (to) == TGL_PEER_CHANNEL) ? TGLMF_POST_AS_CHANNEL : 0,
          send_inline_picture_done, NULL);
    } else {
      failure ("error=%s", err->message);
      g_warn_if_reached();
    }
    imgs = g_list_next(imgs);
  }
  
  // replace markdown with html
  char *html = g_strstrip(tgp_msg_markdown_convert (message));
  
  // check message length
  int size = (int) g_utf8_strlen (html, -1);
  if (size == 0) {
    g_free (html);
    return 0; // fail quietly on empty messages
  }
  if (size > TGP_MAX_MSG_SIZE * TGP_DEFAULT_MAX_MSG_SPLIT_COUNT) {
    g_free (html);
    return -E2BIG;
  }

  // send big message as multiple chunks
  int start = 0;
  while (size > start) {
    int end = start + (int)TGP_MAX_MSG_SIZE;
    if (end > size) {
      end = size;
    }
    char *chunk = g_utf8_substring (html, start, end);
    tgp_msg_send_schedule (TLS, chunk, to);
    start = end;
  }
  
  g_free (html);
  
  // return 0 to assure that the picture is not echoed, since
  // it will already be echoed with the outgoing message
  return 0;
}

static char *tgp_msg_photo_display (struct tgl_state *TLS, const char *filename, int *flags) {
  connection_data *conn = TLS->ev_base;
  int img = p2tgl_imgstore_add_with_id (filename);
  if (img <= 0) {
    failure ("Cannot display picture, adding to imgstore failed.");
    return NULL;
  }
  used_images_add (conn, img);
  *flags |= PURPLE_MESSAGE_IMAGES;
  return tgp_format_img (img);
}

static char *tgp_msg_sticker_display (struct tgl_state *TLS, tgl_peer_id_t from, const char *filename, int *flags) {
  char *text = NULL;
  
#ifdef HAVE_LIBWEBP
  connection_data *conn = TLS->ev_base;
  int img = p2tgl_imgstore_add_with_id_webp ((char *) filename);
  if (img <= 0) {
    failure ("Cannot display sticker, adding to imgstore failed");
    return NULL;
  }
  used_images_add (conn, img);
  text = tgp_format_img (img);
  *flags |= PURPLE_MESSAGE_IMAGES;
#else
  const char *txt_user = tgp_blist_lookup_purple_name (TLS, from);
  
  g_return_val_if_fail (txt_user, NULL);
  
  text = g_strdup_printf (_("%s sent a sticker."), txt_user);
  *flags |= PURPLE_MESSAGE_SYSTEM;
#endif
  return text;
}

static char *tgp_msg_reply_display (struct tgl_state *TLS, tgl_peer_t *replyee, struct tgl_message *reply, const char *message) {
  
  g_return_val_if_fail(reply, NULL);
  g_return_val_if_fail(message, NULL);
  
  // the text quoted by the reply
  char *quote = NULL;
  
  if (reply->flags & TGLMF_SERVICE) {
    quote = tgp_msg_service_display (TLS, reply);
    g_return_val_if_fail(quote == NULL, NULL);
    
  } else {
    switch (reply->media.type) {
      case tgl_message_media_none:
        quote = purple_markup_escape_text (reply->message, strlen (reply->message));
        tgp_replace (quote, '\n', ' ');
        break;
        
      case tgl_message_media_photo:
        quote = g_strdup(_("[photo]"));
        break;
        
      case tgl_message_media_audio:
        quote = g_strdup(_("[audio]"));
        break;
        
      case tgl_message_media_video:
        quote = g_strdup(_("[video]"));
        break;
        
      case tgl_message_media_document:
      case tgl_message_media_document_encr:
        quote = g_strdup(_("[document]"));
        break;
        
      case tgl_message_media_geo:
      case tgl_message_media_venue:
        quote = g_strdup(_("[position]")); // TODO: render
        break;
        
      case tgl_message_media_contact:
        quote = g_strdup(_("[contact]")); // TODO: render
        break;
        
      case tgl_message_media_webpage:
        quote = g_strdup(_("[webpage]"));
        break;
        
      case tgl_message_media_unsupported:
        quote = g_strdup(_("[unsupported media]"));
        break;
        
      default:
        g_warn_if_reached();
        return NULL;
        break;
    }
  }

  if (str_not_empty (reply->media.caption)) {
    char *old = quote;
    if (str_not_empty (quote)) {
      quote = g_strdup_printf ("%s %s", old, reply->media.caption);
    } else {
      quote = g_strdup(reply->media.caption);
    }
    g_free (old);
  }
  
  // the combined reply

  // insert line-break in non-adium clients to display
  // all lines of the quotation on the same level
  const char *br = "";
#ifndef __ADIUM_
  br = "<br>";
#endif
  
  char *value = NULL;
  if (replyee) {
    const char *name = replyee->print_name;
    value = g_strdup_printf (_("%s<b>&gt; %s wrote:</b><br>&gt; %s<br>%s"), br, name, quote, message);
  } else {
    value = g_strdup_printf (_("%s<b>&gt; Unknown user wrote:</b><br>&gt; %s<br>%s"), br, quote, message);
  }
  
  g_free (quote);
  
  return value;
}

static void tgp_msg_display (struct tgl_state *TLS, struct tgp_msg_loading *C) {
  struct tgl_message *M = C->msg;
  char *text = NULL;
  int flags = 0;
  
  if (M->flags & (TGLMF_EMPTY | TGLMF_DELETED)) {
    return;
  }
  if (!(M->flags & TGLMF_CREATED)) {
    return;
  }
  if (!tgl_get_peer_type (M->to_id)) {
    warning ("Bad msg\n");
    return;
  }

#ifdef __ADIUM_
  // prevent double messages in Adium >= v1.6
  if (tgp_outgoing_msg (TLS, M)
      && tgl_get_peer_type (M->to_id) != TGL_PEER_CHANNEL
      && tgl_get_peer_type (M->to_id) != TGL_PEER_CHAT) {
    return;
  }
#endif

  // filter empty messages or messages with invalid recipients
  if (! M->message || !tgl_get_peer_type (M->to_id)) {
    return;
  }
  
  // Mark messages that contain a mention as if they contained our current nick name
  // FIXME: doesn't work in Adium
  if (M->flags & TGLMF_MENTION) {
    flags |= PURPLE_MESSAGE_NICK;
  }
  
  // Mark carbons of our own messages
  if (tgp_our_msg (TLS, M)) {
    flags |= PURPLE_MESSAGE_SEND;
    flags &= ~PURPLE_MESSAGE_RECV;
  } else {
    flags |= PURPLE_MESSAGE_RECV;
  }
  
  // handle messages that failed to load
  if (C->error) {
    const char *err = C->error_msg;
    if (! err) {
      err = _("failed loading message");
    }
    tgp_msg_special_out (TLS, err, tgp_our_msg (TLS, M) ? M->from_id : M->to_id, PURPLE_MESSAGE_ERROR);
    return;
  }

  // format the message text
  if (M->flags & TGLMF_SERVICE) {
    text = tgp_msg_service_display (TLS, M);
    flags |= PURPLE_MESSAGE_SYSTEM;
  } else
    switch (M->media.type) {
      case tgl_message_media_none:
        if (str_not_empty (M->message)) {
          text = purple_markup_escape_text (M->message, strlen (M->message));
        }
        break;

      case tgl_message_media_photo:
        if (M->media.photo) {
          g_return_if_fail(C->data != NULL);
          text = tgp_msg_photo_display (TLS, C->data, &flags);
        }
        break;

      case tgl_message_media_document:
      case tgl_message_media_video:
      case tgl_message_media_audio:
        if (M->media.document->flags & TGLDF_STICKER) {
          g_return_if_fail(C->data != NULL);
          text = tgp_msg_sticker_display (TLS, M->from_id, C->data, &flags);

        } else if (M->media.document->flags & TGLDF_IMAGE && !(M->media.document->flags & TGLDF_ANIMATED)) {
          g_return_if_fail(C->data != NULL);
          text = tgp_msg_photo_display (TLS, C->data, &flags);

        } else {
          // Automatically loaded files have C->data set to the file path
          if (C->data) {
            const char* path = C->data;

            // Content of a file transfer
            const char *caption = _("document");
            if (M->media.document->flags & TGLDF_AUDIO) {
              // Content of a file transfer
              caption = _("audio");
            } else if (M->media.document->flags & TGLDF_ANIMATED) {
              // Content of a file transfer
              caption = _("animation");
            } else if (M->media.document->flags & TGLDF_VIDEO) {
              // Content of a file transfer
              caption = _("video");
            }

            const char *filename = M->media.document->caption;
            if (! str_not_empty (filename)) {
              // audio and video snippets recorded from Telegram don't have captions
              const char *segment = path + strlen(path) - 1;
              while (segment > (char *)path && *segment != G_DIR_SEPARATOR) {
                segment --;
              }
              filename = segment + 1;
            }

            const char *mime = "";
            if (str_not_empty (M->media.document->mime_type)) {
              mime = M->media.document->mime_type;
            }

            text = tgp_msg_file_display (path, filename, caption, mime, M->media.document->size);
          } else {
            if (! tgp_our_msg (TLS, M) && ! tls_get_ft_discard (TLS)) {
              tgprpl_recv_file (tls_get_conn (TLS), tgp_blist_lookup_purple_name (TLS, M->from_id), M);
            }
            return;
          }
        }
        break;

      case tgl_message_media_document_encr:
        if (M->media.encr_document->flags & TGLDF_STICKER) {
          g_return_if_fail(C->data != NULL);
          text = tgp_msg_sticker_display (TLS, M->from_id, C->data, &flags);

        } if (M->media.encr_document->flags & TGLDF_IMAGE) {
          g_return_if_fail(C->data != NULL);
          text = tgp_msg_photo_display (TLS, C->data, &flags);

        } else {
          if (! tgp_our_msg (TLS, M)) {
            if (C->data) {
              // Content of a file transfer
              text = tgp_msg_file_display (C->data, M->media.encr_document->caption, _("document"),
                                           M->media.encr_document->mime_type, M->media.encr_document->size);
              
            } else {
              tgprpl_recv_file (tls_get_conn (TLS), tgp_blist_lookup_purple_name (TLS, M->to_id), M);
              return;
            }
          }
        }
        break;
      
      case tgl_message_media_contact:
        text = g_strdup_printf ("<b>%s %s</b> %s", M->media.first_name, M->media.last_name, M->media.phone);
        break;
        
      case tgl_message_media_venue: {
        char *address = NULL;
        if (M->media.venue.address && strcmp (M->media.venue.title, M->media.venue.address)) {
          address = g_strdup_printf (" %s", M->media.venue.address);
        }
        char *pos = format_geo_link_osm (M->media.venue.geo.latitude, M->media.geo.longitude);
        text = g_strdup_printf ("<a href=\"%s\">%s</a>%s",
                                pos,
                                M->media.venue.title ? M->media.venue.title : "", address ? address : "");
        if (address) {
          g_free (address);
        }
        g_free (pos);
        break;
      }
        
      case tgl_message_media_geo: {
        char *pos = format_geo_link_osm (M->media.venue.geo.latitude, M->media.geo.longitude);
        text = g_strdup_printf ("<a href=\"%s\">%s</a>", pos, pos);
        g_free (pos);
        break;
      }
        
      case tgl_message_media_webpage: {
        char *msg = g_strdup (M->message);
        text = purple_markup_escape_text (msg, strlen (msg));
        g_free (msg);
        break;
      }
        
      default:
        warning ("received unknown media type: %d", M->media.type);
        break;
    }

  //Captions are only used for some media types but others will just have them empty
  if (str_not_empty (M->media.caption)) {
    char *old = text;
    if (str_not_empty (text)) {
      text = g_strdup_printf ("%s<br>%s", old, M->media.caption);
    } else {
      text = g_strdup(M->media.caption);
    }
    g_free (old);
  }

  if (tgl_get_peer_type (M->to_id) != TGL_PEER_ENCR_CHAT
      && tgl_get_peer_type (M->to_id) != TGL_PEER_CHANNEL
      && ! (M->flags & TGLMF_UNREAD)) {
    // why?
    flags |= PURPLE_MESSAGE_DELAYED;
  }
  
  // TODO: is that a problem?
  // some service messages (like removing/adding users from chats) might print the message 
  // text through other means and leave the text empty
  if (! str_not_empty (text)) {
    return;
  }

  // forwarded messages
  if (tgl_get_peer_type (M->fwd_from_id) != TGL_PEER_UNKNOWN) {
    debug("forwarded message: fwd_from_id=%d", tgl_get_peer_id(M->fwd_from_id));
    
    // may be NULL
    tgl_peer_t *FP = tgl_peer_get (TLS, M->fwd_from_id);
    
    // do not run this through tgp_message_reply_display! it doesn't handle media, forwards are all about media
    // keep the full body we've generated
    char *tmp = text;
    if (FP) {
      const char *name = FP->print_name;
      text = g_strdup_printf (_("<b>&gt; Forwarded from %s:</b><br>&gt; %s"), name, text);
    } else {
      text = g_strdup_printf (_("<b>&gt; Forwarded:</b><br>&gt; %s"), text);
    }
    g_free (tmp);
    
    g_return_if_fail(text != NULL);
  }
  
  // replys
  if (M->reply_id) {
    debug("message reply: reply_id=%d", M->reply_id);
    
    tgl_message_id_t msg_id = M->permanent_id;
    msg_id.id = M->reply_id;
    
    struct tgl_message *reply = tgl_message_get (TLS, &msg_id);
    g_return_if_fail(reply != NULL);
  
    tgl_peer_t *replyee = tgl_peer_get (TLS, reply->from_id);
    g_return_if_fail(replyee != NULL);
    
    char *tmp = text;
    text = tgp_msg_reply_display(TLS, replyee, reply, tmp);
    g_free (tmp);
    
    g_return_if_fail(text != NULL);
  }

  // display the message to the user
  switch (tgl_get_peer_type (M->to_id)) {
    case TGL_PEER_CHANNEL: {
      tgl_peer_t *P = tgl_peer_get (TLS, M->to_id);
      g_return_if_fail(P != NULL);
      
      if (P->channel.flags & TGLCHF_MEGAGROUP) {
        // sender is the group
        tgp_chat_got_in (TLS, P, M->from_id, text, flags, M->date);
      } else {
        // sender is the channel itself
        tgp_chat_got_in (TLS, P, P->id, text, flags, M->date);
      }
      break;
    }
    case TGL_PEER_CHAT: {
      tgl_peer_t *P = tgl_peer_get (TLS, M->to_id);
      g_return_if_fail(P != NULL);
      tgp_chat_got_in (TLS, P, (flags & PURPLE_MESSAGE_SEND) ? M->to_id : M->from_id, text, flags, M->date);
      break;
    }
    case TGL_PEER_ENCR_CHAT: {
      p2tgl_got_im_combo (TLS, (flags & PURPLE_MESSAGE_SEND) ? M->to_id : M->from_id, text, flags, M->date);
      break;
    }
    case TGL_PEER_USER: {
      p2tgl_got_im_combo (TLS, (flags & PURPLE_MESSAGE_SEND) ? M->to_id : M->from_id, text, flags, M->date);
      break;
    }
  }
  g_free (text);
}

static time_t tgp_msg_oldest_relevant_ts (struct tgl_state *TLS) {
  connection_data *conn = TLS->ev_base;
  int days = purple_account_get_int (conn->pa, TGP_KEY_HISTORY_RETRIEVAL_THRESHOLD,
      TGP_DEFAULT_HISTORY_RETRIEVAL_THRESHOLD);
  return days > 0 ? tgp_time_n_days_ago (days) : 0;
}

static void tgp_msg_process_in_ready (struct tgl_state *TLS) {
  connection_data *conn = TLS->ev_base;
  struct tgp_msg_loading *C;
  
  while ((C = g_queue_peek_head (conn->new_messages))) {
    if (C->pending) {
      break;
    }
    g_queue_pop_head (conn->new_messages);
    
    tgp_msg_display (TLS, C);
    pending_reads_add (TLS, C->msg);

    if (C->data) {
      g_free (C->data);
    }
    
    if (C->error_msg) {
      g_free (C->error_msg);
    }
    
    tgp_msg_loading_free (C);
  }
  pending_reads_send_all (TLS);

  debug ("tgp_msg_process_in_ready, queue size=%d", g_queue_get_length (conn->new_messages));
}

static void tgp_msg_on_loaded_document (struct tgl_state *TLS, void *extra, int success, const char *filename) {
  debug ("tgp_msg_on_loaded_document()");
 
  struct tgp_msg_loading *C = extra;
  
  if (success) {
    C->data = (void *) g_strdup (filename);
  } else {
    g_warn_if_reached();
    C->error = TRUE;
    C->error_msg = g_strdup (_("loading document or picture failed"));
  }
  
  -- C->pending;
  tgp_msg_process_in_ready (TLS);
}

static void tgp_msg_on_loaded_chat_full (struct tgl_state *TLS, void *extra, int success, struct tgl_chat *chat) {
  debug ("tgp_msg_on_loaded_chat_full()");

  if (! success) {
    // foreign user's names won't be displayed in the user list
    g_warn_if_reached();
  }

  struct tgp_msg_loading *C = extra;
  -- C->pending;
  
  tgp_msg_process_in_ready (TLS);
}

static void tgp_msg_on_loaded_channel_history (struct tgl_state *TLS, void *extra, int success, tgl_peer_t *P) {

  struct tgp_msg_loading *C = extra;
  -- C->pending;

  tgp_msg_process_in_ready (TLS);
}

/*
static void tgp_msg_on_loaded_user_full (struct tgl_state *TLS, void *extra, int success, struct tgl_user *U) {
  debug ("tgp_msg_on_loaded_user_full()");

  struct tgp_msg_loading *C = extra;
  -- C->pending;
  tgp_msg_process_in_ready (TLS);
}
*/

/*
 Libpurple message history is immutable and cannot be changed after printing a message.
 TGP currently keeps the first-in first-out queue *new_messages* to ensure that
 the messages are being printed in the correct order. When its necessary to fetch
 additional info (like attached pictures) before this can be done, the queue will hold
 all newer messages until the old message was completely loaded.
*/
void tgp_msg_recv (struct tgl_state *TLS, struct tgl_message *M, GList *before) {
  debug ("tgp_msg_recv before=%p server_id=%lld", before, M->server_id);
  
  if (M->flags & (TGLMF_EMPTY | TGLMF_DELETED)) {
    return;
  }

  if (!(M->flags & TGLMF_CREATED)) {
    return;
  }

  if (!(M->flags & TGLMF_UNREAD) && M->date != 0 && M->date < tgp_msg_oldest_relevant_ts (TLS)) {
    debug ("Message from %d on %d too old, ignored.", tgl_get_peer_id (M->from_id), M->date);
    return;
  }
  
  struct tgp_msg_loading *C = tgp_msg_loading_init (M);
  
  /*
   For non-channels telegram ensures that tgp receives the messages in the correct order, but in channels
   there may be holes that need to be filled before the message log can be printed. This means that the
   queue may not be processed until all historic messages have been fetched and the messages have been
   inserted into the correct position of the queue
   */
  if (tgl_get_peer_type (C->msg->from_id) == TGL_PEER_CHANNEL
      || tgl_get_peer_type (C->msg->to_id) == TGL_PEER_CHANNEL) {
    tgl_peer_id_t id = tgl_get_peer_type (C->msg->from_id) == TGL_PEER_CHANNEL ?
      C->msg->from_id : C->msg->to_id;
    
    if (! tgp_channel_loaded (TLS, id)) {
      ++ C->pending;
      
      tgp_channel_load (TLS, tgl_peer_get (TLS, id), tgp_msg_on_loaded_channel_history, C);
    }
    
    if (tgp_chat_get_last_server_id (TLS, id) >= C->msg->server_id) {
      info ("dropping duplicate channel messages server_id=%lld", C->msg->server_id);
      return;
    }
    if (tgp_chat_get_last_server_id (TLS, id) == (int) C->msg->server_id - 1) {
      tgp_chat_set_last_server_id (TLS, id, (int) C->msg->server_id);
    }
  }
  
  if (! (M->flags & TGLMF_SERVICE)) {

    // handle all messages that need to load content before they can be displayed
    if (M->media.type != tgl_message_media_none) {
      switch (M->media.type) {
        case tgl_message_media_photo: {
          
          // include the "bad photo" check from telegram-cli interface.c:3287 to avoid crashes
          // when fetching history. TODO: find out the reason for this behavior
          if (M->media.photo) {
            ++ C->pending;
            tgl_do_load_photo (TLS, M->media.photo, tgp_msg_on_loaded_document, C);
          }
          break;
        }
          
        // documents that are stickers or images will be displayed just like regular photo messages
        // and need to be loaded beforehand
        case tgl_message_media_document:
        case tgl_message_media_video:
        case tgl_message_media_audio:
          if (M->media.document->flags & (TGLDF_STICKER | TGLDF_IMAGE)) {
            ++ C->pending;
            tgl_do_load_document (TLS, M->media.document, tgp_msg_on_loaded_document, C);
            
          } else {

            // adium doesn't support printing an inline file link to the downloaded file
#ifndef __ADIUM_

            if (M->media.document->size <= tls_get_ft_threshold (TLS) || tls_get_ft_autoload (TLS)) {
              ++ C->pending;

              if (M->media.document->flags & TGLDF_AUDIO) {
                tgl_do_load_audio (TLS, M->media.document, tgp_msg_on_loaded_document, C);

              } else if (M->media.document->flags & TGLDF_VIDEO) {
                tgl_do_load_video (TLS, M->media.document, tgp_msg_on_loaded_document, C);

              } else {
                tgl_do_load_document (TLS, M->media.document, tgp_msg_on_loaded_document, C);
              }
            }

#endif
          }
          break;

        case tgl_message_media_document_encr:
          if (M->media.encr_document->flags & TGLDF_STICKER || M->media.encr_document->flags & TGLDF_IMAGE) {
            ++ C->pending;
            tgl_do_load_encr_document (TLS, M->media.encr_document, tgp_msg_on_loaded_document, C);
          }
          break;

        case tgl_message_media_geo:
          // TODO: load geo thumbnail
          break;

        default:
          // prevent Clang warnings ...
          break;
      }
    }
  }

  /*
   To display a chat, the full name of every single user is needed, but the updates received from the server only
   contain the names of users mentioned in the events. In order to display a messages we always need to fetch the
   full chat info first. If the user list is empty, this means that we still haven't fetched the full chat
   information. Assure that there is only one chat info request for every chat to avoid causing FLOOD_WAIT_X
   errors that will lead to delays or dropped messages
  */
  gpointer to_ptr = GINT_TO_POINTER(tgl_get_peer_id (M->to_id));

  if (! g_hash_table_lookup (tls_get_data (TLS)->pending_chat_info, to_ptr)) {

    if (tgl_get_peer_type (M->to_id) == TGL_PEER_CHAT) {
      tgl_peer_t *P = tgl_peer_get (TLS, M->to_id);
      g_warn_if_fail(P);

      if (P && ! P->chat.user_list_size) {
        ++ C->pending;

        tgl_do_get_chat_info (TLS, M->to_id, FALSE, tgp_msg_on_loaded_chat_full, C);
        g_hash_table_replace (tls_get_data (TLS)->pending_chat_info, to_ptr, to_ptr);
      }
    }
  }

  GList *b = g_queue_find (tls_get_data (TLS)->new_messages, before);
  if (b) {
    struct tgp_msg_loading *M = before->data;
    debug ("inserting before server_id=%lld", M->msg->server_id);
    g_queue_insert_before (tls_get_data (TLS)->new_messages, b, C);
  } else {
    g_queue_push_tail (tls_get_data (TLS)->new_messages, C);
  }
  tgp_msg_process_in_ready (TLS);
}

