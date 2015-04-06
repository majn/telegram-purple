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

    Copyright Matthias Jentsch, Vitaly Valtman, Christopher Althaus, Markus Endres 2014-2015
*/
#ifndef __TELEGRAM_BASE_H__
#define __TELEGRAM_BASE_H__

#include "telegram-purple.h"

void read_state_file (struct tgl_state *TLS);
void read_auth_file (struct tgl_state *TLS);
void write_auth_file (struct tgl_state *TLS);
void write_state_file (struct tgl_state *TLS);
void write_files_schedule (struct tgl_state *TLS);
void read_secret_chat_file (struct tgl_state *TLS);
void write_secret_chat_file (struct tgl_state *TLS);
void write_secret_chat_gw (struct tgl_state *TLS, void *extra, int success, struct tgl_secret_chat *E);

void telegram_login (struct tgl_state *TLS);
void request_code_entered (gpointer data, const gchar *code);
int generate_ident_icon(struct tgl_state *TLS, unsigned char* sha1_key);

void request_accept_secret_chat (struct tgl_state *TLS, struct tgl_secret_chat *U);
#endif
