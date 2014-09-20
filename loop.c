/*
    This file is part of telegram-client.

    Telegram-client is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Telegram-client is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this telegram-client.  If not, see <http://www.gnu.org/licenses/>.

    Copyright Vitaly Valtman 2013
*/

#define _GNU_SOURCE
#define READLINE_CALLBACKS

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "net.h"
#include "mtproto-client.h"
#include "queries.h"
#include "telegram.h"
#include "loop.h"
#include "binlog.h"

//

#include "purple-plugin/telegram-purple.h"
//

char *get_auth_key_filename (void);
char *get_state_filename (void);
int zero[512];


void write_dc (int auth_file_fd, struct dc *DC) {
  logprintf("writing to auth_file: auth_file_fd: %d, port: %d, ip: %s\n", auth_file_fd, DC->port, DC->ip);
  assert (write (auth_file_fd, &DC->port, 4) == 4);
  int l = strlen (DC->ip);
  assert (write (auth_file_fd, &l, 4) == 4);
  assert (write (auth_file_fd, DC->ip, l) == l);
  if (DC->flags & 1) {
    assert (write (auth_file_fd, &DC->auth_key_id, 8) == 8);
    assert (write (auth_file_fd, DC->auth_key, 256) == 256);
  } else {
    assert (write (auth_file_fd, zero, 256 + 8) == 256 + 8);
  }

  assert (write (auth_file_fd, &DC->server_salt, 8) == 8);
  assert (write (auth_file_fd, &DC->has_auth, 4) == 4);
}

void write_auth_file (struct authorization_state *state, const char *filename) {
  logprintf("Writing to auth_file: %s\n", filename);
  int auth_file_fd = open (filename, O_CREAT | O_RDWR, 0600);
  assert (auth_file_fd >= 0);
  int x = DC_SERIALIZED_MAGIC_V2;
  assert (write (auth_file_fd, &x, 4) == 4);
  x = MAX_DC_ID;
  assert (write (auth_file_fd, &x, 4) == 4);
  assert (write (auth_file_fd, &state->dc_working_num, 4) == 4);
  assert (write (auth_file_fd, &state->auth_state, 4) == 4);
  int i;
  for (i = 0; i <= MAX_DC_ID; i++) {
    if (state->DC_list[i]) {
      x = 1;
      assert (write (auth_file_fd, &x, 4) == 4);
      write_dc (auth_file_fd, state->DC_list[i]);
    } else {
      x = 0;
      assert (write (auth_file_fd, &x, 4) == 4);
    }
  }
  assert (write (auth_file_fd, &state->our_id, 4) == 4);
  close (auth_file_fd);
}

void read_dc (int auth_file_fd, int id, unsigned ver, struct dc *DC_list[]) {
  int port = 0;
  assert (read (auth_file_fd, &port, 4) == 4);
  int l = 0;
  assert (read (auth_file_fd, &l, 4) == 4);
  assert (l >= 0);
  char *ip = talloc (l + 1);
  assert (read (auth_file_fd, ip, l) == l);
  ip[l] = 0;
  struct dc *DC = alloc_dc (DC_list, id, ip, port);
  assert (read (auth_file_fd, &DC->auth_key_id, 8) == 8);
  assert (read (auth_file_fd, &DC->auth_key, 256) == 256);
  assert (read (auth_file_fd, &DC->server_salt, 8) == 8);
  if (DC->auth_key_id) {
    DC->flags |= 1;
  }
  if (ver != DC_SERIALIZED_MAGIC) {
    assert (read (auth_file_fd, &DC->has_auth, 4) == 4);
  } else {
    DC->has_auth = 0;
  }
}


void empty_auth_file (const char *filename) {
  struct authorization_state state;
  memset(state.DC_list, 0, 11 * sizeof(void *));

  logprintf("empty_auth_file()\n");
  alloc_dc (state.DC_list, 1, tstrdup (TG_SERVER), 443);
  state.dc_working_num = 1;
  state.auth_state = 0;
  write_auth_file (&state, filename);
}

/**
 * Read the auth-file and return the read authorization state
 *
 * When the given file doesn't exist, create a new empty 
 * file containing the default authorization state at this
 * path
 */
struct authorization_state read_auth_file (const char *filename) {
  logprintf("read_auth_file()\n");

  struct authorization_state state;
  memset(state.DC_list, 0, 11 * sizeof(void *));

