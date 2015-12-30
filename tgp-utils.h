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
#ifndef __telegram_adium__tgp_util__
#define __telegram_adium__tgp_util__

#include "tgp-structs.h"

#include <stdio.h>
#include <time.h>
#include <tgl.h>
#include <glib.h>

tgl_peer_t *tgp_encr_chat_get_partner (struct tgl_state *TLS, struct tgl_secret_chat *chat);

/**
 * Return whether this message was created by our client in this session
 */
int tgp_outgoing_msg (struct tgl_state *TLS, struct tgl_message *M);

/**
 * Return whether this message was created by the current user
 */
int tgp_our_msg (struct tgl_state *TLS, struct tgl_message *M);

const char *format_time (time_t date);
char *tgp_format_img (int imgstore);
char *tgp_format_user_status (struct tgl_user_status *status);
int str_not_empty (const char *string);
long tgp_time_n_days_ago (int days);
void tgp_g_queue_free_full (GQueue *queue, GDestroyNotify free_func);
void tgp_g_list_free_full (GList *list, GDestroyNotify free_func);
const char *tgp_mime_to_filetype (const char *mime);
int tgp_startswith (const char *str, const char *with);
void tgp_replace (char *string, char what, char with);

#endif
