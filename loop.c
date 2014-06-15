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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE
#define READLINE_CALLBACKS

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef READLINE_GNU
#include <readline/readline.h>
#include <readline/history.h>
#else
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "interface.h"
#include "net.h"
#include "mtproto-client.h"
#include "mtproto-common.h"
#include "queries.h"
#include "telegram.h"
#include "loop.h"
#include "binlog.h"
#include "lua-tg.h"

//

#include "purple-plugin/telegram-purple.h"
//


extern char *default_username;
extern char *auth_token;
extern int test_dc;
void set_default_username (const char *s);
int default_dc_num;
extern int binlog_enabled;

extern int unknown_user_list_pos;
extern int unknown_user_list[];
int register_mode;
extern int safe_quit;
extern int queries_num;

int unread_messages;
void got_it (char *line, int len);
void net_loop (int flags, int (*is_end)(void)) {
  logprintf("starting net_loop()\n");
  while (!is_end ()) {
    struct pollfd fds[101];
    int cc = 0;
    if (flags & 3) {
      fds[0].fd = 0;
      fds[0].events = POLLIN;
      cc ++;
    }

    logprintf("writing_state_file()\n");
    write_state_file ();
    int x = connections_make_poll_array (fds + cc, 101 - cc) + cc;
    double timer = next_timer_in ();
    if (timer > 1000) { timer = 1000; }
    if (poll (fds, x, timer) < 0) {
	  logprintf("poll returned -1, wait a little bit.\n");
      work_timers ();
      continue;
    }
    work_timers ();
    if ((flags & 3) && (fds[0].revents & POLLIN)) {
      unread_messages = 0;
      if (flags & 1) {
        rl_callback_read_char ();
      } else {
        char *line = 0;
        size_t len = 0;
        assert (getline (&line, &len, stdin) >= 0);
        got_it (line, strlen (line));
      }
    }
    connections_poll_result (fds + cc, x - cc);
    #ifdef USE_LUA
      lua_do_all ();
    #endif
    if (safe_quit && !queries_num) {
      logprintf ("All done. Exit\n");
      rl_callback_handler_remove ();
      exit (0);
    }
    if (unknown_user_list_pos) {
      do_get_user_list_info_silent (unknown_user_list_pos, unknown_user_list);
      unknown_user_list_pos = 0;
    }
  }
}

char **_s;
size_t *_l;
int got_it_ok;

void got_it (char *line, int len) {
  assert (len > 0);
  line[-- len] = 0; // delete end of line
  *_s = line;
  *_l = len;
  got_it_ok = 1;
}

int is_got_it (void) {
  return got_it_ok;
}

int net_getline (char **s, size_t *l) {
  fflush (stdout);
//  rl_already_prompted = 1;
  got_it_ok = 0;
  _s = s;
  _l = l;
//  rl_callback_handler_install (0, got_it);
  net_loop (2, is_got_it);
  return 0;
}

int ret1 (void) { return 0; }

int main_loop (void) {
  net_loop (1, ret1);
  return 0;
}


struct dc *DC_list[MAX_DC_ID + 1];
struct dc *DC_working;
int dc_working_num;
int auth_state;
char *get_auth_key_filename (void);
char *get_state_filename (void);
char *get_secret_chat_filename (void);
int zero[512];


