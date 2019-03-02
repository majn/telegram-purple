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

    Copyright Matthias Jentsch, Vitaly Valtman, Ben Wiederhake, Christopher Althaus 2014-2015
*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cipher.h>

#include "telegram-base.h"

// O_BINARY exists on windows and must be defined, but doesn't exist on unix-based systems
#ifndef O_BINARY
#define O_BINARY 0
#endif

#define DC_SERIALIZED_MAGIC 0x868aa81d
#define STATE_FILE_MAGIC 0x28949a93
#define SECRET_CHAT_FILE_MAGIC 0x37a1988a

void read_state_file (struct tgl_state *TLS) {
  char *name = 0;
  name = g_strdup_printf("%s/%s", TLS->base_path, "state");

  int state_file_fd = open (name, O_CREAT | O_RDWR | O_BINARY, 0600);
  free (name);

  if (state_file_fd < 0) {
    return;
  }
  int version, magic;
  if (read (state_file_fd, &magic, 4) < 4) { close (state_file_fd); return; }
  if (magic != (int)STATE_FILE_MAGIC) { close (state_file_fd); return; }
  if (read (state_file_fd, &version, 4) < 4 || version < 0) { close (state_file_fd); return; }
  int x[4];
  if (read (state_file_fd, x, 16) < 16) {
    close (state_file_fd); 
    return;
  }
  int pts = x[0];
  int qts = x[1];
  int seq = x[2];
  int date = x[3];
  close (state_file_fd); 
  bl_do_set_seq (TLS, seq);
  bl_do_set_pts (TLS, pts);
  bl_do_set_qts (TLS, qts);
  bl_do_set_date (TLS, date);
  info ("read state file: seq=%d pts=%d qts=%d date=%d", seq, pts, qts, date);
}

void write_state_file (struct tgl_state *TLS) {
  int wseq;
  int wpts;
  int wqts;
  int wdate;
  wseq = TLS->seq; wpts = TLS->pts; wqts = TLS->qts; wdate = TLS->date;
  
  char *name = 0;
  name = g_strdup_printf("%s/%s", TLS->base_path, "state");

  int state_file_fd = open (name, O_CREAT | O_RDWR | O_BINARY, 0600);
  free (name);

  if (state_file_fd < 0) {
    return;
  }
  int x[6];
  x[0] = STATE_FILE_MAGIC;
  x[1] = 0;
  x[2] = wpts;
  x[3] = wqts;
  x[4] = wseq;
  x[5] = wdate;
  assert (write (state_file_fd, x, 24) == 24);
  close (state_file_fd);
  info ("wrote state file: wpts=%d wqts=%d wseq=%d wdate=%d", wpts, wqts, wseq, wdate);
}

static gboolean write_files_gw (gpointer data) {
  struct tgl_state *TLS = data;
  
  ((connection_data *)TLS->ev_base)->write_timer = 0;
  write_state_file (TLS);
  write_secret_chat_file (TLS);
  
  return FALSE;
}

void write_files_schedule (struct tgl_state *TLS) {
  connection_data *conn = TLS->ev_base;
  
  if (! conn->write_timer) {
    conn->write_timer = purple_timeout_add (0, write_files_gw, TLS);
  }
}

struct write_dc_extra {
  int auth_file;
  int flags;
};

void write_dc (struct tgl_dc *DC, void *extra) {
  struct write_dc_extra *ex = extra;
  int auth_file_fd = ex->auth_file;
  if (!DC) { 
    int x = 0;
    assert (write (auth_file_fd, &x, 4) == 4);
    return;
  } else {
    int x = 1;
    assert (write (auth_file_fd, &x, 4) == 4);
  }

  assert (DC->flags & TGLDCF_LOGGED_IN);

  assert (write (auth_file_fd, &DC->options[ex->flags]->port, 4) == 4);
  int l = strlen (DC->options[ex->flags]->ip);
  assert (write (auth_file_fd, &l, 4) == 4);
  assert (write (auth_file_fd, DC->options[ex->flags]->ip, l) == l);
  assert (write (auth_file_fd, &DC->auth_key_id, 8) == 8);
  assert (write (auth_file_fd, DC->auth_key, 256) == 256);
}

