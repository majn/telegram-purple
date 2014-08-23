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

#include "net.h"
#include "mtproto-client.h"
#include "queries.h"
#include "telegram.h"
#include "loop.h"
#include "binlog.h"
#include "lua-tg.h"

//

#include "purple-plugin/telegram-purple.h"
//



extern char *auth_token;
int test_dc = 0;
int default_dc_num;
extern int binlog_enabled;

extern int unknown_user_list_pos;
extern int unknown_user_list[];
int register_mode;
extern int safe_quit;

int unread_messages;
void got_it (char *line, int len);
/*
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
	// Ensure that all connections are active?
    int x = connections_make_poll_array (fds + cc, 101 - cc) + cc;
    double timer = next_timer_in ();

	// Wait until file descriptors are ready
    if (timer > 1000) { timer = 1000; }
    if (poll (fds, x, timer) < 0) {
	  logprintf("poll returned -1, wait a little bit.\n");
      work_timers ();
      continue;
    }

	// Execute all timers that are currently due
    work_timers ();
	
	// ?
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

	// 
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
*/

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

/*
int net_getline (char **s, size_t *l) {
  fflush (stdout);
//  rl_already_prompted = 1;
  got_it_ok = 0;
  _s = s;
  _l = l;
//  rl_callback_handler_install (0, got_it);
  //net_loop (2, is_got_it);
  return 0;
}
*/

int ret1 (void) { return 0; }

/*
int main_loop (void) {
  net_loop (1, ret1);
  return 0;
}
*/


//struct dc *DC_list[MAX_DC_ID + 1];
//struct dc *DC_working;
//int dc_working_num;
//int auth_state;
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

int our_id;

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
  alloc_dc (state.DC_list, 1, tstrdup (test_dc ? TG_SERVER_TEST : TG_SERVER), 443);
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

int pts, qts, seq, last_date;

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
  /*
  static int wseq;
  static int wpts;
  static int wqts;
  static int wdate;
  if (wseq >= seq && wpts >= pts && wqts >= qts && wdate >= last_date) { return; }
  */
  int state_file_fd = open (filename /*get_state_filename ()*/, O_CREAT | O_RDWR, 0600);
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
  //wseq = seq; wpts = pts; wqts = qts; wdate = last_date;
}

extern peer_t *Peers[];
extern int peer_num;

extern int encr_root;
extern unsigned char *encr_prime;
extern int encr_param_version;
extern int dialog_list_got;