void write_dc (int auth_file_fd, struct dc *DC) {
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

int our_id;

void store_config () {
  write_auth_file();
}

void write_auth_file (void) {
  if (binlog_enabled) { return; }
  int auth_file_fd = open (get_auth_key_filename (), O_CREAT | O_RDWR, 0600);
  assert (auth_file_fd >= 0);
  int x = DC_SERIALIZED_MAGIC_V2;
  assert (write (auth_file_fd, &x, 4) == 4);
  x = MAX_DC_ID;
  assert (write (auth_file_fd, &x, 4) == 4);
  assert (write (auth_file_fd, &dc_working_num, 4) == 4);
  assert (write (auth_file_fd, &auth_state, 4) == 4);
  int i;
  for (i = 0; i <= MAX_DC_ID; i++) {
    if (DC_list[i]) {
      x = 1;
      assert (write (auth_file_fd, &x, 4) == 4);
      write_dc (auth_file_fd, DC_list[i]);
    } else {
      x = 0;
      assert (write (auth_file_fd, &x, 4) == 4);
    }
  }
  assert (write (auth_file_fd, &our_id, 4) == 4);
  close (auth_file_fd);
}

void read_dc (int auth_file_fd, int id, unsigned ver) {
  int port = 0;
  assert (read (auth_file_fd, &port, 4) == 4);
  int l = 0;
  assert (read (auth_file_fd, &l, 4) == 4);
  assert (l >= 0);
  char *ip = talloc (l + 1);
  assert (read (auth_file_fd, ip, l) == l);
  ip[l] = 0;
  struct dc *DC = alloc_dc (id, ip, port);
  assert (read (auth_file_fd, &DC->auth_key_id, 8) == 8);
  assert (read (auth_file_fd, &DC->auth_key, 256) == 256);
  assert (read (auth_file_fd, &DC->server_salt, 8) == 8);
  logprintf("auth_key_id: %lli \n", DC->auth_key_id);
  logprintf("auth_key_id: ?");
  logprintf("server_salt: %lli \n", DC->server_salt);
  if (DC->auth_key_id) {
    DC->flags |= 1;
  }
  if (ver != DC_SERIALIZED_MAGIC) {
    assert (read (auth_file_fd, &DC->has_auth, 4) == 4);
  } else {
    DC->has_auth = 0;
  }
}

void empty_auth_file (void) {
  alloc_dc (1, tstrdup (test_dc ? TG_SERVER_TEST : TG_SERVER), 443);
  dc_working_num = 1;
  auth_state = 0;
  write_auth_file ();
}

int need_dc_list_update;
void read_auth_file (void) {
  if (binlog_enabled) { return; }
  int auth_file_fd = open (get_auth_key_filename (), O_CREAT | O_RDWR, 0600);
  if (auth_file_fd < 0) {
    empty_auth_file ();
  }
  assert (auth_file_fd >= 0);

  // amount of data centers
  unsigned x;
  // magic number of file
  unsigned m;
  if (read (auth_file_fd, &m, 4) < 4 || (m != DC_SERIALIZED_MAGIC && m != DC_SERIALIZED_MAGIC_V2)) {
    close (auth_file_fd);
    empty_auth_file ();
    return;
  }
  assert (read (auth_file_fd, &x, 4) == 4);
  assert (x <= MAX_DC_ID);
  assert (read (auth_file_fd, &dc_working_num, 4) == 4);
  assert (read (auth_file_fd, &auth_state, 4) == 4);
  if (m == DC_SERIALIZED_MAGIC) {
    auth_state = 700;
  }
  int i;
  for (i = 0; i <= (int)x; i++) {
    int y;
    assert (read (auth_file_fd, &y, 4) == 4);
    if (y) {
      read_dc (auth_file_fd, i, m);
    }
  }
  int l = read (auth_file_fd, &our_id, 4);
  if (l < 4) {
    assert (!l);
  }
  close (auth_file_fd);
  DC_working = DC_list[dc_working_num];
  if (m == DC_SERIALIZED_MAGIC) {
    DC_working->has_auth = 1;
  }
}

int pts, qts, seq, last_date;

void read_state_file (void) {
  if (binlog_enabled) { return; }
  int state_file_fd = open (get_state_filename (), O_CREAT | O_RDWR, 0600);
  if (state_file_fd < 0) {
    return;
  }
  int version, magic;
  if (read (state_file_fd, &magic, 4) < 4) { close (state_file_fd); return; }
  if (magic != (int)STATE_FILE_MAGIC) { close (state_file_fd); return; }
  if (read (state_file_fd, &version, 4) < 4) { close (state_file_fd); return; }
  assert (version >= 0);
  int x[4];
  if (read (state_file_fd, x, 16) < 16) {
    close (state_file_fd);
    return;
  }
  pts = x[0];
  qts = x[1];
  seq = x[2];
  last_date = x[3];
  close (state_file_fd);
}

void write_state_file (void) {
  if (binlog_enabled) { return; }
  static int wseq;
  static int wpts;
  static int wqts;
  static int wdate;
  if (wseq >= seq && wpts >= pts && wqts >= qts && wdate >= last_date) { return; }
  int state_file_fd = open (get_state_filename (), O_CREAT | O_RDWR, 0600);
  if (state_file_fd < 0) {
    return;
  }
  int x[6];
  x[0] = STATE_FILE_MAGIC;
  x[1] = 0;
  x[2] = pts;
  x[3] = qts;
  x[4] = seq;
  x[5] = last_date;
  assert (write (state_file_fd, x, 24) == 24);
  close (state_file_fd);
  wseq = seq; wpts = pts; wqts = qts; wdate = last_date;
}

extern peer_t *Peers[];
extern int peer_num;

extern int encr_root;
extern unsigned char *encr_prime;
extern int encr_param_version;
extern int dialog_list_got;

void read_secret_chat_file (void) {
  if (binlog_enabled) { return; }
  int fd = open (get_secret_chat_filename (), O_CREAT | O_RDWR, 0600);
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
    peer_insert_name (P);

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
    insert_encrypted_chat (P);
  }
  if (version >= 1) {
    assert (read (fd, &encr_root, 4) == 4);
    if (encr_root) {
      assert (read (fd, &encr_param_version, 4) == 4);
      encr_prime = talloc (256);
      assert (read (fd, encr_prime, 256) == 256);
    }
  }
  close (fd);
}