void write_auth_file (struct tgl_state *TLS) {
  struct write_dc_extra extra;
  char *name = 0;
  name = g_strdup_printf("%s/%s", TLS->base_path, "auth");
  int auth_file_fd = open (name, O_CREAT | O_RDWR | O_BINARY, 0600);
  free (name);
  if (auth_file_fd < 0) { return; }
  int x = DC_SERIALIZED_MAGIC;
  assert (write (auth_file_fd, &x, 4) == 4);
  assert (write (auth_file_fd, &TLS->max_dc_num, 4) == 4);
  assert (write (auth_file_fd, &TLS->dc_working_num, 4) == 4);

  extra.auth_file = auth_file_fd;
  extra.flags     = TLS->ipv6_enabled ? 1 : 0;

  tgl_dc_iterator_ex (TLS, write_dc, &extra);

  assert (write (auth_file_fd, &TLS->our_id, 4) == 4);
  close (auth_file_fd);
  info ("wrote auth file: magic=%d max_dc_num=%d dc_working_num=%d", x, TLS->max_dc_num, TLS->dc_working_num);
}

void read_dc (struct tgl_state *TLS, int auth_file_fd, int id, unsigned ver) {
  int port = 0;
  assert (read (auth_file_fd, &port, 4) == 4);
  int l = 0;
  assert (read (auth_file_fd, &l, 4) == 4);
  assert (l >= 0 && l < 100);
  char ip[100];
  assert (read (auth_file_fd, ip, l) == l);
  ip[l] = 0;

  long long auth_key_id;
  static unsigned char auth_key[256];
  assert (read (auth_file_fd, &auth_key_id, 8) == 8);
  assert (read (auth_file_fd, auth_key, 256) == 256);

  bl_do_dc_option (TLS, TLS->ipv6_enabled ? 1: 0, id, "DC", 2, ip, l, port);
  bl_do_set_auth_key (TLS, id, auth_key);
  bl_do_dc_signed (TLS, id);
  debug ("read dc: id=%d", id);
}

int tgp_error_if_false (struct tgl_state *TLS, int val, const char *cause, const char *msg) {
  if (! val) {
    connection_data *conn = TLS->ev_base;
    purple_connection_error_reason (conn->gc, PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED, msg);
    purple_notify_message (_telegram_protocol, PURPLE_NOTIFY_MSG_ERROR, cause, msg, NULL, NULL, NULL);
    return TRUE;
  }
  return 0;
}

void empty_auth_file (struct tgl_state *TLS) {
  info ("initializing empty auth file");
  if (TLS->test_mode) {
    bl_do_dc_option (TLS, 0, 1, "", 0, TG_SERVER_TEST_1, strlen (TG_SERVER_TEST_1), 443);
    bl_do_dc_option (TLS, 0, 2, "", 0, TG_SERVER_TEST_2, strlen (TG_SERVER_TEST_2), 443);
    bl_do_dc_option (TLS, 0, 3, "", 0, TG_SERVER_TEST_3, strlen (TG_SERVER_TEST_3), 443);
    bl_do_set_working_dc (TLS, TG_SERVER_TEST_DEFAULT);
  } else {
    if (TLS->ipv6_enabled) {
      bl_do_dc_option (TLS, 1, 1, "", 0, TG_SERVER_IPV6_1, strlen (TG_SERVER_IPV6_1), 443);
      bl_do_dc_option (TLS, 1, 2, "", 0, TG_SERVER_IPV6_2, strlen (TG_SERVER_IPV6_2), 443);
      bl_do_dc_option (TLS, 1, 3, "", 0, TG_SERVER_IPV6_3, strlen (TG_SERVER_IPV6_3), 443);
      bl_do_dc_option (TLS, 1, 4, "", 0, TG_SERVER_IPV6_4, strlen (TG_SERVER_IPV6_4), 443);
      bl_do_dc_option (TLS, 1, 5, "", 0, TG_SERVER_IPV6_5, strlen (TG_SERVER_IPV6_5), 443);
    } else {
      bl_do_dc_option (TLS, 0, 1, "", 0, TG_SERVER_1, strlen (TG_SERVER_1), 443);
      bl_do_dc_option (TLS, 0, 2, "", 0, TG_SERVER_2, strlen (TG_SERVER_2), 443);
      bl_do_dc_option (TLS, 0, 3, "", 0, TG_SERVER_3, strlen (TG_SERVER_3), 443);
      bl_do_dc_option (TLS, 0, 4, "", 0, TG_SERVER_4, strlen (TG_SERVER_4), 443);
      bl_do_dc_option (TLS, 0, 5, "", 0, TG_SERVER_5, strlen (TG_SERVER_5), 443);
    }
    bl_do_set_working_dc (TLS, TG_SERVER_DEFAULT);
  }
}

