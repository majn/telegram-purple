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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <openssl/bn.h>

#include "binlog.h"
#include "net.h"
#include "include.h"
#include "mtproto-client.h"
#include "telegram.h"

#include <openssl/sha.h>

#define MAX_LOG_EVENT_SIZE (1 << 17)

int binlog_enabled = 0;

void *alloc_log_event (struct binlog *bl, int l UU) {
  return bl->binlog_buffer;
}

void replay_log_event (struct telegram *instance) {
  struct mtproto_connection *self = instance->connection;
  struct binlog *bl = instance->bl;

  int *start = bl->rptr;
  bl->in_replay_log = 1;
  assert (bl->rptr < bl->wptr);
  int op = *bl->rptr;

  if (verbosity >= 2) {
    logprintf ("log_pos %lld, op 0x%08x\n", bl->binlog_pos, op);
  }

  self->in_ptr = bl->rptr;
  self->in_end = bl->wptr;
  switch (op) {
  case LOG_START:
    bl->rptr ++;
    break;
  case CODE_binlog_dc_option:
    self->in_ptr ++;
    {
      int id = fetch_int (self);
      int l1 = prefetch_strlen (self);
      char *name = fetch_str (self, l1);
      int l2 = prefetch_strlen (self);
      char *ip = fetch_str (self, l2);
      int port = fetch_int (self);
      logprintf ( "id = %d, name = %.*s ip = %.*s port = %d\n", id, l1, name, l2, ip, port);
      alloc_dc (instance->auth.DC_list, id, tstrndup (ip, l2), port);
    }
    bl->rptr = self->in_ptr;
    break;
  case LOG_AUTH_KEY:
    bl->rptr ++;
    {
      int num = *(bl->rptr ++);
      assert (num >= 0 && num <= MAX_DC_ID);
      assert (instance->auth.DC_list[num]);
      instance->auth.DC_list[num]->auth_key_id = *(long long *)bl->rptr;
      bl->rptr += 2;
      memcpy (instance->auth.DC_list[num]->auth_key, bl->rptr, 256);
      bl->rptr += 64;
      instance->auth.DC_list[num]->flags |= 1;
    };
    break;
  case LOG_DEFAULT_DC:
    bl->rptr ++;
    { 
      int num = *(bl->rptr ++);
      assert (num >= 0 && num <= MAX_DC_ID);
      instance->auth.dc_working_num = num;
    }
    break;
  case LOG_OUR_ID:
    bl->rptr ++;
    {
      instance->our_id = *(bl->rptr ++);
    }
    break;
  case LOG_DC_SIGNED:
    bl->rptr ++;
    {
      int num = *(bl->rptr ++);
      assert (num >= 0 && num <= MAX_DC_ID);
      assert (instance->auth.DC_list[num]);
      instance->auth.DC_list[num]->has_auth = 1;
    }
    break;
  case LOG_DC_SALT:
    bl->rptr ++;
    {
      int num = *(bl->rptr ++);
      assert (num >= 0 && num <= MAX_DC_ID);
      assert (instance->auth.DC_list[num]);
      instance->auth.DC_list[num]->server_salt = *(long long *)bl->rptr;
      bl->rptr += 2;
    };
    break;
//  case CODE_user_empty:
//  case CODE_user_self:
//  case CODE_user_contact:
//  case CODE_user_request:
//  case CODE_user_foreign:
  case CODE_user_deleted:
    fetch_alloc_user (self);
    bl->rptr = self->in_ptr;
    break;
  case LOG_DH_CONFIG:
    get_dh_config_on_answer (0);
    bl->rptr = self->in_ptr;
    break;
  case LOG_ENCR_CHAT_KEY:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      struct secret_chat *U = (void *)user_chat_get (bl, id);
      assert (U);
      U->key_fingerprint = *(long long *)bl->rptr;
      bl->rptr += 2;
      memcpy (U->key, bl->rptr, 256);
      bl->rptr += 64;
    };
    break;
  case LOG_ENCR_CHAT_SEND_ACCEPT:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      struct secret_chat *U = (void *)user_chat_get (bl, id);
      assert (U);
      U->key_fingerprint = *(long long *)bl->rptr;
      bl->rptr += 2;
      memcpy (U->key, bl->rptr, 256);
      bl->rptr += 64;
      if (!U->g_key) {
        U->g_key = talloc (256);
      }
      memcpy (U->g_key, bl->rptr, 256);
      bl->rptr += 64;
    };
    break;
  case LOG_ENCR_CHAT_SEND_CREATE:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      struct secret_chat *U = (void *)user_chat_get (bl, id);
      assert (!U || !(U->flags & FLAG_CREATED));
      if (!U) {
        U = talloc0 (sizeof (peer_t));
        U->id = id;
        insert_encrypted_chat (bl, (void *)U);
      }
      U->flags |= FLAG_CREATED;
      U->user_id = *(bl->rptr ++);
      memcpy (U->key, bl->rptr, 256);
      bl->rptr += 64;
      if (!U->print_name) {  
        peer_t *P = user_chat_get (bl, MK_USER (U->user_id));
        if (P) {
          U->print_name = create_print_name (bl, U->id, "!", P->user.first_name, P->user.last_name, 0);
        } else {
          static char buf[100];
          tsnprintf (buf, 99, "user#%d", U->user_id);
          U->print_name = create_print_name (bl, U->id, "!", buf, 0, 0);
        }
        peer_insert_name (bl, (void *)U);
      }
    };
    break;
  case LOG_ENCR_CHAT_DELETED:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      struct secret_chat *U = (void *)user_chat_get (bl, id);
      if (!U) {
        U = talloc0 (sizeof (peer_t));
        U->id = id;
        insert_encrypted_chat (bl, (void *)U);
      }
      U->flags |= FLAG_CREATED;
      U->state = sc_deleted;
    };
    break;
  case LOG_ENCR_CHAT_WAITING:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      struct secret_chat *U = (void *)user_chat_get (bl, id);
      assert (U);
      U->state = sc_waiting;
      U->date = *(bl->rptr ++);
      U->admin_id = *(bl->rptr ++);
      U->user_id = *(bl->rptr ++);
      U->access_hash = *(long long *)bl->rptr;
      bl->rptr += 2;
    };
    break;
  case LOG_ENCR_CHAT_REQUESTED:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      struct secret_chat *U = (void *)user_chat_get (bl, id);
      if (!U) {
        U = talloc0 (sizeof (peer_t));
        U->id = id;
        insert_encrypted_chat (bl, (void *)U);
      }
      U->flags |= FLAG_CREATED;
      U->state = sc_request;
      U->date = *(bl->rptr ++);
      U->admin_id = *(bl->rptr ++);
      U->user_id = *(bl->rptr ++);
      U->access_hash = *(long long *)bl->rptr;
      if (!U->print_name) {  
        peer_t *P = user_chat_get (bl, MK_USER (U->user_id));
        if (P) {
          U->print_name = create_print_name (bl, U->id, "!", P->user.first_name, P->user.last_name, 0);
        } else {
          static char buf[100];
          tsnprintf (buf, 99, "user#%d", U->user_id);
          U->print_name = create_print_name (bl, U->id, "!", buf, 0, 0);
        }
        peer_insert_name (bl, (void *)U);
      }
      bl->rptr += 2;
    };
    break;
  case LOG_ENCR_CHAT_OK:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      struct secret_chat *U = (void *)user_chat_get (bl, id);
      assert (U);
      U->state = sc_ok;
    }
    break;
  case CODE_binlog_new_user:
    self->in_ptr ++;
    {
      peer_id_t id = MK_USER (fetch_int (self));
      peer_t *_U = user_chat_get (bl, id);
      if (!_U) {
        _U = talloc0 (sizeof (*_U));
        _U->id = id;
        insert_user (bl, _U);
      } else {
        assert (!(_U->flags & FLAG_CREATED));
      }
      struct user *U = (void *)_U;
      U->flags |= FLAG_CREATED;
      if (get_peer_id (id) == instance->our_id) {
        U->flags |= FLAG_USER_SELF;
      }
      U->first_name = fetch_str_dup (self);
      U->last_name = fetch_str_dup (self);
      assert (!U->print_name);
      U->print_name = create_print_name (bl, U->id, U->first_name, U->last_name, 0, 0);
      peer_insert_name (bl, (void *)U);
      U->access_hash = fetch_long (self);
      U->phone = fetch_str_dup (self);
      if (fetch_int (self)) {
        U->flags |= FLAG_USER_CONTACT;
      }
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_user_delete:
    bl->rptr ++;
    {
      peer_id_t id = MK_USER (*(bl->rptr ++));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      U->flags |= FLAG_DELETED;
    }
    break;
  case CODE_binlog_set_user_access_token:
    bl->rptr ++;
    {
      peer_id_t id = MK_USER (*(bl->rptr ++));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      U->user.access_hash = *(long long *)bl->rptr;
      bl->rptr += 2;
    }
    break;
  case CODE_binlog_set_user_phone:
    self->in_ptr ++;
    {
      peer_id_t id = MK_USER (fetch_int (self));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      if (U->user.phone) { tfree_str (U->user.phone); }
      U->user.phone = fetch_str_dup (self);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_set_user_friend:
    bl->rptr ++;
    {
      peer_id_t id = MK_USER (*(bl->rptr ++));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      int friend = *(bl->rptr ++);
      if (friend) { U->flags |= FLAG_USER_CONTACT; }
      else { U->flags &= ~FLAG_USER_CONTACT; }
    }
    break;
  case CODE_binlog_user_full_photo:
    self->in_ptr ++;
    {
      peer_id_t id = MK_USER (fetch_int (self));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      if (U->flags & FLAG_HAS_PHOTO) {
        free_photo (&U->user.photo);
      }
      fetch_photo (self, &U->user.photo);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_user_blocked:
    bl->rptr ++;
    {
      peer_id_t id = MK_USER (*(bl->rptr ++));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      U->user.blocked = *(bl->rptr ++);
    }
    break;
  case CODE_binlog_set_user_full_name:
    self->in_ptr ++;
    {
      peer_id_t id = MK_USER (fetch_int (self));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      if (U->user.real_first_name) { tfree_str (U->user.real_first_name); }
      if (U->user.real_last_name) { tfree_str (U->user.real_last_name); }
      U->user.real_first_name = fetch_str_dup (self);
      U->user.real_last_name = fetch_str_dup (self);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_encr_chat_delete:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      peer_t *_U = user_chat_get (bl, id);
      assert (_U);
      struct secret_chat *U = &_U->encr_chat;
      memset (U->key, 0, sizeof (U->key));
      U->flags |= FLAG_DELETED;
      U->state = sc_deleted;
      if (U->nonce) {
        tfree_secure (U->nonce, 256);
        U->nonce = 0;
      }
      if (U->g_key) {
        tfree_secure (U->g_key, 256);
        U->g_key = 0;
      }
    }
    break;
  case CODE_binlog_encr_chat_requested:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      peer_t *_U = user_chat_get (bl, id);
      if (!_U) {
        _U = talloc0 (sizeof (*_U));
        _U->id = id;
        insert_encrypted_chat (bl, _U);
      } else {
        assert (!(_U->flags & FLAG_CREATED));
      }
      struct secret_chat *U = (void *)_U;
      U->access_hash = *(long long *)bl->rptr;
      bl->rptr += 2;
      U->date = *(bl->rptr ++);
      U->admin_id = *(bl->rptr ++);
      U->user_id = *(bl->rptr ++);

      peer_t *Us = user_chat_get (bl, MK_USER (U->user_id));
      assert (!U->print_name);
      if (Us) {
        U->print_name = create_print_name (bl, id, "!", Us->user.first_name, Us->user.last_name, 0);
      } else {
        static char buf[100];
        tsnprintf (buf, 99, "user#%d", U->user_id);
        U->print_name = create_print_name (bl, id, "!", buf, 0, 0);
      }
      peer_insert_name (bl, (void *)U);
      U->g_key = talloc (256);
      U->nonce = talloc (256);
      memcpy (U->g_key, bl->rptr, 256);
      bl->rptr += 64;
      memcpy (U->nonce, bl->rptr, 256);
      bl->rptr += 64;

      U->flags |= FLAG_CREATED;
      U->state = sc_request;
    }
    break;
  case CODE_binlog_set_encr_chat_access_hash:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      U->encr_chat.access_hash = *(long long *)bl->rptr;
      bl->rptr += 2;
    }
    break;
  case CODE_binlog_set_encr_chat_date:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      U->encr_chat.date = *(bl->rptr ++);
    }
    break;
  case CODE_binlog_set_encr_chat_state:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      peer_t *U = user_chat_get (bl, id);
      assert (U);
      U->encr_chat.state = *(bl->rptr ++);
    }
    break;
  case CODE_binlog_encr_chat_accepted:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      peer_t *_U = user_chat_get (bl, id);
      assert (_U);
      struct secret_chat *U = &_U->encr_chat;
      if (!U->g_key) {
        U->g_key = talloc (256);
      }
      if (!U->nonce) {
        U->nonce = talloc (256);
      }
      memcpy (U->g_key, bl->rptr, 256);
      bl->rptr += 64;
      memcpy (U->nonce, bl->rptr, 256);
      bl->rptr += 64;
      U->key_fingerprint = *(long long *)bl->rptr;
      bl->rptr += 2;
      if (U->state == sc_waiting) {
        do_create_keys_end (instance, U);
      }
      U->state = sc_ok;
    }
    break;
  case CODE_binlog_set_encr_chat_key:
    bl->rptr ++;
    {
      peer_id_t id = MK_ENCR_CHAT (*(bl->rptr ++));
      peer_t *_U = user_chat_get (bl, id);
      assert (_U);
      struct secret_chat *U = &_U->encr_chat;
      memcpy (U->key, bl->rptr, 256);
      bl->rptr += 64;
      U->key_fingerprint = *(long long *)bl->rptr;
      bl->rptr += 2;
    }
    break;
  case CODE_binlog_set_dh_params:
    bl->rptr ++;
    {
      if (instance->encr_prime) { tfree (instance->encr_prime, 256); }
      instance->encr_root = *(bl->rptr ++);
      instance->encr_prime = talloc (256);
      memcpy (instance->encr_prime, bl->rptr, 256);
      bl->rptr += 64;
      instance->encr_param_version = *(bl->rptr ++);
    }
    break;
  case CODE_binlog_encr_chat_init:
    bl->rptr ++;
    {
      peer_t *P = talloc0 (sizeof (*P));
      P->id = MK_ENCR_CHAT (*(bl->rptr ++));
      assert (!user_chat_get (bl, P->id));
      P->encr_chat.user_id = *(bl->rptr ++);
      P->encr_chat.admin_id = instance->our_id;
      insert_encrypted_chat (bl, P);
      peer_t *Us = user_chat_get (bl, MK_USER (P->encr_chat.user_id));
      assert (Us);
      P->print_name = create_print_name (bl, P->id, "!", Us->user.first_name, Us->user.last_name, 0);
      peer_insert_name (bl, P);
      memcpy (P->encr_chat.key, bl->rptr, 256);
      bl->rptr += 64;
      P->encr_chat.g_key = talloc (256);
      memcpy (P->encr_chat.g_key, bl->rptr, 256);
      bl->rptr += 64;
      P->flags |= FLAG_CREATED;
    }
    break;
  case CODE_binlog_set_pts:
    bl->rptr ++;
    instance->proto.pts = *(bl->rptr ++);
    break;
  case CODE_binlog_set_qts:
    bl->rptr ++;
    instance->proto.qts = *(bl->rptr ++);
    break;
  case CODE_binlog_set_date:
    bl->rptr ++;
    instance->proto.last_date = *(bl->rptr ++);
    break;
  case CODE_binlog_set_seq:
    bl->rptr ++;
    instance->proto.seq = *(bl->rptr ++);
    break;
  case CODE_binlog_chat_create:
    self->in_ptr ++;
    {
      peer_id_t id = MK_CHAT (fetch_int (self));
      peer_t *_C = user_chat_get (bl, id);
      if (!_C) {
        _C = talloc0 (sizeof (*_C));
        _C->id = id;
        insert_chat (bl, _C);
      } else {
        assert (!(_C->flags & FLAG_CREATED));
      }
      struct chat *C = &_C->chat;
      C->flags = FLAG_CREATED | fetch_int (self);
      C->title = fetch_str_dup (self);
      assert (!C->print_title);
      C->print_title = create_print_name (bl, id, C->title, 0, 0, 0);
      peer_insert_name (bl, (void *)C);
      C->users_num = fetch_int (self);
      C->date = fetch_int (self);
      C->version = fetch_int (self);
      fetch_data (self, &C->photo_big, sizeof (struct file_location));
      fetch_data (self, &C->photo_small, sizeof (struct file_location));
    };
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_chat_change_flags:
    bl->rptr ++;
    {
      peer_t *C = user_chat_get (bl, MK_CHAT (*(bl->rptr ++)));
      assert (C && (C->flags & FLAG_CREATED));
      C->flags |= *(bl->rptr ++);
      C->flags &= ~*(bl->rptr ++);
    };
    break;
  case CODE_binlog_set_chat_title:
    self->in_ptr ++;
    {
      peer_t *_C = user_chat_get (bl, MK_CHAT (fetch_int (self)));
      assert (_C && (_C->flags & FLAG_CREATED));
      struct chat *C = &_C->chat;
      if (C->title) { tfree_str (C->title); }
      C->title = fetch_str_dup (self);
      if (C->print_title) { 
        peer_delete_name (bl, (void *)C);
        tfree_str (C->print_title); 
      }
      C->print_title = create_print_name (bl, C->id, C->title, 0, 0, 0);
      peer_insert_name (bl, (void *)C);
    };
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_set_chat_photo:
    self->in_ptr ++;
    {
      peer_t *C = user_chat_get (bl, MK_CHAT (fetch_int (self)));
      assert (C && (C->flags & FLAG_CREATED));
      fetch_data (self, &C->photo_big, sizeof (struct file_location));
      fetch_data (self, &C->photo_small, sizeof (struct file_location));
    };
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_set_chat_date:
    bl->rptr ++;
    {
      peer_t *C = user_chat_get (bl, MK_CHAT (*(bl->rptr ++)));
      assert (C && (C->flags & FLAG_CREATED));
      C->chat.date = *(bl->rptr ++);
    };
    break;
  case CODE_binlog_set_chat_version:
    bl->rptr ++;
    {
      peer_t *C = user_chat_get (bl, MK_CHAT (*(bl->rptr ++)));
      assert (C && (C->flags & FLAG_CREATED));
      C->chat.version = *(bl->rptr ++);
      C->chat.users_num = *(bl->rptr ++);
    };
    break;
  case CODE_binlog_set_chat_admin:
    bl->rptr ++;
    {
      peer_t *C = user_chat_get (bl, MK_CHAT (*(bl->rptr ++)));
      assert (C && (C->flags & FLAG_CREATED));
      C->chat.admin_id = *(bl->rptr ++);
    };
    break;
  case CODE_binlog_set_chat_participants:
    bl->rptr ++;
    {
      peer_t *C = user_chat_get (bl, MK_CHAT (*(bl->rptr ++)));
      assert (C && (C->flags & FLAG_CREATED));
      C->chat.user_list_version = *(bl->rptr ++);
      if (C->chat.user_list) { tfree (C->chat.user_list, 12 * C->chat.user_list_size); }
      C->chat.user_list_size = *(bl->rptr ++);
      C->chat.user_list = talloc (12 * C->chat.user_list_size);
      memcpy (C->chat.user_list, bl->rptr, 12 * C->chat.user_list_size);
      bl->rptr += 3 * C->chat.user_list_size;
    };
    break;
  case CODE_binlog_chat_full_photo:
    self->in_ptr ++;
    {
      peer_id_t id = MK_CHAT (fetch_int (self));
      peer_t *U = user_chat_get (bl, id);
      assert (U && (U->flags & FLAG_CREATED));
      if (U->flags & FLAG_HAS_PHOTO) {
        free_photo (&U->chat.photo);
      }
      fetch_photo (self, &U->chat.photo);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_add_chat_participant:
    bl->rptr ++;
    {
      peer_id_t id = MK_CHAT (*(bl->rptr ++));
      peer_t *_C = user_chat_get (bl, id);
      assert (_C && (_C->flags & FLAG_CREATED));
      struct chat *C = &_C->chat;

      int version = *(bl->rptr ++);
      int user = *(bl->rptr ++);
      int inviter = *(bl->rptr ++);
      int date = *(bl->rptr ++);
      assert (C->user_list_version < version);

      int i;
      for (i = 0; i < C->user_list_size; i++) {
        assert (C->user_list[i].user_id != user);
      }
      C->user_list_size ++;
      C->user_list = trealloc (C->user_list, 12 * C->user_list_size - 12, 12 * C->user_list_size);
      C->user_list[C->user_list_size - 1].user_id = user;
      C->user_list[C->user_list_size - 1].inviter_id = inviter;
      C->user_list[C->user_list_size - 1].date = date;
      C->user_list_version = version;
    }
    break;
  case CODE_binlog_del_chat_participant:
    bl->rptr ++;
    {
      peer_id_t id = MK_CHAT (*(bl->rptr ++));
      peer_t *_C = user_chat_get (bl, id);
      assert (_C && (_C->flags & FLAG_CREATED));
      struct chat *C = &_C->chat;

      int version = *(bl->rptr ++);
      int user = *(bl->rptr ++);
      assert (C->user_list_version < version);

      int i;
      for (i = 0; i < C->user_list_size; i++) {
        if (C->user_list[i].user_id == user) {
          struct chat_user t;
          t = C->user_list[i];
          C->user_list[i] = C->user_list[C->user_list_size - 1];
          C->user_list[C->user_list_size - 1] = t;
        }
      }
      assert (C->user_list[C->user_list_size - 1].user_id == user);
      C->user_list_size --;
      C->user_list = trealloc (C->user_list, 12 * C->user_list_size + 12, 12 * C->user_list_size);
      C->user_list_version = version;
    }
    break;
  case CODE_binlog_create_message_text:
  case CODE_binlog_send_message_text:
    self->in_ptr ++;
    {
      long long id;
      if (op == CODE_binlog_create_message_text) {
        id = fetch_int (self);
      } else {
        id = fetch_long (self);
      }
      struct message *M = message_get(bl, id);
      if (!M) {
        M = talloc0 (sizeof (*M));
        M->id = id;
        M->instance = instance;
        message_insert_tree (M);
        bl->messages_allocated ++;
	    event_update_new_message (instance, M);
      } else {
        assert (!(M->flags & FLAG_CREATED));
      }
      M->flags |= FLAG_CREATED;
      M->from_id = MK_USER (fetch_int (self));
      int t = fetch_int (self);
      if (t == PEER_ENCR_CHAT) {
        M->flags |= FLAG_ENCRYPTED;
      }
      M->to_id = set_peer_id (t, fetch_int (self));
      M->date = fetch_int (self);
      
      int l = prefetch_strlen (self);
      M->message = talloc (l + 1);
      memcpy (M->message, fetch_str (self, l), l);
      M->message[l] = 0;
      M->message_len = l;

      if (t == PEER_ENCR_CHAT) {
        M->media.type = CODE_decrypted_message_media_empty;
      } else {
        M->media.type = CODE_message_media_empty;
      }
      M->unread = 1;
      M->out = get_peer_id (M->from_id) == instance->our_id;

      message_insert (M);
      if (op == CODE_binlog_send_message_text) {
        message_insert_unsent (M);
        M->flags |= FLAG_PENDING;
      }
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_create_message_text_fwd:
    self->in_ptr ++;
    {
      int id = fetch_int (self);
      struct message *M = message_get(bl, id);
      if (!M) {
        M = talloc0 (sizeof (*M));
        M->id = id;
        message_insert_tree (M);
        bl->messages_allocated ++;
      } else {
        assert (!(M->flags & FLAG_CREATED));
      }
      M->flags |= FLAG_CREATED;
      M->from_id = MK_USER (fetch_int (self));
      int t = fetch_int (self);
      M->to_id = set_peer_id (t, fetch_int (self));
      M->date = fetch_int (self);
      M->fwd_from_id = MK_USER (fetch_int (self));
      M->fwd_date = fetch_int (self);
      
      int l = prefetch_strlen (self);
      M->message = talloc (l + 1);
      memcpy (M->message, fetch_str (self, l), l);
      M->message[l] = 0;
      M->message_len = l;
      
      M->media.type = CODE_message_media_empty;
      M->unread = 1;
      M->out = get_peer_id (M->from_id) == instance->our_id;

      message_insert (M);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_create_message_media:
    self->in_ptr ++;
    {
      int id = fetch_int (self);
      struct message *M = message_get(bl, id);
      if (!M) {
        M = talloc0 (sizeof (*M));
        M->id = id;
        message_insert_tree (M);
        bl->messages_allocated ++;
      } else {
        assert (!(M->flags & FLAG_CREATED));
      }
      M->flags |= FLAG_CREATED;
      M->from_id = MK_USER (fetch_int (self));
      int t = fetch_int (self);
      M->to_id = set_peer_id (t, fetch_int (self));
      M->date = fetch_int (self);
      
      int l = prefetch_strlen (self);
      M->message = talloc (l + 1);
      memcpy (M->message, fetch_str (self, l), l);
      M->message[l] = 0;
      M->message_len = l;

      fetch_message_media (self, &M->media);
      M->unread = 1;
      M->out = get_peer_id (M->from_id) == instance->our_id;

      message_insert (M);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_create_message_media_encr:
    self->in_ptr ++;
    {
      long long id = fetch_long (self);
      struct message *M = message_get(bl, id);
      if (!M) {
        M = talloc0 (sizeof (*M));
        M->id = id;
        message_insert_tree (M);
        bl->messages_allocated ++;
      } else {
        assert (!(M->flags & FLAG_CREATED));
      }
      M->flags |= FLAG_CREATED | FLAG_ENCRYPTED;
      M->from_id = MK_USER (fetch_int (self));
      int t = fetch_int (self);
      M->to_id = set_peer_id (t, fetch_int (self));
      M->date = fetch_int (self);
      
      int l = prefetch_strlen (self);
      M->message = talloc (l + 1);
      memcpy (M->message, fetch_str (self, l), l);
      M->message[l] = 0;
      M->message_len = l;

      fetch_message_media_encrypted (self, &M->media);
      fetch_encrypted_message_file (self, &M->media);

      M->unread = 1;
      M->out = get_peer_id (M->from_id) == instance->our_id;

      message_insert (M);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_create_message_media_fwd:
    self->in_ptr ++;
    {
      int id = fetch_int (self);
      struct message *M = message_get(bl, id);
      if (!M) {
        M = talloc0 (sizeof (*M));
        M->id = id;
        message_insert_tree (M);
        bl->messages_allocated ++;
      } else {
        assert (!(M->flags & FLAG_CREATED));
      }
      M->flags |= FLAG_CREATED;
      M->from_id = MK_USER (fetch_int (self));
      int t = fetch_int (self);
      M->to_id = set_peer_id (t, fetch_int (self));
      M->date = fetch_int (self);
      M->fwd_from_id = MK_USER (fetch_int (self));
      M->fwd_date = fetch_int (self);
      
      int l = prefetch_strlen (self);
      M->message = talloc (l + 1);
      memcpy (M->message, fetch_str (self, l), l);
      M->message[l] = 0;
      M->message_len = l;

      fetch_message_media (self, &M->media);
      M->unread = 1;
      M->out = get_peer_id (M->from_id) == instance->our_id;

      message_insert (M);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_create_message_service:
    self->in_ptr ++;
    {
      int id = fetch_int (self);
      struct message *M = message_get(bl, id);
      if (!M) {
        M = talloc0 (sizeof (*M));
        M->id = id;
        message_insert_tree (M);
        bl->messages_allocated ++;
      } else {
        assert (!(M->flags & FLAG_CREATED));
      }
      M->flags |= FLAG_CREATED;
      M->from_id = MK_USER (fetch_int (self));
      int t = fetch_int (self);
      M->to_id = set_peer_id (t, fetch_int (self));
      M->date = fetch_int (self);

      fetch_message_action (self, &M->action);
      M->unread = 1;
      M->out = get_peer_id (M->from_id) == instance->our_id;
      M->service = 1;

      message_insert (M);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_create_message_service_encr:
    self->in_ptr ++;
    {
      long long id = fetch_long (self);
      struct message *M = message_get(bl, id);
      if (!M) {
        M = talloc0 (sizeof (*M));
        M->id = id;
        message_insert_tree (M);
        bl->messages_allocated ++;
      } else {
        assert (!(M->flags & FLAG_CREATED));
      }
      M->flags |= FLAG_CREATED | FLAG_ENCRYPTED;
      M->from_id = MK_USER (fetch_int (self));
      int t = fetch_int (self);
      M->to_id = set_peer_id (t, fetch_int (self));
      M->date = fetch_int (self);

      fetch_message_action_encrypted (self, &M->action); 
      
      M->unread = 1;
      M->out = get_peer_id (M->from_id) == instance->our_id;
      M->service = 1;

      message_insert (M);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_create_message_service_fwd:
    self->in_ptr ++;
    {
      int id = fetch_int (self);
      struct message *M = message_get(bl, id);
      if (!M) {
        M = talloc0 (sizeof (*M));
        M->id = id;
        message_insert_tree (M);
        bl->messages_allocated ++;
      } else {
        assert (!(M->flags & FLAG_CREATED));
      }
      M->flags |= FLAG_CREATED;
      M->from_id = MK_USER (fetch_int (self));
      int t = fetch_int (self);
      M->to_id = set_peer_id (t, fetch_int (self));
      M->date = fetch_int (self);
      M->fwd_from_id = MK_USER (fetch_int (self));
      M->fwd_date = fetch_int (self);
      fetch_message_action (self, &M->action);
      M->unread = 1;
      M->out = get_peer_id (M->from_id) == instance->our_id;
      M->service = 1;

      message_insert (M);
    }
    bl->rptr = self->in_ptr;
    break;
  case CODE_binlog_set_unread:
    bl->rptr ++;
    {
      struct message *M = message_get(bl, *(bl->rptr ++));
      assert (M);
      M->unread = 0;
    }
    break;
  case CODE_binlog_set_message_sent:
    bl->rptr ++;
    {
      struct message *M = message_get(bl, *(long long *)bl->rptr);
      bl->rptr += 2;
      assert (M);
      message_remove_unsent (M);
      M->flags &= ~FLAG_PENDING;
    }
    break;
  case CODE_binlog_set_msg_id:
    bl->rptr ++;
    {
      struct message *M = message_get(bl, *(long long *)bl->rptr);
      bl->rptr += 2;
      assert (M);
      if (M->flags & FLAG_PENDING) {
        message_remove_unsent (M);
        M->flags &= ~FLAG_PENDING;
      }
      message_remove_tree (M);
      message_del_peer (M);
      M->id = *(bl->rptr ++);
      if (message_get(bl, M->id)) {
        free_message (M);
        tfree (M, sizeof (*M));
      } else {
        message_insert_tree (M);
        message_add_peer (M);
      }
    }
    break;
  case CODE_binlog_delete_msg:
    bl->rptr ++;
    {
      struct message *M = message_get(bl, *(long long *)bl->rptr);
      bl->rptr += 2;
      assert (M);
      if (M->flags & FLAG_PENDING) {
        message_remove_unsent (M);
        M->flags &= ~FLAG_PENDING;
      }
      message_remove_tree (M);
      message_del_peer (M);
      message_del_use (M);
      free_message (M);
      tfree (M, sizeof (*M));
    }
    break;
  case CODE_update_user_photo:
  case CODE_update_user_name:
    work_update_binlog (self);
    bl->rptr = self->in_ptr;
    break;
  default:
    logprintf ("Unknown logevent [0x%08x] 0x%08x [0x%08x] at %lld\n", *(bl->rptr - 1), op, *(bl->rptr + 1), bl->binlog_pos);

    assert (0);
  }
  if (verbosity >= 2) {
    logprintf ("Event end\n");
  }
  bl->in_replay_log = 0;
  bl->binlog_pos += (bl->rptr - start) * 4;
}

void add_log_event (struct binlog *bl, struct mtproto_connection *self, const int *data, int len) {
  logprintf ("Add log event: magic = 0x%08x, len = %d\n", data[0], len);
  assert (!(len & 3));
  if (bl->in_replay_log) { return; }
  bl->rptr = (void *)data;
  bl->wptr = bl->rptr + (len / 4);
  int *in = self->in_ptr;
  int *end = self->in_end;
  replay_log_event (self->connection->instance);
  if (bl->rptr != bl->wptr) {
    logprintf ("Unread %lld ints. Len = %d\n", (long long)(bl->wptr - bl->rptr), len);
    assert (bl->rptr == bl->wptr);
  }
  if (bl->binlog_enabled) {
    assert (bl->binlog_fd > 0);
    assert (write (bl->binlog_fd, data, len) == len);
  }
  self->in_ptr = in;
  self->in_end = end;
}

void bl_do_set_auth_key_id (struct telegram *instance, int num, unsigned char *buf) {
  struct mtproto_connection *self = instance->connection;
  struct binlog *bl = instance->bl;

  static unsigned char sha1_buffer[20];
  SHA1 (buf, 256, sha1_buffer);
  long long fingerprint = *(long long *)(sha1_buffer + 12);
  int *ev = alloc_log_event (bl, 8 + 8 + 256);
  ev[0] = LOG_AUTH_KEY;
  ev[1] = num;
  *(long long *)(ev + 2) = fingerprint;
  memcpy (ev + 4, buf, 256);
  add_log_event (bl, self, ev, 8 + 8 + 256);
}

void bl_do_set_our_id (struct binlog *bl, struct mtproto_connection *self, int id) {
  int *ev = alloc_log_event (bl, 8);
  ev[0] = LOG_OUR_ID;
  ev[1] = id;
  add_log_event (bl, self, ev, 8);
}

void bl_do_new_user (struct binlog *bl, struct mtproto_connection *self, int id, const char *f, int fl, const char *l, int ll, 
    long long access_token, const char *p, int pl, int contact) {

  clear_packet (self);
  out_int (self, CODE_binlog_new_user);
  out_int (self, id);
  out_cstring (self, f ? f : "", fl);
  out_cstring (self, l ? l : "", ll);
  out_long (self, access_token);
  out_cstring (self, p ? p : "", pl);
  out_int (self, contact);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_user_delete (struct binlog *bl, struct mtproto_connection *self, struct user *U) {
  if (U->flags & FLAG_DELETED) { return; }
  int *ev = alloc_log_event (bl, 8);
  ev[0] = CODE_binlog_user_delete;
  ev[1] = get_peer_id (U->id);
  add_log_event (bl, self, ev, 8);
}

void bl_do_set_user_profile_photo (struct binlog *bl, struct mtproto_connection *self, struct user *U, 
    long long photo_id, struct file_location *big, struct file_location *small) {
  if (photo_id == U->photo_id) { return; }
  if (!photo_id) {
    int *ev = alloc_log_event (bl, 20);
    ev[0] = CODE_update_user_photo;
    ev[1] = get_peer_id (U->id);
    ev[2] = self->connection->instance->proto.last_date;
    ev[3] = CODE_user_profile_photo_empty;
    ev[4] = CODE_bool_false;
    add_log_event (bl, self, ev, 20);
  } else {
    clear_packet (self);
    out_int (self, CODE_update_user_photo);
    out_int (self, get_peer_id (U->id));
    out_int (self, self->connection->instance->proto.last_date);
    out_int (self, CODE_user_profile_photo);
    out_long (self, photo_id);
    if (small->dc >= 0) {
      out_int (self, CODE_file_location);
      out_int (self, small->dc);
      out_long (self, small->volume);
      out_int (self, small->local_id);
      out_long (self, small->secret);
    } else {
      out_int (self, CODE_file_location_unavailable);
      out_long (self, small->volume);
      out_int (self, small->local_id);
      out_long (self, small->secret);
    }
    if (big->dc >= 0) {
      out_int (self, CODE_file_location);
      out_int (self, big->dc);
      out_long (self, big->volume);
      out_int (self, big->local_id);
      out_long (self, big->secret);
    } else {
      out_int (self, CODE_file_location_unavailable);
      out_long (self, big->volume);
      out_int (self, big->local_id);
      out_long (self, big->secret);
    }
    out_int (self, CODE_bool_false);
    add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
  }
}

void bl_do_set_user_name (struct binlog *bl, struct mtproto_connection *self, struct user *U, const char *f, 
    int fl, const char *l, int ll) {
  if ((U->first_name && (int)strlen (U->first_name) == fl && !strncmp (U->first_name, f, fl)) && 
      (U->last_name  && (int)strlen (U->last_name)  == ll && !strncmp (U->last_name,  l, ll))) {
    return;
  }
  clear_packet (self);
  out_int (self, CODE_update_user_name);
  out_int (self, get_peer_id (U->id));
  out_cstring (self, f, fl);
  out_cstring (self, l, ll);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_set_user_access_token (struct binlog *bl, struct mtproto_connection *self, struct user *U, long long access_token) {
  if (U->access_hash == access_token) { return; }
  int *ev = alloc_log_event (bl, 16);
  ev[0] = CODE_binlog_set_user_access_token;
  ev[1] = get_peer_id (U->id);
  *(long long *)(ev + 2) = access_token;
  add_log_event (bl, self, ev, 16);
}

void bl_do_set_user_phone (struct binlog *bl, struct mtproto_connection *self, struct user *U, 
const char *p, int pl) {
  if (U->phone && (int)strlen (U->phone) == pl && !strncmp (U->phone, p, pl)) {
    return;
  }
  clear_packet (self);
  out_int (self, CODE_binlog_set_user_phone);
  out_int (self, get_peer_id (U->id));
  out_cstring (self, p, pl);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_set_user_friend (struct binlog *bl, struct mtproto_connection *self, struct user *U, int friend) {
  if (friend == ((U->flags & FLAG_USER_CONTACT) != 0)) { return ; }
  int *ev = alloc_log_event (bl, 12);
  ev[0] = CODE_binlog_set_user_friend;
  ev[1] = get_peer_id (U->id);
  ev[2] = friend;
  add_log_event (bl, self, ev, 12);
}

void bl_do_dc_option (struct binlog *bl, struct mtproto_connection *self, int id, int l1, const char *name, 
    int l2, const char *ip, int port, struct telegram *instance) {
  struct dc *DC = instance->auth.DC_list[id];
  if (DC) { return; }
  
  clear_packet (self);
  out_int (self, CODE_binlog_dc_option);
  out_int (self, id);
  out_cstring (self, name, l1);
  out_cstring (self, ip, l2);
  out_int (self, port);

  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_dc_signed (struct binlog *bl, struct mtproto_connection *self, int id) {
  int *ev = alloc_log_event (bl, 8);
  ev[0] = LOG_DC_SIGNED;
  ev[1] = id;
  add_log_event (bl, self, ev, 8);
}

void bl_do_set_working_dc (struct binlog *bl, struct mtproto_connection *self, int num) {
  int *ev = alloc_log_event (bl, 8);
  ev[0] = LOG_DEFAULT_DC;
  ev[1] = num;
  add_log_event (bl, self, ev, 8);
}

void bl_do_set_user_full_photo (struct binlog *bl, struct mtproto_connection *self, struct user *U, const int *start, int len) {
  if (U->photo.id == *(long long *)(start + 1)) { return; }
  int *ev = alloc_log_event (bl, len + 8);
  ev[0] = CODE_binlog_user_full_photo;
  ev[1] = get_peer_id (U->id);
  memcpy (ev + 2, start, len);
  add_log_event (bl, self, ev, len + 8);
}

void bl_do_set_user_blocked (struct binlog *bl, struct mtproto_connection *self, struct user *U, int blocked) {
  if (U->blocked == blocked) { return; }
  int *ev = alloc_log_event (bl, 12);
  ev[0] = CODE_binlog_user_blocked;
  ev[1] = get_peer_id (U->id);
  ev[2] = blocked;
  add_log_event (bl, self, ev, 12);
}

void bl_do_set_user_real_name (struct binlog *bl, struct mtproto_connection *self, struct user *U, const char *f, int fl, const char *l, int ll) {
  if ((U->real_first_name && (int)strlen (U->real_first_name) == fl && !strncmp (U->real_first_name, f, fl)) && 
      (U->real_last_name  && (int)strlen (U->real_last_name)  == ll && !strncmp (U->real_last_name,  l, ll))) {
    return;
  }
  clear_packet (self);
  out_int (self, CODE_binlog_set_user_full_name);
  out_int (self, get_peer_id (U->id));
  out_cstring (self, f, fl);
  out_cstring (self, l, ll);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_encr_chat_delete (struct binlog *bl, struct mtproto_connection *self, struct secret_chat *U) {
  if (!(U->flags & FLAG_CREATED) || U->state == sc_deleted || U->state == sc_none) { return; }
  int *ev = alloc_log_event (bl, 8);
  ev[0] = CODE_binlog_encr_chat_delete;
  ev[1] = get_peer_id (U->id);
  add_log_event (bl, self, ev, 8);
}

void bl_do_encr_chat_requested (struct binlog *bl, struct mtproto_connection *self, struct secret_chat *U, 
    long long access_hash, int date, int admin_id, int user_id, unsigned char g_key[], 
    unsigned char nonce[]) {
  if (U->state != sc_none) { return; }
  int *ev = alloc_log_event (bl, 540);
  ev[0] = CODE_binlog_encr_chat_requested;
  ev[1] = get_peer_id (U->id);
  *(long long *)(ev + 2) = access_hash;
  ev[4] = date;
  ev[5] = admin_id;
  ev[6] = user_id;
  memcpy (ev + 7, g_key, 256);
  memcpy (ev + 7 + 64, nonce, 256);
  add_log_event (bl, self, ev, 540);
}

void bl_do_set_encr_chat_access_hash (struct binlog *bl, struct mtproto_connection *self, struct secret_chat *U, 
    long long access_hash) {
  if (U->access_hash == access_hash) { return; }
  int *ev = alloc_log_event (bl, 16);
  ev[0] = CODE_binlog_set_encr_chat_access_hash;
  ev[1] = get_peer_id (U->id);
  *(long long *)(ev + 2) = access_hash;
  add_log_event (bl, self, ev, 16);
}

void bl_do_set_encr_chat_date (struct binlog *bl, struct mtproto_connection *self, struct secret_chat *U, int date) {
  if (U->date == date) { return; }
  int *ev = alloc_log_event (bl, 12);
  ev[0] = CODE_binlog_set_encr_chat_date;
  ev[1] = get_peer_id (U->id);
  ev[2] = date;
  add_log_event (bl, self, ev, 12);
}

void bl_do_set_encr_chat_state (struct binlog *bl, struct mtproto_connection *self, struct secret_chat *U, enum secret_chat_state state) {
  if (U->state == state) { return; }
  int *ev = alloc_log_event (bl, 12);
  ev[0] = CODE_binlog_set_encr_chat_state;
  ev[1] = get_peer_id (U->id);
  ev[2] = state;
  add_log_event (bl, self, ev, 12);
}

void bl_do_encr_chat_accepted (struct binlog *bl, struct mtproto_connection *self, struct secret_chat *U, const unsigned char g_key[], const unsigned char nonce[], long long key_fingerprint) {
  if (U->state != sc_waiting && U->state != sc_request) { return; }
  int *ev = alloc_log_event (bl, 528);
  ev[0] = CODE_binlog_encr_chat_accepted;
  ev[1] = get_peer_id (U->id);
  memcpy (ev + 2, g_key, 256);
  memcpy (ev + 66, nonce, 256);
  *(long long *)(ev + 130) = key_fingerprint;
  add_log_event (bl, self, ev, 528);
}

void bl_do_set_encr_chat_key (struct binlog *bl, struct mtproto_connection *self, struct secret_chat *E, unsigned char key[], long long key_fingerprint) {
  int *ev = alloc_log_event (bl, 272);
  ev[0] = CODE_binlog_set_encr_chat_key;
  ev[1] = get_peer_id (E->id);
  memcpy (ev + 2, key, 256);
  *(long long *)(ev + 66) = key_fingerprint;
  add_log_event (bl, self, ev, 272);
}

void bl_do_set_dh_params (struct binlog *bl, struct mtproto_connection *self, int root, unsigned char prime[], int version) {
  int *ev = alloc_log_event (bl, 268);
  ev[0] = CODE_binlog_set_dh_params;
  ev[1] = root;
  memcpy (ev + 2, prime, 256);
  ev[66] = version;
  add_log_event (bl, self, ev, 268);
}

void bl_do_encr_chat_init (struct binlog *bl, struct mtproto_connection *self, int id, int user_id, unsigned char random[], unsigned char g_a[]) {
  int *ev = alloc_log_event (bl, 524);
  ev[0] = CODE_binlog_encr_chat_init;
  ev[1] = id;
  ev[2] = user_id;
  memcpy (ev + 3, random, 256);
  memcpy (ev + 67, g_a, 256);
  add_log_event (bl, self, ev, 524);
}

void bl_do_set_pts (struct binlog *bl, struct mtproto_connection *self, int pts) {
  int *ev = alloc_log_event (bl, 8);
  ev[0] = CODE_binlog_set_pts;
  ev[1] = pts;
  add_log_event (bl, self, ev, 8);
}

void bl_do_set_qts (struct binlog *bl, struct mtproto_connection *self, int qts) {
  int *ev = alloc_log_event (bl, 8);
  ev[0] = CODE_binlog_set_qts;
  ev[1] = qts;
  add_log_event (bl, self, ev, 8);
}

void bl_do_set_date (struct binlog *bl, struct mtproto_connection *self, int date) {
  int *ev = alloc_log_event (bl, 8);
  ev[0] = CODE_binlog_set_date;
  ev[1] = date;
  add_log_event (bl, self, ev, 8);
}

void bl_do_set_seq (struct binlog *bl, struct mtproto_connection *self, int seq) {
  int *ev = alloc_log_event (bl, 8);
  ev[0] = CODE_binlog_set_seq;
  ev[1] = seq;
  add_log_event (bl, self, ev, 8);
}

void bl_do_create_chat (struct binlog *bl, struct mtproto_connection *self, struct chat *C, int y, const char *s, int l, int users_num, int date, int version, struct file_location *big, struct file_location *small) {
  clear_packet (self);
  out_int (self, CODE_binlog_chat_create);
  out_int (self, get_peer_id (C->id));
  out_int (self, y);
  out_cstring (self, s, l);
  out_int (self, users_num);
  out_int (self, date);
  out_int (self, version);
  out_data (self, big, sizeof (struct file_location));
  out_data (self, small, sizeof (struct file_location));
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_chat_forbid (struct binlog *bl, struct mtproto_connection *self, struct chat *C, int on) {
  if (on) {
    if (C->flags & FLAG_FORBIDDEN) { return; }
    int *ev = alloc_log_event (bl, 16);
    ev[0] = CODE_binlog_chat_change_flags;
    ev[1] = get_peer_id (C->id);
    ev[2] = FLAG_FORBIDDEN;
    ev[3] = 0;
    add_log_event (bl, self, ev, 16);
  } else {
    if (!(C->flags & FLAG_FORBIDDEN)) { return; }
    int *ev = alloc_log_event (bl, 16);
    ev[0] = CODE_binlog_chat_change_flags;
    ev[1] = get_peer_id (C->id);
    ev[2] = 0;
    ev[3] = FLAG_FORBIDDEN;
    add_log_event (bl, self, ev, 16);
  }
}

void bl_do_set_chat_title (struct binlog *bl, struct mtproto_connection *self, struct chat *C, const char *s, int l) {
  if (C->title && (int)strlen (C->title) == l && !strncmp (C->title, s, l)) { return; }
  clear_packet (self);
  out_int (self, CODE_binlog_set_chat_title);
  out_int (self, get_peer_id (C->id));
  out_cstring (self, s, l);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_set_chat_photo (struct binlog *bl, struct mtproto_connection *self, struct chat *C, struct file_location *big, struct file_location *small) {
  if (!memcmp (&C->photo_small, small, sizeof (struct file_location)) &&
      !memcmp (&C->photo_big, big, sizeof (struct file_location))) { return; }
  clear_packet (self);
  out_int (self, CODE_binlog_set_chat_photo);
  out_int (self, get_peer_id (C->id));
  out_data (self, big, sizeof (struct file_location));
  out_data (self, small, sizeof (struct file_location));
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_set_chat_date (struct binlog *bl, struct mtproto_connection *self, struct chat *C, int date) {
  if (C->date == date) { return; }
  int *ev = alloc_log_event (bl, 12);
  ev[0] = CODE_binlog_set_chat_date;
  ev[1] = get_peer_id (C->id);
  ev[2] = date;
  add_log_event (bl, self, ev, 12);
}

void bl_do_set_chat_set_in_chat (struct binlog *bl, struct mtproto_connection *self, struct chat *C, int on) {
  if (on) {
    if (C->flags & FLAG_CHAT_IN_CHAT) { return; }
    int *ev = alloc_log_event (bl, 16);
    ev[0] = CODE_binlog_chat_change_flags;
    ev[1] = get_peer_id (C->id);
    ev[2] = FLAG_CHAT_IN_CHAT;
    ev[3] = 0;
    add_log_event (bl, self, ev, 16);
  } else {
    if (!(C->flags & FLAG_CHAT_IN_CHAT)) { return; }
    int *ev = alloc_log_event (bl, 16);
    ev[0] = CODE_binlog_chat_change_flags;
    ev[1] = get_peer_id (C->id);
    ev[2] = 0;
    ev[3] = FLAG_CHAT_IN_CHAT;
    add_log_event (bl, self, ev, 16);
  }
}

void bl_do_set_chat_version (struct binlog *bl, struct mtproto_connection *self, struct chat *C, int version, int user_num) {
  if (C->version >= version) { return; }
  int *ev = alloc_log_event (bl, 16);
  ev[0] = CODE_binlog_set_chat_version;
  ev[1] = get_peer_id (C->id);
  ev[2] = version;
  ev[3] = user_num;
  add_log_event (bl, self, ev, 16);
}

void bl_do_set_chat_admin (struct binlog *bl, struct mtproto_connection *self, struct chat *C, int admin) {
  if (C->admin_id == admin) { return; }
  int *ev = alloc_log_event (bl, 12);
  ev[0] = CODE_binlog_set_chat_admin;
  ev[1] = get_peer_id (C->id);
  ev[2] = admin;
  add_log_event (bl, self, ev, 12);
}

void bl_do_set_chat_participants (struct binlog *bl, struct mtproto_connection *self, struct chat *C, int version, int user_num, struct chat_user *users) {
  if (C->user_list_version >= version) { return; }
  int *ev = alloc_log_event (bl, 12 * user_num + 16);
  ev[0] = CODE_binlog_set_chat_participants;
  ev[1] = get_peer_id (C->id);
  ev[2] = version;
  ev[3] = user_num;
  memcpy (ev + 4, users, 12 * user_num);
  add_log_event (bl, self, ev, 12 * user_num + 16);
}

void bl_do_set_chat_full_photo (struct binlog *bl, struct mtproto_connection *self, struct chat *U, const int *start, int len) {
  if (U->photo.id == *(long long *)(start + 1)) { return; }
  int *ev = alloc_log_event (bl, len + 8);
  ev[0] = CODE_binlog_chat_full_photo;
  ev[1] = get_peer_id (U->id);
  memcpy (ev + 2, start, len);
  add_log_event (bl, self, ev, len + 8);
}

void bl_do_chat_add_user (struct binlog *bl, struct mtproto_connection *self, struct chat *C, int version, int user, int inviter, int date) {
  if (C->user_list_version >= version || !C->user_list_version) { return; }
  int *ev = alloc_log_event (bl, 24);
  ev[0] = CODE_binlog_add_chat_participant;
  ev[1] = get_peer_id (C->id);
  ev[2] = version;
  ev[3] = user;
  ev[4] = inviter;
  ev[5] = date;
  add_log_event (bl, self, ev, 24);
}

void bl_do_chat_del_user (struct binlog *bl, struct mtproto_connection *self, struct chat *C, int version, int user) {
  if (C->user_list_version >= version || !C->user_list_version) { return; }
  int *ev = alloc_log_event (bl, 16);
  ev[0] = CODE_binlog_add_chat_participant;
  ev[1] = get_peer_id (C->id);
  ev[2] = version;
  ev[3] = user;
  add_log_event (bl, self, ev, 16);
}

void bl_do_create_message_text (struct binlog *bl, struct mtproto_connection *self, int msg_id, int from_id, int to_type, int to_id, int date, int l, const char *s) {
  clear_packet (self);
  out_int (self, CODE_binlog_create_message_text);
  out_int (self, msg_id);
  out_int (self, from_id);
  out_int (self, to_type);
  out_int (self, to_id);
  out_int (self, date);
  out_cstring (self, s, l);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_send_message_text (struct binlog *bl, struct mtproto_connection *self, long long msg_id, int from_id, int to_type, int to_id, int date, int l, const char *s) {
  clear_packet (self);
  out_int (self, CODE_binlog_send_message_text);
  out_long (self, msg_id);
  out_int (self, from_id);
  out_int (self, to_type);
  out_int (self, to_id);
  out_int (self, date);
  out_cstring (self, s, l);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_create_message_text_fwd (struct binlog *bl, struct mtproto_connection *self, int msg_id, int from_id, int to_type, int to_id, int date, int fwd, int fwd_date, int l, const char *s) {
  clear_packet (self);
  out_int (self, CODE_binlog_create_message_text_fwd);
  out_int (self, msg_id);
  out_int (self, from_id);
  out_int (self, to_type);
  out_int (self, to_id);
  out_int (self, date);
  out_int (self, fwd);
  out_int (self, fwd_date);
  out_cstring (self, s, l);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_create_message_media (struct binlog *bl, struct mtproto_connection *self, int msg_id, int from_id, int to_type, int to_id, int date, int l, const char *s, const int *data, int len) {
  clear_packet (self);
  out_int (self, CODE_binlog_create_message_media);
  out_int (self, msg_id);
  out_int (self, from_id);
  out_int (self, to_type);
  out_int (self, to_id);
  out_int (self, date);
  out_cstring (self, s, l);
  out_ints (self, data, len);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_create_message_media_encr (struct binlog *bl, struct mtproto_connection *self, long long msg_id, int from_id, int to_type, int to_id, int date, int l, const char *s, const int *data, int len, const int *data2, int len2) {
  clear_packet (self);
  out_int (self, CODE_binlog_create_message_media_encr);
  out_long (self, msg_id);
  out_int (self, from_id);
  out_int (self, to_type);
  out_int (self, to_id);
  out_int (self, date);
  out_cstring (self, s, l);
  out_ints (self, data, len);
  out_ints (self, data2, len2);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_create_message_media_fwd (struct binlog *bl, struct mtproto_connection *self, int msg_id, int from_id, int to_type, int to_id, int date, int fwd, int fwd_date, int l, const char *s, const int *data, int len) {
  clear_packet (self);
  out_int (self, CODE_binlog_create_message_media_fwd);
  out_int (self, msg_id);
  out_int (self, from_id);
  out_int (self, to_type);
  out_int (self, to_id);
  out_int (self, date);
  out_int (self, fwd);
  out_int (self, fwd_date);
  out_cstring (self, s, l);
  out_ints (self, data, len);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_create_message_service (struct binlog *bl, struct mtproto_connection *self, int msg_id, int from_id, int to_type, int to_id, int date, const int *data, int len) {
  clear_packet (self);
  out_int (self, CODE_binlog_create_message_service);
  out_int (self, msg_id);
  out_int (self, from_id);
  out_int (self, to_type);
  out_int (self, to_id);
  out_int (self, date);
  out_ints (self, data, len);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}
void bl_do_create_message_service_encr (struct binlog *bl, struct mtproto_connection *self, long long msg_id, int from_id, int to_type, int to_id, int date, const int *data, int len) {
  clear_packet (self);
  out_int (self, CODE_binlog_create_message_service_encr);
  out_long (self, msg_id);
  out_int (self, from_id);
  out_int (self, to_type);
  out_int (self, to_id);
  out_int (self, date);
  out_ints (self, data, len);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_create_message_service_fwd (struct binlog *bl, struct mtproto_connection *self, int msg_id, int from_id, int to_type, int to_id, int date, int fwd, int fwd_date, const int *data, int len) {
  clear_packet (self);
  out_int (self, CODE_binlog_create_message_service_fwd);
  out_int (self, msg_id);
  out_int (self, from_id);
  out_int (self, to_type);
  out_int (self, to_id);
  out_int (self, date);
  out_int (self, fwd);
  out_int (self, fwd_date);
  out_ints (self, data, len);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_set_unread (struct binlog *bl, struct mtproto_connection *self, struct message *M, int unread) {
  if (unread || !M->unread) { return; }
  clear_packet (self);
  out_int (self, CODE_binlog_set_unread);
  out_int (self, M->id);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_set_message_sent (struct binlog *bl, struct mtproto_connection *self, struct message *M) {
  if (!(M->flags & FLAG_PENDING)) { return; }
  clear_packet (self);
  out_int (self, CODE_binlog_set_message_sent);
  out_long (self, M->id);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_set_msg_id (struct binlog *bl, struct mtproto_connection *self, struct message *M, int id) {
  if (M->id == id) { return; }
  clear_packet (self);
  out_int (self, CODE_binlog_set_msg_id);
  out_long (self, M->id);
  out_int (self, id);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}

void bl_do_delete_msg (struct binlog *bl, struct mtproto_connection *self, struct message *M) {
  clear_packet (self);
  out_int (self, CODE_binlog_delete_msg);
  out_long (self, M->id);
  add_log_event (bl, self, self->packet_buffer, 4 * (self->packet_ptr - self->packet_buffer));
}
