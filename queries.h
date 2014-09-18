/*
    This file is part of telegram-client.

    struct telegram-client is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    struct telegram-client is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this telegram-client.  If not, see <http://www.gnu.org/licenses/>.

    Copyright Vitaly Valtman 2013
*/
#ifndef __QUERIES_H__
#define __QUERIES_H__

#pragma once
#include "structures.h"

struct telegram;
struct encr_video;
struct document;
struct secret_chat;
struct tree_query;
struct tree_timer;

#define QUERY_ACK_RECEIVED 1

struct query;
struct query_methods {
  int (*on_answer)(struct query *q);
  int (*on_error)(struct query *q, int error_code, int len, char *error);
  int (*on_timeout)(struct query *q);
};

struct event_timer {
  double timeout;
  int (*alarm)(void *self);
  void *self;
};

struct query {
  long long msg_id;
  int data_len;
  int flags;
  int seq_no;
  void *data;
  struct query_methods *methods;
  struct event_timer ev;
  struct dc *DC;
  struct session *session;
  void *extra;
};


struct query *send_query (struct telegram *instance, struct dc *DC, int len, void *data, struct query_methods *methods, void *extra);
void query_ack (struct telegram *instance, long long id);
void query_error (struct telegram *instance, long long id);
void query_result (struct telegram *instance, long long id);
void query_restart (struct telegram *instance, long long id);

void insert_event_timer (struct telegram *instance, struct event_timer *ev);
void remove_event_timer (struct telegram *instance, struct event_timer *ev);
double next_timer_in (struct telegram *instance);
void work_timers (struct telegram *instance);

extern struct query_methods help_get_config_methods;

void do_send_code (struct telegram *instance, const char *user);
void do_phone_call (struct telegram *instance, const char *user);
void do_send_code_result (struct telegram *instance, const char *code);
double get_double_time (void);

void do_update_contact_list (struct telegram *instance);
union user_chat;
void do_send_message (struct telegram *instance, peer_id_t id, const char *msg, int len);
void do_send_text (struct telegram *instance, peer_id_t id, char *file);
void do_get_history (struct telegram *instance, peer_id_t id, int limit);
void do_get_dialog_list (struct telegram*);
void do_get_dialog_list_ex (struct telegram*);
void do_send_photo (struct telegram *instance, int type, peer_id_t to_id, char *file_name);
void do_get_chat_info (struct telegram *instance, peer_id_t id);
void do_get_user_list_info_silent (struct telegram *instance, int num, int *list);
void do_get_user_info (struct telegram *instance, peer_id_t id);
void do_forward_message (struct telegram *instance, peer_id_t id, int n);
void do_rename_chat (struct telegram *instance, peer_id_t id, char *name);
void do_load_encr_video (struct telegram *instance, struct encr_video *V, int next);
void do_create_encr_chat_request (struct telegram *instance, int user_id);
void do_create_secret_chat (struct telegram *instance, peer_id_t id);
void do_create_group_chat (struct telegram *instance, peer_id_t id, char *chat_topic);
void do_get_suggested (struct telegram*);

struct photo;
struct video;
void do_load_photo (struct telegram *instance, struct photo *photo, int next);
void do_load_video_thumb (struct telegram *instance, struct video *video, int next);
void do_load_audio (struct telegram *instance, struct video *V, int next);
void do_load_video (struct telegram *instance, struct video *V, int next);
void do_load_document (struct telegram *instance, struct document *V, int next);
void do_load_document_thumb (struct telegram *instance, struct document *video, int next);
void do_help_get_config (struct telegram *instance);
void do_auth_check_phone (struct telegram *instance, const char *user);
void do_get_nearest_dc (struct telegram*);
void do_send_code_result_auth (struct telegram *instance, const char *code, const char *first_name, const char *last_name);
void do_import_auth (struct telegram *instance, int num);
void do_export_auth (struct telegram *instance, int num);
void do_add_contact (struct telegram *instance, const char *phone, int phone_len, const char *first_name, int first_name_len, const char *last_name, int last_name_len, int force);
void do_msg_search (struct telegram *instance, peer_id_t id, int from, int to, int limit, const char *s);
void do_accept_encr_chat_request (struct telegram *instance, struct secret_chat *E);
void do_get_difference (struct telegram*, int sync_from_start);
void do_mark_read (struct telegram *instance, peer_id_t id);
void do_visualize_key (struct binlog *bl, peer_id_t id);
void do_create_keys_end (struct telegram *, struct secret_chat *U);

void do_add_user_to_chat (struct telegram *instance, peer_id_t chat_id, peer_id_t id, int limit);
void do_del_user_from_chat (struct telegram *instance, peer_id_t chat_id, peer_id_t id);
void do_update_status (struct telegram *instance, int online);
void do_contacts_search (struct telegram *instance, int limit, const char *s);
void do_send_msg (struct telegram *instance, struct message *M);
void do_delete_msg (struct telegram *instance, long long id);
void do_restore_msg (struct telegram *instance, long long id);

// For binlog

int get_dh_config_on_answer (struct query *q);
void fetch_dc_option (struct telegram *instance);
void free_queries (struct telegram *instance);
void free_timers (struct telegram *instance);
#endif

const char *get_last_err();

int all_queries_done();
