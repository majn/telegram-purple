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

    Copyright Matthias Jentsch, Vitaly Valtman, Christopher Althaus, Markus Endres 2014
*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>

#include <tgl.h>
#include <tgl-binlog.h>

#include <glib.h>
#include <request.h>

#include <telegram-purple.h>
#include <msglog.h>
#include <tgp-2prpl.h>

#define DC_SERIALIZED_MAGIC 0x868aa81d
#define STATE_FILE_MAGIC 0x28949a93
#define SECRET_CHAT_FILE_MAGIC 0x37a1988a


void read_state_file (struct tgl_state *TLS) {
  char *name = 0;
  if (asprintf (&name, "%s/%s", TLS->base_path, "state") < 0) {
    return;
  }

  int state_file_fd = open (name, O_CREAT | O_RDWR, 0600);
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
}

void write_state_file (struct tgl_state *TLS) {
  int wseq;
  int wpts;
  int wqts;
  int wdate;
  wseq = TLS->seq; wpts = TLS->pts; wqts = TLS->qts; wdate = TLS->date;
  
  char *name = 0;
  if (asprintf (&name, "%s/%s", TLS->base_path, "state") < 0) {
    return;
  }

  int state_file_fd = open (name, O_CREAT | O_RDWR, 0600);
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
}

void write_dc (struct tgl_dc *DC, void *extra) {
  int auth_file_fd = *(int *)extra;
  if (!DC) { 
    int x = 0;
    assert (write (auth_file_fd, &x, 4) == 4);
    return;
  } else {
    int x = 1;
    assert (write (auth_file_fd, &x, 4) == 4);
  }

  assert (DC->has_auth);

  assert (write (auth_file_fd, &DC->port, 4) == 4);
  int l = strlen (DC->ip);
  assert (write (auth_file_fd, &l, 4) == 4);
  assert (write (auth_file_fd, DC->ip, l) == l);
  assert (write (auth_file_fd, &DC->auth_key_id, 8) == 8);
  assert (write (auth_file_fd, DC->auth_key, 256) == 256);
}

void write_auth_file (struct tgl_state *TLS) {
  char *name = 0;
  if (asprintf (&name, "%s/%s", TLS->base_path, "auth") < 0) {
    return;
  }
  int auth_file_fd = open (name, O_CREAT | O_RDWR, 0600);
  free (name);
  if (auth_file_fd < 0) { return; }
  int x = DC_SERIALIZED_MAGIC;
  assert (write (auth_file_fd, &x, 4) == 4);
  assert (write (auth_file_fd, &TLS->max_dc_num, 4) == 4);
  assert (write (auth_file_fd, &TLS->dc_working_num, 4) == 4);

  tgl_dc_iterator_ex (TLS, write_dc, &auth_file_fd);

  assert (write (auth_file_fd, &TLS->our_id, 4) == 4);
  close (auth_file_fd);
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

  //bl_do_add_dc (id, ip, l, port, auth_key_id, auth_key);
  bl_do_dc_option (TLS, id, 2, "DC", l, ip, port);
  bl_do_set_auth_key_id (TLS, id, auth_key);
  bl_do_dc_signed (TLS, id);
}

void empty_auth_file (struct tgl_state *TLS) {
  if (TLS->test_mode) {
    bl_do_dc_option (TLS, 1, 0, "", strlen (TG_SERVER_TEST_1), TG_SERVER_TEST_1, 443);
    bl_do_dc_option (TLS, 2, 0, "", strlen (TG_SERVER_TEST_2), TG_SERVER_TEST_2, 443);
    bl_do_dc_option (TLS, 3, 0, "", strlen (TG_SERVER_TEST_3), TG_SERVER_TEST_3, 443);
    bl_do_set_working_dc (TLS, TG_SERVER_TEST_DEFAULT);
  } else {
    bl_do_dc_option (TLS, 1, 0, "", strlen (TG_SERVER_1), TG_SERVER_1, 443);
    bl_do_dc_option (TLS, 2, 0, "", strlen (TG_SERVER_2), TG_SERVER_2, 443);
    bl_do_dc_option (TLS, 3, 0, "", strlen (TG_SERVER_3), TG_SERVER_3, 443);
    bl_do_dc_option (TLS, 4, 0, "", strlen (TG_SERVER_4), TG_SERVER_4, 443);
    bl_do_dc_option (TLS, 5, 0, "", strlen (TG_SERVER_5), TG_SERVER_5, 443);
    bl_do_set_working_dc (TLS, TG_SERVER_DEFAULT);;
  }
}

