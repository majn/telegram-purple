/**
 * @file yahoochat.h The Yahoo! protocol plugin, chat and conference stuff
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

#ifndef _YAHOOCHAT_H_
#define _YAHOOCHAT_H_

#include "roomlist.h"
#include "yahoo_packet.h"

void yahoo_process_conference_invite(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_conference_decline(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_conference_logon(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_conference_logoff(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_conference_message(PurpleConnection *gc, struct yahoo_packet *pkt);

void yahoo_process_chat_online(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_logout(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_join(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_exit(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_message(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_addinvite(PurpleConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_goto(PurpleConnection *gc, struct yahoo_packet *pkt);

void yahoo_c_leave(PurpleConnection *gc, int id);
int yahoo_c_send(PurpleConnection *gc, int id, const char *what, PurpleMessageFlags flags);
GList *yahoo_c_info(PurpleConnection *gc);
GHashTable *yahoo_c_info_defaults(PurpleConnection *gc, const char *chat_name);
void yahoo_c_join(PurpleConnection *gc, GHashTable *data);
char *yahoo_get_chat_name(GHashTable *data);
void yahoo_c_invite(PurpleConnection *gc, int id, const char *msg, const char *name);

void yahoo_conf_leave(YahooData *yd, const char *room, const char *dn, GList *who);

void yahoo_chat_goto(PurpleConnection *gc, const char *name);

/* room listing functions */
PurpleRoomlist *yahoo_roomlist_get_list(PurpleConnection *gc);
void yahoo_roomlist_cancel(PurpleRoomlist *list);
void yahoo_roomlist_expand_category(PurpleRoomlist *list, PurpleRoomlistRoom *category);

/* util */
void yahoo_chat_add_users(PurpleConvChat *chat, GList *newusers);
void yahoo_chat_add_user(PurpleConvChat *chat, const char *user, const char *reason);

#endif /* _YAHOO_CHAT_H_ */
