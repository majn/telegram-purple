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
 
 Copyright Matthias Jentsch 2014
 */
#ifndef __telegram_adium__tgp_util__
#define __telegram_adium__tgp_util__

#include <stdio.h>
#include <time.h>
#include <tgl.h>
#include <glib.h>

tgl_peer_t *tgp_encr_chat_get_partner (struct tgl_state *TLS, struct tgl_secret_chat *chat);
const char *format_time (time_t date);
char *format_img_full (int imgstore);
int str_not_empty (const char *string);

int our_msg (struct tgl_state *TLS, struct tgl_message *M);
int out_msg (struct tgl_state *TLS, struct tgl_message *M);

char *tgp_g_format_size (gint64 size);
void tgp_g_queue_free_full (GQueue *queue, GDestroyNotify free_func);

#endif /* defined(__telegram_adium__tgp_util__) */