  int auth_file_fd = open (filename, O_RDWR, 0600);
  logprintf("fd: %d\n", auth_file_fd);
  if (auth_file_fd < 0) {
    logprintf("auth_file does not exist, creating empty...\n");
    empty_auth_file (filename);
  }
  auth_file_fd = open (filename, O_RDWR, 0600);
  assert (auth_file_fd >= 0);

  // amount of data centers
  unsigned x;
  // magic number of file
  unsigned m;
  if (read (auth_file_fd, &m, 4) < 4 || (m != DC_SERIALIZED_MAGIC && m != DC_SERIALIZED_MAGIC_V2)) {
    logprintf("Invalid File content, wrong Magic numebr\n");
    close (auth_file_fd);
    empty_auth_file (filename);
    return state;
  }
  assert (read (auth_file_fd, &x, 4) == 4);
  assert (x <= MAX_DC_ID);
  assert (read (auth_file_fd, &state.dc_working_num, 4) == 4);
  assert (read (auth_file_fd, &state.auth_state, 4) == 4);
  if (m == DC_SERIALIZED_MAGIC) {
    state.auth_state = 700;
  }
  int i;
  for (i = 0; i <= (int)x; i++) {
    int y;
    assert (read (auth_file_fd, &y, 4) == 4);
    if (y) {
      read_dc (auth_file_fd, i, m, state.DC_list);
      logprintf("loaded dc[%d] - port: %d, ip: %s, auth_key_id: %lli, server_salt: %lli, has_auth: %d\n", 
          i, state.DC_list[i]->port, state.DC_list[i]->ip, state.DC_list[i]->auth_key_id, 
          state.DC_list[i]->server_salt, state.DC_list[i]->has_auth);
    } else {
      logprintf("loaded dc[%d] - NULL\n", i);
    }
  }
  int l = read (auth_file_fd, &state.our_id, 4);
  if (l < 4) {
    assert (!l);
  }
  close (auth_file_fd);
  struct dc *DC_working = state.DC_list[state.dc_working_num];
  if (m == DC_SERIALIZED_MAGIC) {
    DC_working->has_auth = 1;
  }
  logprintf("loaded authorization state - our_id: %d, auth_state: %d, dc_working_num: %d \n", state.our_id, state.auth_state, state.dc_working_num);
  return state;
}

struct protocol_state read_state_file (const char *filename) {
  logprintf("read_state_file()\n");
  struct protocol_state state = {0, 0, 0, 0};

  int state_file_fd = open (filename, O_CREAT | O_RDWR, 0600);
  if (state_file_fd < 0) {
    return state;
  }
  int version, magic;
  if (read (state_file_fd, &magic, 4) < 4) { close (state_file_fd); return state; }
  if (magic != (int)STATE_FILE_MAGIC) { close (state_file_fd); return state; }
  if (read (state_file_fd, &version, 4) < 4) { close (state_file_fd); return state; }
  assert (version >= 0);
  int x[4];
  if (read (state_file_fd, x, 16) < 16) {
    close (state_file_fd);
    return state;
  }
  state.pts = x[0];
  state.qts = x[1];
  state.seq = x[2];
  state.last_date = x[3];
  close (state_file_fd);
  logprintf("loaded session state - pts: %d, qts: %d, seq: %d, last_date: %d.\n", state.pts, 
    state.qts, state.seq, state.last_date);
  return state;
}

void write_state_file (struct protocol_state *state, const char* filename) {
  int state_file_fd = open (filename, O_CREAT | O_RDWR, 0600);
  if (state_file_fd < 0) {
    return;
  }
  int x[6];
  x[0] = STATE_FILE_MAGIC;
  x[1] = 0;
  x[2] = state->pts;
  x[3] = state->qts;
  x[4] = state->seq;
  x[5] = state->last_date;
  assert (write (state_file_fd, x, 24) == 24);
  close (state_file_fd);
}