void read_auth_file (struct tgl_state *TLS) {
  char *name = 0;
  if (asprintf (&name, "%s/%s", TLS->base_path, "auth") < 0) {
    return;
  }
  int auth_file_fd = open (name, O_CREAT | O_RDWR, 0600);
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
    bl_do_set_our_id (TLS, our_id);
  }
  close (auth_file_fd);
}

void telegram_export_authorization (struct tgl_state *TLS);
void export_auth_callback (struct tgl_state *TLS, void *extra, int success) {
  assert (success);
  telegram_export_authorization (TLS);
}

void telegram_export_authorization (struct tgl_state *TLS) {
  int i;
  for (i = 0; i <= TLS->max_dc_num; i++) if (TLS->DC_list[i] && !tgl_signed_dc (TLS, TLS->DC_list[i])) {
    tgl_do_export_auth (TLS, i, export_auth_callback, (void*)(long)TLS->DC_list[i]);    
    return;
  }
  write_auth_file (TLS);
  on_ready (TLS);
}

static void request_code (struct tgl_state *TLS);
static void request_name_and_code (struct tgl_state *TLS);
static void code_receive_result (struct tgl_state *TLS, void *extra, int success, struct tgl_user *U) {
  if (!success) {
    debug ("Bad code...\n");
    request_code (TLS);
  } else {
    telegram_export_authorization (TLS);
  }
}

static void code_auth_receive_result (struct tgl_state *TLS, void *extra, int success, struct tgl_user *U) {
  if (!success) {
    debug ("Bad code...\n");
    request_name_and_code (TLS);
  } else {
    telegram_export_authorization (TLS);
  }
}

void request_code_entered (gpointer data, const gchar *code) {
  struct tgl_state *TLS = data;
  telegram_conn *conn = TLS->ev_base;
  char const *username = purple_account_get_username(conn->pa);
  tgl_do_send_code_result (TLS, username, conn->hash, code, code_receive_result, 0) ;
}

static void request_code_canceled (gpointer data) {
  struct tgl_state *TLS = data;
  telegram_conn *conn = TLS->ev_base;

  purple_connection_error_reason(conn->gc,
      PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED, "registration canceled");
}

static void request_name_code_entered (PurpleConnection* gc, PurpleRequestFields* fields) {
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  struct tgl_state *TLS = conn->TLS;
  char const *username = purple_account_get_username(conn->pa);
  
  const char* first = purple_request_fields_get_string(fields, "first_name");
  const char* last = purple_request_fields_get_string(fields, "last_name");
  const char* code = purple_request_fields_get_string(fields, "code");
  if (!first || !last || !code) {
    request_name_and_code (TLS);
    return;
  }
  
  tgl_do_send_code_result_auth (TLS, username, conn->hash, code, first, last, code_auth_receive_result, NULL);
}

static void request_code (struct tgl_state *TLS) {
  debug ("Client is not registered, registering...\n");
  telegram_conn *conn = TLS->ev_base;
  int compat = purple_account_get_bool (tg_get_acc(TLS), "compat-verification", 0);
  
  if (compat || ! purple_request_input (conn->gc, "Telegram Code", "Enter Telegram Code",
        "Telegram wants to verify your identity, please enter the code, that you have received via SMS.",
        NULL, 0, 0, "code", "OK", G_CALLBACK(request_code_entered), "Cancel",
        G_CALLBACK(request_code_canceled), conn->pa, NULL, NULL, TLS)) {

    // purple request API is not available, so we create a new conversation (the Telegram system
    // account "7770000") to prompt the user for the code
          
    conn->in_fallback_chat = 1;
    purple_connection_set_state (conn->gc, PURPLE_CONNECTED);
    PurpleConversation *conv = purple_conversation_new (PURPLE_CONV_TYPE_IM, conn->pa, "777000");
    purple_conversation_write (conv, "777000", "What is your SMS verification code?",
          PURPLE_MESSAGE_RECV | PURPLE_MESSAGE_SYSTEM, 0);
  }
}

