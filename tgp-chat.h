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

#ifndef __telegram_adium__tgp_chat__
#define __telegram_adium__tgp_chat__

#include "telegram-purple.h"
#include "tgp-structs.h"
#include "tgp-2prpl.h"
#include <purple.h>

void tgp_chat_on_loaded_chat_full (struct tgl_state *TLS, struct tgl_chat *C);
PurpleConversation *tgp_chat_show (struct tgl_state *TLS, struct tgl_chat *C);
void tgp_chat_users_update (struct tgl_state *TLS, struct tgl_chat *C);
int chat_add_message (struct tgl_state *TLS, struct tgl_message *M, char *text);

char *tgprpl_get_chat_name (GHashTable *data);
void tgprpl_chat_join (PurpleConnection *gc, GHashTable *data);
GList *tgprpl_chat_join_info (PurpleConnection *gc);

#endif