void read_auth_file (struct tgl_state *TLS) {
  char *name = 0;
  name = g_strdup_printf("%s/%s", TLS->base_path, "auth");
  int auth_file_fd = open (name, O_CREAT | O_RDWR | O_BINARY, 0600);
  free (name);
  if (auth_file_fd < 0) {
    empty_auth_file (TLS);
    return;
  }
  assert (auth_file_fd >= 0);
  unsigned x;
  unsigned m;
  if (read (auth_file_fd, &m, 4) < 4 || (m != DC_SERIALIZED_MAGIC)) {
    close (auth_file_fd);
    empty_auth_file (TLS);
    return;
  }
  assert (read (auth_file_fd, &x, 4) == 4);
  assert (x > 0);
  int dc_working_num;
  assert (read (auth_file_fd, &dc_working_num, 4) == 4);
  
  int i;
  for (i = 0; i <= (int)x; i++) {
    int y;
    assert (read (auth_file_fd, &y, 4) == 4);
    if (y) {
      read_dc (TLS, auth_file_fd, i, m);
    }
  }
  bl_do_set_working_dc (TLS, dc_working_num);
  int our_id;
  int l = read (auth_file_fd, &our_id, 4);
  if (l < 4) {
    assert (!l);
  }
  if (our_id) {
    bl_do_set_our_id (TLS, TGL_MK_USER (our_id));
  }
  close (auth_file_fd);
  info ("read auth file: dcs=%d dc_working_num=%d our_id=%d", x, dc_working_num, our_id);
}

void write_secret_chat (tgl_peer_t *_P, void *extra) {
  struct tgl_secret_chat *P = (void *)_P;
  if (tgl_get_peer_type (P->id) != TGL_PEER_ENCR_CHAT) { return; }
  if (P->state != sc_ok) { return; }
  int *a = extra;
  int fd = a[0];
  a[1] ++;
  
  int id = tgl_get_peer_id (P->id);
  assert (write (fd, &id, 4) == 4);
  int l = strlen (P->print_name);
  assert (write (fd, &l, 4) == 4);
  assert (write (fd, P->print_name, l) == l);
  assert (write (fd, &P->user_id, 4) == 4);
  assert (write (fd, &P->admin_id, 4) == 4);
  assert (write (fd, &P->date, 4) == 4);
  assert (write (fd, &P->ttl, 4) == 4);
  assert (write (fd, &P->layer, 4) == 4);
  assert (write (fd, &P->access_hash, 8) == 8);
  assert (write (fd, &P->state, 4) == 4);
  assert (write (fd, &P->key_fingerprint, 8) == 8);
  assert (write (fd, &P->key, 256) == 256);
  assert (write (fd, &P->first_key_sha, 20) == 20);
  assert (write (fd, &P->in_seq_no, 4) == 4);
  assert (write (fd, &P->last_in_seq_no, 4) == 4);
  assert (write (fd, &P->out_seq_no, 4) == 4);
  debug ("wrote secret chat: %s, state=%d, in_seq_no=%d, out_seq_no=%d", P->print_name, P->state, P->in_seq_no, P->out_seq_no);
}

void write_secret_chat_file (struct tgl_state *TLS) {
  char *name = 0;
  name = g_strdup_printf("%s/%s", TLS->base_path, "secret");
  int secret_chat_fd = open (name, O_CREAT | O_RDWR | O_BINARY, 0600);
  free (name);
  assert (secret_chat_fd >= 0);
  int x = SECRET_CHAT_FILE_MAGIC;
  assert (write (secret_chat_fd, &x, 4) == 4);
  x = 2;
  assert (write (secret_chat_fd, &x, 4) == 4); // version
  assert (write (secret_chat_fd, &x, 4) == 4); // num
  
  int y[2];
  y[0] = secret_chat_fd;
  y[1] = 0;
  
  tgl_peer_iterator_ex (TLS, write_secret_chat, y);
  
  lseek (secret_chat_fd, 8, SEEK_SET);
  assert (write (secret_chat_fd, &y[1], 4) == 4);
  close (secret_chat_fd);
  info ("wrote secret chat file: %d chats written.", y[1]);
}

