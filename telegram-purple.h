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
#ifndef __TG_PURPLE_H__
#define __TG_PURPLE_H__

#include <tgl.h>

#define PLUGIN_ID "prpl-telegram"
#define TG_AUTHOR "Matthias Jentsch <mtthsjntsch@gmail.com>, Vitaly Valtman, Christopher Althaus <althaus.christopher@gmail.com>, Markus Endres <endresma45241@th-nuernberg.de>. Based on libtgl by Vitaly Valtman."
#define TG_DESCRIPTION "Telegram protocol."
#define TG_BUILD "11"

#include <notify.h>
#include <plugin.h>
#include <version.h>
#include <account.h>
#include <connection.h>
  
#define TGP_APP_HASH "99428c722d0ed59b9cd844e4577cb4bb"
#define TGP_APP_ID 16154

#define TGP_MAX_MSG_SIZE 4096
#define TGP_DEFAULT_MAX_MSG_SPLIT_COUNT 4
#define TGP_DEFAULT_HISTORY_RETRIEVAL_THRESHOLD 14

void on_chat_get_info (struct tgl_state *TLS, void *extra, int success, struct tgl_chat *C);
void on_ready (struct tgl_state *TLS);
extern const char *pk_path;
extern const char *config_dir;
extern PurplePlugin *_telegram_protocol;
char *format_user_status (struct tgl_user_status *status);
char *p2tgl_peer_strdup_id (tgl_peer_id_t user);

#endif