void write_secret_chat_file (void) {
  if (binlog_enabled) { return; }
  int fd = open (get_secret_chat_filename (), O_CREAT | O_RDWR, 0600);
  if (fd < 0) {
    return;
  }
  int x[2];
  x[0] = SECRET_CHAT_FILE_MAGIC;
  x[1] = 1;
  assert (write (fd, x, 8) == 8);
  int i;
  int cc = 0;
  for (i = 0; i < peer_num; i++) if (get_peer_type (Peers[i]->id) == PEER_ENCR_CHAT) {
    if (Peers[i]->encr_chat.state != sc_none && Peers[i]->encr_chat.state != sc_deleted) {
      cc ++;
    }
  }
  assert (write (fd, &cc, 4) == 4);
  for (i = 0; i < peer_num; i++) if (get_peer_type (Peers[i]->id) == PEER_ENCR_CHAT) {
    if (Peers[i]->encr_chat.state != sc_none && Peers[i]->encr_chat.state != sc_deleted) {
      int t = get_peer_id (Peers[i]->id);
      assert (write (fd, &t, 4) == 4);
      t = Peers[i]->flags;
      assert (write (fd, &t, 4) == 4);
      t = strlen (Peers[i]->print_name);
      assert (write (fd, &t, 4) == 4);
      assert (write (fd, Peers[i]->print_name, t) == t);

      assert (write (fd, &Peers[i]->encr_chat.state, 4) == 4);

      assert (write (fd, &Peers[i]->encr_chat.user_id, 4) == 4);
      assert (write (fd, &Peers[i]->encr_chat.admin_id, 4) == 4);
      assert (write (fd, &Peers[i]->encr_chat.ttl, 4) == 4);
      assert (write (fd, &Peers[i]->encr_chat.access_hash, 8) == 8);
      if (Peers[i]->encr_chat.state != sc_waiting) {
        assert (write (fd, Peers[i]->encr_chat.g_key, 256) == 256);
      }
      if (Peers[i]->encr_chat.state != sc_waiting) {
        assert (write (fd, Peers[i]->encr_chat.nonce, 256) == 256);
      }
      assert (write (fd, Peers[i]->encr_chat.key, 256) == 256);
      assert (write (fd, &Peers[i]->encr_chat.key_fingerprint, 8) == 8);
    }
  }
  assert (write (fd, &encr_root, 4) == 4);
  if (encr_root) {
    assert (write (fd, &encr_param_version, 4) == 4);
    assert (write (fd, encr_prime, 256) == 256);
  }
  close (fd);
}

