/**
 * @file ycht.h The Yahoo! protocol plugin, YCHT protocol stuff.
 *
 * purple
 *
 * Copyright (C) 2004 Timothy Ringenbach <omarvo@hotmail.com>
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#ifndef _PURPLE_YCHT_H_
#define _PURPLE_YCHT_H_

/* #define YAHOO_YCHT_DEBUG */

#define YAHOO_YCHT_HOST "jcs3.chat.dcn.yahoo.com"
#define YAHOO_YCHT_PORT 8002

#define YCHT_VERSION (0xae)
#define YCHT_HEADER_LEN (0x10)

typedef enum {
	YCHT_SERVICE_LOGIN = 0x01,
	YCHT_SERVICE_LOGOUT = 0x02,
	YCHT_SERVICE_CHATJOIN = 0x11,
	YCHT_SERVICE_CHATPART = 0x12,
	YCHT_SERVICE_CHATMSG = 0x41,
	YCHT_SERVICE_CHATMSG_EMOTE = 0x43,
	YCHT_SERVICE_PING = 0x62,
	YCHT_SERVICE_ONLINE_FRIENDS = 0x68
} ycht_service;
/*
yahoo: YCHT Service: 0x11 Version: 0x100
yahoo: Data[0]: Linux, FreeBSD, Solaris:1
yahoo: Data[1]: Questions, problems and discussions about all flavors of Unix.
yahoo: Data[2]:
yahoo: Data[3]: 0
yahoo: Data[4]: sgooki888\0020\002 \0022769036\00258936\002
yahoo: --==End of incoming YCHT packet==--

yahoo: --==Incoming YCHT packet==--
yahoo: YCHT Service: 0x12 Version: 0x100
yahoo: Data[0]: Linux, FreeBSD, Solaris:1
yahoo: Data[1]: cccc4cccc
yahoo: --==End of incoming YCHT packet==--

*/
#define YCHT_SEP "\xc0\x80"

typedef struct _YchtConn {
	PurpleConnection *gc;
	gchar *room;
	int room_id;
	gint fd;
	gint inpa;
	gboolean logged_in;
	gboolean changing_rooms;
	guchar *rxqueue;
	guint rxlen;
	PurpleCircBuffer *txbuf;
	guint tx_handler;
} YchtConn;

typedef struct {
	guint version;
	guint service;
	gint status;
	GList *data;
} YchtPkt;

void ycht_connection_open(PurpleConnection *gc);
void ycht_connection_close(YchtConn *ycht);

void ycht_chat_join(YchtConn *ycht, const char *room);
int ycht_chat_send(YchtConn *ycht, const char *room, const char *what);
void ycht_chat_leave(YchtConn *ycht, const char *room, gboolean logout);
void ycht_chat_send_invite(YchtConn *ycht, const char *room, const char *buddy, const char *msg);
void ycht_chat_goto_user(YchtConn *ycht, const char *name);
void ycht_chat_send_keepalive(YchtConn *ycht);

#endif /* _PURPLE_YCHT_H_ */