void read_secret_chat_file (struct telegram *instance, const char *file) {
  struct binlog *bl = instance->bl;

  int fd = open (file, O_CREAT | O_RDWR, 0600);
  if (fd < 0) {
    return;
  }
  int x[2];
  if (read (fd, x, 8) < 8) {
    close (fd); return;
  }
  if (x[0] != (int)SECRET_CHAT_FILE_MAGIC) { close (fd); return; }
  int version = x[1];
  assert (version >= 0);
  int cc;
  assert (read (fd, &cc, 4) == 4);
  int i;
  for (i = 0; i < cc; i++) {
    peer_t *P = talloc0 (sizeof (*P));
    struct secret_chat *E = &P->encr_chat;
    int t;
    assert (read (fd, &t, 4) == 4);
    P->id = MK_ENCR_CHAT (t);
    assert (read (fd, &P->flags, 4) == 4);
    assert (read (fd, &t, 4) == 4);
    assert (t > 0);
    P->print_name = talloc (t + 1);
    assert (read (fd, P->print_name, t) == t);
    P->print_name[t] = 0;
    peer_insert_name (bl, P);

    assert (read (fd, &E->state, 4) == 4);
    assert (read (fd, &E->user_id, 4) == 4);
    assert (read (fd, &E->admin_id, 4) == 4);
    assert (read (fd, &E->ttl, 4) == 4);
    assert (read (fd, &E->access_hash, 8) == 8);

    if (E->state != sc_waiting) {
      E->g_key = talloc (256);
      assert (read (fd, E->g_key, 256) == 256);
      E->nonce = talloc (256);
      assert (read (fd, E->nonce, 256) == 256);
    }
    assert (read (fd, E->key, 256) == 256);
    assert (read (fd, &E->key_fingerprint, 8) == 8);
    insert_encrypted_chat (bl, P);
  }
  if (version >= 1) {
    assert (read (fd, &instance->encr_root, 4) == 4);
    if (instance->encr_root) {
      assert (read (fd, &instance->encr_param_version, 4) == 4);
      instance->encr_prime = talloc (256);
      assert (read (fd, instance->encr_prime, 256) == 256);
    }
  }
  close (fd);
}

// TODO: Refactor
void write_secret_chat_file (struct telegram *instance, const char *filename) {
  struct binlog *bl = instance->bl;

  int fd = open (filename, O_CREAT | O_RDWR, 0600);
  if (fd < 0) {
    return;
  }
  int x[2];
  x[0] = SECRET_CHAT_FILE_MAGIC;
  x[1] = 1;
  assert (write (fd, x, 8) == 8);
  int i;
  int cc = 0;
  for (i = 0; i < bl->peer_num; i++) if (get_peer_type (bl->Peers[i]->id) == PEER_ENCR_CHAT) {
    if (bl->Peers[i]->encr_chat.state != sc_none && bl->Peers[i]->encr_chat.state != sc_deleted) {
      cc ++;
    }
  }
  assert (write (fd, &cc, 4) == 4);
  for (i = 0; i < bl->peer_num; i++) if (get_peer_type (bl->Peers[i]->id) == PEER_ENCR_CHAT) {
    if (bl->Peers[i]->encr_chat.state != sc_none && bl->Peers[i]->encr_chat.state != sc_deleted) {
      int t = get_peer_id (bl->Peers[i]->id);
      assert (write (fd, &t, 4) == 4);
      t = bl->Peers[i]->flags;
      assert (write (fd, &t, 4) == 4);
      t = strlen (bl->Peers[i]->print_name);
      assert (write (fd, &t, 4) == 4);
      assert (write (fd, bl->Peers[i]->print_name, t) == t);

      assert (write (fd, &bl->Peers[i]->encr_chat.state, 4) == 4);

      assert (write (fd, &bl->Peers[i]->encr_chat.user_id, 4) == 4);
      assert (write (fd, &bl->Peers[i]->encr_chat.admin_id, 4) == 4);
      assert (write (fd, &bl->Peers[i]->encr_chat.ttl, 4) == 4);
      assert (write (fd, &bl->Peers[i]->encr_chat.access_hash, 8) == 8);
      if (bl->Peers[i]->encr_chat.state != sc_waiting) {
        assert (write (fd, bl->Peers[i]->encr_chat.g_key, 256) == 256);
      }
      if (bl->Peers[i]->encr_chat.state != sc_waiting) {
        assert (write (fd, bl->Peers[i]->encr_chat.nonce, 256) == 256);
      }
      assert (write (fd, bl->Peers[i]->encr_chat.key, 256) == 256);
      assert (write (fd, &bl->Peers[i]->encr_chat.key_fingerprint, 8) == 8);
    }
  }
  assert (write (fd, &instance->encr_root, 4) == 4);
  if (instance->encr_root) {
    assert (write (fd, &instance->encr_param_version, 4) == 4);
    assert (write (fd, instance->encr_prime, 256) == 256);
  }
  close (fd);
}