void read_secret_chat (struct tgl_state *TLS, int fd, int v) {
  int id, l, user_id, admin_id, date, ttl, layer, state;
  long long access_hash, key_fingerprint;
  static char s[1000];
  static unsigned char key[256];
  static unsigned char sha[20];
  assert (read (fd, &id, 4) == 4);
  assert (read (fd, &l, 4) == 4);
  assert (l > 0 && l < 999);
  assert (read (fd, s, l) == l);
  assert (read (fd, &user_id, 4) == 4);
  assert (read (fd, &admin_id, 4) == 4);
  assert (read (fd, &date, 4) == 4);
  assert (read (fd, &ttl, 4) == 4);
  assert (read (fd, &layer, 4) == 4);
  assert (read (fd, &access_hash, 8) == 8);
  assert (read (fd, &state, 4) == 4);
  assert (read (fd, &key_fingerprint, 8) == 8);
  assert (read (fd, &key, 256) == 256);
  if (v >= 2) {
    assert (read (fd, sha, 20) == 20);
  } else {
    PurpleCipher *sha1_cipher = purple_ciphers_find_cipher("sha1");
    PurpleCipherContext *sha1_ctx = purple_cipher_context_new(sha1_cipher, NULL);
    purple_cipher_context_append(sha1_ctx, key, 256);
    purple_cipher_context_digest(sha1_ctx, 20, sha, NULL);
    purple_cipher_context_destroy(sha1_ctx);
  }
  int in_seq_no = 0, out_seq_no = 0, last_in_seq_no = 0;
  if (v >= 1) {
    assert (read (fd, &in_seq_no, 4) == 4);
    assert (read (fd, &last_in_seq_no, 4) == 4);
    assert (read (fd, &out_seq_no, 4) == 4);
  }
  
  s[l] = '\0';
  debug ("read secret chat: %s, state=%d, in_seq_no=%d, last_in_seq_no=%d, out_seq_no=%d",
      s, state, in_seq_no, last_in_seq_no, out_seq_no);
  bl_do_encr_chat (TLS, id, &access_hash, &date, &admin_id, &user_id, key, NULL, sha, &state, &ttl,
      &layer, &in_seq_no, &last_in_seq_no, &out_seq_no, &key_fingerprint, TGLECF_CREATE | TGLECF_CREATED,
      s, l);
}

void read_secret_chat_file (struct tgl_state *TLS) {
  char *name = 0;
  name = g_strdup_printf("%s/%s", TLS->base_path, "secret");
  
  int secret_chat_fd = open (name, O_RDWR | O_BINARY, 0600);
  free (name);
  
  if (secret_chat_fd < 0) { return; }
  
  int x;
  if (read (secret_chat_fd, &x, 4) < 4) { close (secret_chat_fd); return; }
  if (x != SECRET_CHAT_FILE_MAGIC) { close (secret_chat_fd); return; }
  int v = 0;
  assert (read (secret_chat_fd, &v, 4) == 4);
  assert (v == 0 || v == 1 || v == 2); // version
  assert (read (secret_chat_fd, &x, 4) == 4);
  assert (x >= 0);
  int cnt = x;
  while (x -- > 0) {
    read_secret_chat (TLS, secret_chat_fd, v);
  }
  close (secret_chat_fd);
  info ("read secret chat file: %d chats read", cnt);
}

gchar *get_config_dir (char const *username) {
  gchar *dir = g_strconcat (purple_user_dir(), G_DIR_SEPARATOR_S, config_dir,
                                G_DIR_SEPARATOR_S, username, NULL);
  
  if (g_str_has_prefix (dir, g_get_tmp_dir())) {
    // telepathy-haze will set purple user dir to a tmp path,
    // but we need the files to be persistent
    g_free (dir);
    dir = g_strconcat (g_get_home_dir(), G_DIR_SEPARATOR_S, ".telegram-purple",
                                  G_DIR_SEPARATOR_S, username, NULL);
  }
  g_mkdir_with_parents (dir, 0700);
  return dir;
}

