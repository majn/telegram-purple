/**
 * @file yahoo_packet.h The Yahoo! protocol plugin
 *
 * purple
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

#ifndef _YAHOO_PACKET_H_
#define _YAHOO_PACKET_H_

enum yahoo_service { /* these are easier to see in hex */
	YAHOO_SERVICE_LOGON = 1,
	YAHOO_SERVICE_LOGOFF,
	YAHOO_SERVICE_ISAWAY,
	YAHOO_SERVICE_ISBACK,
	YAHOO_SERVICE_IDLE, /* 5 (placemarker) */
	YAHOO_SERVICE_MESSAGE,
	YAHOO_SERVICE_IDACT,
	YAHOO_SERVICE_IDDEACT,
	YAHOO_SERVICE_MAILSTAT,
	YAHOO_SERVICE_USERSTAT, /* 0xa */
	YAHOO_SERVICE_NEWMAIL,
	YAHOO_SERVICE_CHATINVITE,
	YAHOO_SERVICE_CALENDAR,
	YAHOO_SERVICE_NEWPERSONALMAIL,
	YAHOO_SERVICE_NEWCONTACT,
	YAHOO_SERVICE_ADDIDENT, /* 0x10 */
	YAHOO_SERVICE_ADDIGNORE,
	YAHOO_SERVICE_PING,
	YAHOO_SERVICE_GOTGROUPRENAME,
	YAHOO_SERVICE_SYSMESSAGE = 0x14,
	YAHOO_SERVICE_SKINNAME = 0x15,
	YAHOO_SERVICE_PASSTHROUGH2 = 0x16,
	YAHOO_SERVICE_CONFINVITE = 0x18,
	YAHOO_SERVICE_CONFLOGON,
	YAHOO_SERVICE_CONFDECLINE,
	YAHOO_SERVICE_CONFLOGOFF,
	YAHOO_SERVICE_CONFADDINVITE,
	YAHOO_SERVICE_CONFMSG,
	YAHOO_SERVICE_CHATLOGON,
	YAHOO_SERVICE_CHATLOGOFF,
	YAHOO_SERVICE_CHATMSG = 0x20,
	YAHOO_SERVICE_GAMELOGON = 0x28,
	YAHOO_SERVICE_GAMELOGOFF,
	YAHOO_SERVICE_GAMEMSG = 0x2a,
	YAHOO_SERVICE_FILETRANSFER = 0x46,
	YAHOO_SERVICE_VOICECHAT = 0x4A,
	YAHOO_SERVICE_NOTIFY = 0x4B,
	YAHOO_SERVICE_VERIFY,
	YAHOO_SERVICE_P2PFILEXFER,
	YAHOO_SERVICE_PEERTOPEER = 0x4F,
	YAHOO_SERVICE_WEBCAM,
	YAHOO_SERVICE_AUTHRESP = 0x54,
	YAHOO_SERVICE_LIST = 0x55,
	YAHOO_SERVICE_AUTH = 0x57,
	YAHOO_SERVICE_AUTHBUDDY = 0x6d,
	YAHOO_SERVICE_ADDBUDDY = 0x83,
	YAHOO_SERVICE_REMBUDDY = 0x84,
	YAHOO_SERVICE_IGNORECONTACT,    /* > 1, 7, 13 < 1, 66, 13, 0*/
	YAHOO_SERVICE_REJECTCONTACT,
	YAHOO_SERVICE_GROUPRENAME = 0x89, /* > 1, 65(new), 66(0), 67(old) */
	YAHOO_SERVICE_KEEPALIVE = 0x8A,
	YAHOO_SERVICE_CHATONLINE = 0x96, /* > 109(id), 1, 6(abcde) < 0,1*/
	YAHOO_SERVICE_CHATGOTO,
	YAHOO_SERVICE_CHATJOIN, /* > 1 104-room 129-1600326591 62-2 */
	YAHOO_SERVICE_CHATLEAVE,
	YAHOO_SERVICE_CHATEXIT = 0x9b,
	YAHOO_SERVICE_CHATADDINVITE = 0x9d,
	YAHOO_SERVICE_CHATLOGOUT = 0xa0,
	YAHOO_SERVICE_CHATPING,
	YAHOO_SERVICE_COMMENT = 0xa8,
	YAHOO_SERVICE_PRESENCE_PERM = 0xb9,
	YAHOO_SERVICE_PRESENCE_SESSION = 0xba,
	YAHOO_SERVICE_AVATAR = 0xbc,
	YAHOO_SERVICE_PICTURE_CHECKSUM = 0xbd,
	YAHOO_SERVICE_PICTURE = 0xbe,
	YAHOO_SERVICE_PICTURE_UPDATE = 0xc1,
	YAHOO_SERVICE_PICTURE_UPLOAD = 0xc2,
	YAHOO_SERVICE_Y6_VISIBLE_TOGGLE = 0xc5,
	YAHOO_SERVICE_Y6_STATUS_UPDATE = 0xc6,
	YAHOO_SERVICE_AVATAR_UPDATE = 0xc7,
	YAHOO_SERVICE_VERIFY_ID_EXISTS = 0xc8,
	YAHOO_SERVICE_AUDIBLE = 0xd0,
	YAHOO_SERVICE_CONTACT_DETAILS = 0xd3,
	/* YAHOO_SERVICE_CHAT_SESSION = 0xd4,?? Reports start of chat session, gets an id from server */
	YAHOO_SERVICE_AUTH_REQ_15 = 0xd6,
	YAHOO_SERVICE_FILETRANS_15 = 0xdc,
	YAHOO_SERVICE_FILETRANS_INFO_15 = 0xdd,
	YAHOO_SERVICE_FILETRANS_ACC_15 = 0xde,
	/* photo sharing services ?? - 0xd2, 0xd7, 0xd8, 0xda */
	YAHOO_SERVICE_CHGRP_15 = 0xe7,
	YAHOO_SERVICE_STATUS_15 = 0xf0,
	YAHOO_SERVICE_LIST_15 = 0xf1,
	YAHOO_SERVICE_MESSAGE_ACK = 0xfb,
	YAHOO_SERVICE_WEBLOGIN = 0x0226,
	YAHOO_SERVICE_SMS_MSG = 0x02ea
	/* YAHOO_SERVICE_DISCONNECT = 0x07d1 Server forces us to disconnect. Is sent with TCP FIN flag set */
};