static void request_name_and_code (struct tgl_state *TLS) {
  debug ("Phone is not registered, registering...\n");

  telegram_conn *conn = TLS->ev_base;

  PurpleRequestFields* fields = purple_request_fields_new();
  PurpleRequestField* field = 0;

  PurpleRequestFieldGroup* group = purple_request_field_group_new("Registration");
  field = purple_request_field_string_new("first_name", "First Name", "", 0);
  purple_request_field_group_add_field(group, field);
  field = purple_request_field_string_new("last_name", "Last Name", "", 0);
  purple_request_field_group_add_field(group, field);
  purple_request_fields_add_group(fields, group);

  group = purple_request_field_group_new("Authorization");
  field = purple_request_field_string_new("code", "Telegram Code", "", 0);
  purple_request_field_group_add_field(group, field);
  purple_request_fields_add_group(fields, group);

  if (!purple_request_fields (conn->gc, "Register", "Please register your phone number.", NULL, fields, "Ok",
    G_CALLBACK( request_name_code_entered ), "Cancel", NULL, conn->pa, NULL, NULL, conn->gc)) {
    // purple_request API not available
    const char *error = "Phone number is not registered, please register your phone on a different client.";
    purple_connection_error_reason (conn->gc, PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED, error);
    purple_notify_error(_telegram_protocol, "Not Registered", "Not Registered", error);
  }
}

static void sign_in_callback (struct tgl_state *TLS, void *extra, int success, int registered, const char *mhash) {
  assert (success); // TODO proper error handle
  telegram_conn *conn = TLS->ev_base;
  conn->hash = strdup (mhash);

  if (registered) {
    request_code (TLS);
  } else {
    request_name_and_code (TLS);
  }
}

static void telegram_send_sms (struct tgl_state *TLS) {
  if (tgl_signed_dc (TLS, TLS->DC_working)) {
    telegram_export_authorization (TLS);
    return;
  }
  telegram_conn *conn = TLS->ev_base;
  char const *username = purple_account_get_username(conn->pa);
  tgl_do_send_code (TLS, username, sign_in_callback, 0);
}

static int all_authorized (struct tgl_state *TLS) {
  int i;
  for (i = 0; i <= TLS->max_dc_num; i++) if (TLS->DC_list[i]) {
    if (!tgl_authorized_dc (TLS, TLS->DC_list[i])) {
      return 0;
    }
  }
  return 1;
}

static int check_all_authorized (gpointer arg) {
  struct tgl_state *TLS = arg;
  if (all_authorized (TLS)) {
    telegram_send_sms (TLS);    
    return FALSE;
  } else {
    return TRUE;
  }
}
    
void telegram_login (struct tgl_state *TLS) {    
  read_auth_file (TLS);
  read_state_file (TLS);
  if (all_authorized (TLS)) {
    telegram_send_sms (TLS);
    return;
  }
  purple_timeout_add (100, check_all_authorized, TLS);
}

PurpleConversation *chat_show (PurpleConnection *gc, int id) {
  debug ("show chat");
  telegram_conn *conn = purple_connection_get_protocol_data(gc);
  
  PurpleConversation *convo = purple_find_chat(gc, id);
  if (! convo) {
    gchar *name = g_strdup_printf ("%d", id);
    if (! g_hash_table_contains (conn->joining_chats, name)) {
      g_hash_table_insert(conn->joining_chats, name, 0);
      tgl_do_get_chat_info (conn->TLS, TGL_MK_CHAT(id), 0, on_chat_get_info, NULL);
    } else {
      g_free(name);
    }
  }
  return convo;
}

int chat_add_message (struct tgl_state *TLS, struct tgl_message *M, char *text) {
  telegram_conn *conn = TLS->ev_base;
  
  if (chat_show (conn->gc, tgl_get_peer_id (M->to_id))) {
    p2tgl_got_chat_in(TLS, M->to_id, M->from_id, text ? text : M->message,
                      M->service ? PURPLE_MESSAGE_SYSTEM : PURPLE_MESSAGE_RECV, M->date);
    return 1;
  } else {
    // add message once the chat was initialised
    struct message_text *mt = malloc (sizeof (*mt));
    mt->M = M;
    mt->text = text ? g_strdup (text) : text;
    g_queue_push_tail (conn->new_messages, mt);
    return 0;
  }
}

void chat_add_all_users (PurpleConversation *pc, struct tgl_chat *chat) {
  struct tgl_chat_user *curr =  chat->user_list;
  if (!curr) {
    warning ("add_all_users_to_chat: chat contains no user list, cannot add users\n.");
    return;
  }
  
  int i;
  for (i = 0; i < chat->user_list_size; i++) {
    struct tgl_chat_user *uid = (curr + i);
    int flags = (chat->admin_id == uid->user_id ? PURPLE_CBFLAGS_FOUNDER : PURPLE_CBFLAGS_NONE);
    p2tgl_conv_add_user(pc, *uid, NULL, flags, 0);
  }
}