// TODO: Refactor 
void read_secret_chat_file (const char *file) {

  if (binlog_enabled) { return; }
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

// TODO: Refactor
void write_secret_chat_file (const char *filename) {
  if (binlog_enabled) { return; }
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
void network_connect (struct telegram *instance) {
  verbosity = 0;
  on_start ();
  // will return empty default values on empty files
  instance->auth = read_auth_file ("/home/dev-jessie/.telegram/auth_file");
  instance->proto = read_state_file ("/home/dev-jessie/.telegram/auth_file");

  struct dc *DC_list = (struct dc*)instance->auth.DC_list;
  struct dc *DC_working = NULL;

  assert (DC_list[dc_working_num]);
  if (!DC_working || !DC_working->auth_key_id) {
//  if (auth_state == 0) {
    logprintf("No working DC or not start_loopd.\n");
    DC_working = &DC_list[instance->auth.dc_working_num];
    assert (!DC_working->auth_key_id);
    dc_authorize (DC_working);
    assert (DC_working->auth_key_id);
    auth_state = 100;
    write_auth_file (instance->auth);
    logprintf("Authorized DataCentre: auth_key_id: %lld \n", DC_working->auth_key_id);
  } else {
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
  // read saved connection state
  //read_state_file ();
  read_secret_chat_file ();
}
 */

/**
 * Return if the given phone is registered
 */
 /*
int network_phone_is_registered() {
	int res = do_auth_check_phone (default_username);
	assert(res >= 0);
	return res;
}
*/


/**
 * Verify the phone number by providing the sms_code and the real name
 *
 * NOTE: This should be called when the phone number was previously
 * unknown to the telegram network.
 */
 /*
int network_verify_phone_registration(const char* code, const char *sms_hash, 
	const char *first ,const char *last) 
{
    logprintf("Registering with code:%s, hash:%s, first:%s, last:%s\n", code, sms_hash, 
		first, last);
	return 0;
}
*/

/**
 * Export current authentication state to all known data centers.
 */
//void network_export_registration()
//{
//    int i;
//    for (i = 0; i <= MAX_DC_NUM; i++) if (DC_list[i] && !DC_list[i]->has_auth) {
//        do_export_auth (i);
//        do_import_auth (i);
//        bl_do_dc_signed (i);
//        write_auth_file ();
//    }
//    write_auth_file ();
//    fflush (stdout);
//    fflush (stderr);
//}

//int start_loop (char* code, char* auth_mode) {
//  logprintf("Calling start_loop()\n");
//  logprintf("auth_state %i\n", auth_state);
//  if (auth_state == 100 || !(DC_working->has_auth)) {
//    logprintf("auth_state == 100 || !(DC_working->has_auth)");
//    int res = do_auth_check_phone (default_username);
//    assert (res >= 0);
//    logprintf ("%s\n", res > 0 ? "phone registered" : "phone not registered");
//    if (res > 0 && !register_mode) {
//      // Register Mode 1
//	  logprintf ("Register Mode 1\n");
//      if (code) {
//	    /*
//        if (do_send_code_result (code) >= 0) {
//          logprintf ("Authentication successfull, state = 300\n");
//          auth_state = 300;
//        }
//		*/
//      } else {
//	      logprintf("No code given, attempting to register\n");
//          // Send Code
//		  logprintf ("auth mode %s\n", auth_mode);
//		  /*
//          if (strcmp(TELEGRAM_AUTH_MODE_SMS"sms", auth_mode)) {
//		  */
//              do_send_code (default_username);
//              logprintf ("Code from sms (if you did not receive an SMS and want to be called, type \"call\"): ");
//			  logprintf("storing current state in auth file...\n");
//    		  write_auth_file ();
//			  logprintf("exitting...\n");
//			  return 0;
//			  /*
//          } else {
//              logprintf ("You typed \"call\", switching to phone system.\n");
//              do_phone_call (default_username);
//              logprintf ("Calling you!");
//          }
//		  */
//      }
//    } else {
//      logprintf ("User is not registered. Do you want to register? [Y/n] ");
//      logprintf ("ERROR THIS IS NOT POSSIBLE!\n");
//	  return 1;
//      // Register Mode 2
//      // TODO: Requires first and last name, decide how to handle this.
//      //    - We need some sort of switch between registration modes
//      //    - When this mode is selected First and Last name should be added to the form
//      // Currently Requires Manuel Entry in Terminal.
//      size_t size;
//      char *first_name;
//      logprintf ("First name: ");
//      if (net_getline (&first_name, &size) == -1) {
//        perror ("getline()");
//        exit (EXIT_FAILURE);
//      }
//      char *last_name;
//      logprintf ("Last name: ");
//      if (net_getline (&last_name, &size) == -1) {
//        perror ("getline()");
//        exit (EXIT_FAILURE);
//      }
//
//      int dc_num = do_get_nearest_dc ();
//      assert (dc_num >= 0 && dc_num <= MAX_DC_NUM && DC_list[dc_num]);
//      dc_working_num = dc_num;
//      DC_working = DC_list[dc_working_num];
//
//      if (*code) {
//         if (do_send_code_result_auth (code, "-", first_name, last_name) >= 0) {
//             auth_state = 300;
//         }
//      } else {
//         if (strcmp(TELEGRAM_AUTH_MODE_SMS, auth_mode)) {
//             do_send_code (default_username);
//             logprintf ("Code from sms (if you did not receive an SMS and want to be called, type \"call\"): ");
//         } else {
//             logprintf ("You typed \"call\", switching to phone system.\n");
//             do_phone_call (default_username);
//             logprintf ("Calling you! Code: ");
//         }
//      }
//    }
//  }
//  logprintf("Authentication done\n");
//
//  int i;
//  for (i = 0; i <= MAX_DC_NUM; i++) if (DC_list[i] && !DC_list[i]->has_auth) {
//    do_export_auth (i);
//    do_import_auth (i);
//    bl_do_dc_signed (i);
//    write_auth_file ();
//  }
//  write_auth_file ();
//
//  fflush (stdout);
//  fflush (stderr);
//
//  // read saved connection state
//  read_state_file ();
//  read_secret_chat_file ();
//	
//  // callbacks for interface functions
//  set_interface_callbacks ();
//
//  do_get_difference ();
//  net_loop (0, dgot);
//  #ifdef USE_LUA
//    lua_diff_end ();
//  #endif
//  send_all_unsent ();
//
//  do_get_dialog_list ();
//  if (wait_dialog_list) {
//    dialog_list_got = 0;
//    net_loop (0, dlgot);
//  }
//
//  return 0; //main_loop ();
//}

/*
int loop (void) {
    network_connect();
    return start_loop(NULL, NULL);
}
*/
