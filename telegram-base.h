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
#ifndef __TELEGRAM_BASE_H__
#define __TELEGRAM_BASE_H__

#include "telegram-purple.h"

void read_state_file (struct tgl_state *TLS);
void read_auth_file (struct tgl_state *TLS);
void write_auth_file (struct tgl_state *TLS);
void write_state_file (struct tgl_state *TLS);

void telegram_login (struct tgl_state *TLS);
PurpleConversation *chat_show (PurpleConnection *gc, int id);
int chat_add_message (struct tgl_state *TLS, struct tgl_message *M, char *text);
void chat_add_all_users (PurpleConversation *pc, struct tgl_chat *chat);
void request_code_entered (gpointer data, const gchar *code);

void pending_reads_send_all (GQueue *queue, struct tgl_state *TLS);
void pending_reads_add (GQueue *queue, tgl_peer_id_t id);
void pending_reads_clear (GQueue *queue);

#endif