extern int max_chat_size;
int mcs (void) {
  return max_chat_size;
}

extern int difference_got;
int dgot (void) {
  return difference_got;
}
int dlgot (void) {
  return dialog_list_got;
}

int readline_active;
int new_dc_num;
int wait_dialog_list;

/**
 * Discover the network and authorise with all data centers
 */
void network_connect (void) {
  verbosity = 0;
  on_start ();
  if (binlog_enabled) {
    double t = get_double_time ();
    logprintf ("replay log start\n");
    replay_log ();
    logprintf ("replay log end in %lf seconds\n", get_double_time () - t);
    write_binlog ();
    #ifdef USE_LUA
      lua_binlog_end ();
    #endif
  } else {
    read_auth_file ();
  }
  logprintf("update prompt()\n");
  update_prompt ();
  logprintf("update prompt() done... \n");

  assert (DC_list[dc_working_num]);
  if (!DC_working || !DC_working->auth_key_id) {
//  if (auth_state == 0) {
    logprintf("No working DC or not start_loopd.\n");
    DC_working = DC_list[dc_working_num];
    assert (!DC_working->auth_key_id);
    dc_authorize (DC_working);
    assert (DC_working->auth_key_id);
    auth_state = 100;
    write_auth_file ();
    logprintf("Authorized DataCentre: auth_key_id: %lld \n", DC_working->auth_key_id);
  }

  if (verbosity) {
    logprintf ("Requesting info about DC...\n");
  }
  do_help_get_config ();
  logprintf("net_loop\n");
  net_loop (0, mcs);
  logprintf("net_loop done...\n");
  if (verbosity) {
    logprintf ("DC_info: %d new DC got\n", new_dc_num);
  }
  int i;
  for (i = 0; i <= MAX_DC_NUM; i++) if (DC_list[i] && !DC_list[i]->auth_key_id) {
    logprintf("DataCentre %d not start_loopd, authorizing...\n", i);
    dc_authorize (DC_list[i]);
    assert (DC_list[i]->auth_key_id);
    write_auth_file ();
    logprintf("DataCentre start_loopd, key id: %lld\n", DC_list[i]->auth_key_id);
  }
}

/**
 * Return if the given phone is registered
 */
int network_phone_is_registered() {
	int res = do_auth_check_phone (default_username);
	assert(res >= 0);
	return res;
}


/**
 * Return if the current client is registered.
 */
int network_client_is_registered() {
  return !(auth_state == 100 || !(DC_working->has_auth));
}

/**
 * Request a verification for the given client, by sending
 *  a code to the current phone number
 */
char* network_request_registration () 
{
    return do_send_code (default_username);
}

/**
 * Verify the phone, by providing the code and the real name
 *
 * NOTE: This should be called when the phone number was previously
 * unknown to the telegram network.
 */
int network_verify_phone_registration(char* code, char *firstname, char *lastname) 
{
    if (do_send_code_result_auth (code, firstname, lastname) >= 0) {
      auth_state = 300;
	  return 1;
    }
	return 0;
}

/**
 * Verify the current client by providing the given code 
 */
int network_verify_registration(const char *code, const char *sms_hash) 
{
  logprintf("telegram: pointer - code: %p, hash: %p\n", code, sms_hash);
  logprintf("telegram: string  - code: %s, hash: %s\n", code, sms_hash);
  if (do_send_code_result (code, sms_hash) >= 0) {
    logprintf ("Authentication successfull, state = 300\n");
    auth_state = 300;
	return 1;
  }
  return 0;
}