gchar *get_download_dir (struct tgl_state *TLS) {
  assert (TLS->base_path);
  static gchar *dir;
  if (dir) {
    g_free (dir);
  }
  dir = g_strconcat (TLS->base_path, G_DIR_SEPARATOR_S, "downloads", NULL);
  g_mkdir_with_parents (dir, 0700);
  return dir;
}

void write_secret_chat_gw (struct tgl_state *TLS, void *extra, int success, struct tgl_secret_chat *_) {
  if (!success) {
    tgp_notify_on_error_gw (TLS, NULL, success);
    return;
  }
  write_secret_chat_file (TLS);
}

void tgp_create_group_chat_by_usernames (struct tgl_state *TLS, const char *title, const char **users,
      int num_users, int use_print_names) {
  tgl_peer_id_t ids[num_users + 1];
  int i, j = 0;
  ids[j++] = TLS->our_id;
  for (i = 0; i < num_users; i++) if (str_not_empty (users[i])) {
    tgl_peer_t *P = NULL;
    if (use_print_names) {
       // used by Adium autocompletion that is based on print_names
      P = tgl_peer_get_by_name (TLS, users[i]);
    } else {
      P = tgp_blist_lookup_peer_get (TLS, users[i]);
    }
    if (P && tgl_get_peer_id (P->id) != tgl_get_peer_id (TLS->our_id)) {
      debug ("Adding %s: %d", P->print_name, tgl_get_peer_id (P->id));
      ids[j++] = P->id;
    } else {
      debug ("User %s not found in peer list", users[j]);
    }
  }
  if (j > 1) {
    tgl_do_create_group_chat (TLS, j, ids, title, (int) strlen(title),
        tgp_notify_on_error_gw, g_strdup (title));
  } else {
    purple_notify_message (_telegram_protocol, PURPLE_NOTIFY_MSG_INFO, _("Couldn't create group"),
        _("Please select at least one other user."), NULL, NULL, NULL);
  }
}

/**
 * This function generates a png image to visualize the sha1 key from an encrypted chat.
 */
int tgp_visualize_key (struct tgl_state *TLS, unsigned char* sha1_key) {
  int colors[4] = {
    0xffffff,
    0xd5e6f3,
    0x2d5775,
    0x2f99c9
  };
  unsigned img_size = 160;
  unsigned char* image = (unsigned char*)malloc (img_size * img_size * 4);
  assert (image);
  unsigned x, y, i, j, idx = 0;
  int bitpointer = 0;
  for (y = 0; y < 8; y++)
  {
    unsigned offset_y = y * img_size * 4 * (img_size / 8);
    for (x = 0; x < 8; x++)
    {
      int offset = bitpointer / 8;
      int shiftOffset = bitpointer % 8;
      int val = sha1_key[offset + 3] << 24 | sha1_key[offset + 2] << 16 | sha1_key[offset + 1] << 8 | sha1_key[offset];
      idx = abs ((val >> shiftOffset) & 3) % 4;
      bitpointer += 2;
      unsigned offset_x = x * 4 * (img_size / 8);
      for (i = 0; i < img_size / 8; i++)
      {
        unsigned off_y = offset_y + i * img_size * 4;
        for (j = 0; j < img_size / 8; j++)
        {
          unsigned off_x = offset_x + j * 4;
          image[off_y + off_x + 0] = colors[idx] & 0xFF;
          image[off_y + off_x + 1] = (colors[idx] >> 8) & 0xFF;
          image[off_y + off_x + 2] = (colors[idx] >> 16) & 0xFF;
          image[off_y + off_x + 3] = 0xFF;
        }
      }
    }
  }
  int imgStoreId = p2tgl_imgstore_add_with_id_raw(image, img_size, img_size);
  used_images_add ((connection_data*)TLS->ev_base, imgStoreId);
  g_free (image);
  return imgStoreId;
}

void tgp_notify_on_error_gw (struct tgl_state *TLS, void *extra, int success) {
  if (!success) {
    char *errormsg = g_strdup_printf ("%d: %s", TLS->error_code, TLS->error);
    failure (errormsg);
    purple_notify_message (_telegram_protocol, PURPLE_NOTIFY_MSG_ERROR, _("Query Failed"),
                           errormsg, NULL, NULL, NULL);
    g_free (errormsg);
    return;
  }
}
