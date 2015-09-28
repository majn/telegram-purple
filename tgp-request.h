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

#ifndef tgp_request_h
#define tgp_request_h

#include <stdio.h>
#include <purple.h>

#include "tgp-structs.h"

struct request_password_data {
  struct tgl_state *TLS;
  void *callback;
  void *arg;
};

struct accept_create_chat_data {
  struct tgl_state *TLS;
  char *title;
};

struct accept_secret_chat_data {
  struct tgl_state *TLS;
  struct tgl_secret_chat *U;
};

void request_code (struct tgl_state *TLS, void (*callback) (struct tgl_state *TLS));
void request_code_entered (gpointer extra, const gchar *code);
void request_name_code_entered (PurpleConnection* gc, PurpleRequestFields* fields);
void request_name_and_code (struct tgl_state *TLS);
void request_password (struct tgl_state *TLS, void (*callback)(struct tgl_state *TLS, const char *string[], void *arg), void *arg);
void request_accept_secret_chat (struct tgl_state *TLS, struct tgl_secret_chat *U);
void request_choose_user (struct accept_create_chat_data *data);
void request_create_chat (struct tgl_state *TLS, const char *subject);

#endif