int start_loop (char* code, char* auth_mode) {
  logprintf("Calling start_loop()\n");
  logprintf("auth_state %i\n", auth_state);
  if (auth_state == 100 || !(DC_working->has_auth)) {
    logprintf("auth_state == 100 || !(DC_working->has_auth)");
    int res = do_auth_check_phone (default_username);
    assert (res >= 0);
    logprintf ("%s\n", res > 0 ? "phone registered" : "phone not registered");
    if (res > 0 && !register_mode) {
      // Register Mode 1
	  logprintf ("Register Mode 1\n");
      if (code) {
	    /*
        if (do_send_code_result (code) >= 0) {
          logprintf ("Authentication successfull, state = 300\n");
          auth_state = 300;
        }
		*/
      } else {
	      logprintf("No code given, attempting to register\n");
          // Send Code
		  logprintf ("auth mode %s\n", auth_mode);
		  /*
          if (strcmp(TELEGRAM_AUTH_MODE_SMS"sms", auth_mode)) {
		  */
              do_send_code (default_username);
              logprintf ("Code from sms (if you did not receive an SMS and want to be called, type \"call\"): ");
			  logprintf("storing current state in auth file...\n");
    		  write_auth_file ();
			  logprintf("exitting...\n");
			  return 0;
			  /*
          } else {
              logprintf ("You typed \"call\", switching to phone system.\n");
              do_phone_call (default_username);
              logprintf ("Calling you!");
          }
		  */
      }
    } else {
      logprintf ("User is not registered. Do you want to register? [Y/n] ");
      logprintf ("ERROR THIS IS NOT POSSIBLE!\n");
	  return 1;
      // Register Mode 2
      // TODO: Requires first and last name, decide how to handle this.
      //    - We need some sort of switch between registration modes
      //    - When this mode is selected First and Last name should be added to the form
      // Currently Requires Manuel Entry in Terminal.
	  /*
      size_t size;
      char *first_name;
      logprintf ("First name: ");
      if (net_getline (&first_name, &size) == -1) {
        perror ("getline()");
        exit (EXIT_FAILURE);
      }
      char *last_name;
      logprintf ("Last name: ");
      if (net_getline (&last_name, &size) == -1) {
        perror ("getline()");
        exit (EXIT_FAILURE);
      }

      int dc_num = do_get_nearest_dc ();
      assert (dc_num >= 0 && dc_num <= MAX_DC_NUM && DC_list[dc_num]);
      dc_working_num = dc_num;
      DC_working = DC_list[dc_working_num];

      if (*code) {
         if (do_send_code_result_auth (code, first_name, last_name) >= 0) {
             auth_state = 300;
         }
      } else {
         if (strcmp(TELEGRAM_AUTH_MODE_SMS, auth_mode)) {
             do_send_code (default_username);
             logprintf ("Code from sms (if you did not receive an SMS and want to be called, type \"call\"): ");
         } else {
             logprintf ("You typed \"call\", switching to phone system.\n");
             do_phone_call (default_username);
             logprintf ("Calling you! Code: ");
         }
      }
	  */
    }
  }
  logprintf("Authentication done\n");

  int i;
  for (i = 0; i <= MAX_DC_NUM; i++) if (DC_list[i] && !DC_list[i]->has_auth) {
    do_export_auth (i);
    do_import_auth (i);
    bl_do_dc_signed (i);
    write_auth_file ();
  }
  write_auth_file ();

  fflush (stdout);
  fflush (stderr);

  read_state_file ();
  read_secret_chat_file ();

  set_interface_callbacks ();

  do_get_difference ();
  net_loop (0, dgot);
  #ifdef USE_LUA
    lua_diff_end ();
  #endif
  send_all_unsent ();

  do_get_dialog_list ();
  if (wait_dialog_list) {
    dialog_list_got = 0;
    net_loop (0, dlgot);
  }

  return 0; //main_loop ();
}

/**
 * Do what loop does, but use input from arguments instead of prompting
 * the user
 *
 * When authentication is not yet done, request authenticatino code and exit
 * with 0. In all other cases exit with 1.
 */
int loop_auto(char *username, char *code, char* auth_mode) {
    network_connect();
    set_default_username (username);
    return start_loop(code, auth_mode);
}

int loop (void) {
    network_connect();
    return start_loop(NULL, NULL);
}