struct yahoo_pair {
	int key;
	char *value;
};

struct yahoo_packet {
	guint16 service;
	guint32 status;
	guint32 id;
	GSList *hash;
};

#define YAHOO_WEBMESSENGER_PROTO_VER 0x0065
#define YAHOO_PROTO_VER 0x0010
#define YAHOO_PROTO_VER_JAPAN 0x0010

#define YAHOO_PACKET_HDRLEN (4 + 2 + 2 + 2 + 2 + 4 + 4)

struct yahoo_packet *yahoo_packet_new(enum yahoo_service service,
				      enum yahoo_status status, int id);
void yahoo_packet_hash(struct yahoo_packet *pkt, const char *fmt, ...);
void yahoo_packet_hash_str(struct yahoo_packet *pkt, int key, const char *value);
void yahoo_packet_hash_int(struct yahoo_packet *pkt, int key, int value);
int yahoo_packet_send(struct yahoo_packet *pkt, YahooData *yd);
int yahoo_packet_send_and_free(struct yahoo_packet *pkt, YahooData *yd);
size_t yahoo_packet_build(struct yahoo_packet *pkt, int pad, gboolean wm, gboolean jp,
guchar **buf);
void yahoo_packet_read(struct yahoo_packet *pkt, const guchar *data, int len);
void yahoo_packet_write(struct yahoo_packet *pkt, guchar *data);
void yahoo_packet_dump(guchar *data, int len);
size_t yahoo_packet_length(struct yahoo_packet *pkt);
void yahoo_packet_free(struct yahoo_packet *pkt);

#endif /* _YAHOO_PACKET_H_ */
