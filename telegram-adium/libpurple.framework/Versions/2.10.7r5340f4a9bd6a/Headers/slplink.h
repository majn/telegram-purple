/**
 * @file slplink.h MSNSLP Link support
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
#ifndef MSN_SLPLINK_H
#define MSN_SLPLINK_H

typedef struct _MsnSlpLink MsnSlpLink;

#include "directconn.h"
#include "session.h"
#include "slpcall.h"
#include "slpmsg.h"
#include "switchboard.h"

typedef void (*MsnSlpCb)(MsnSlpCall *slpcall,
						 const guchar *data, gsize size);
typedef void (*MsnSlpEndCb)(MsnSlpCall *slpcall, MsnSession *session);

struct _MsnSlpLink
{
	MsnSession *session;
	MsnSwitchBoard *swboard;
	MsnDirectConn *dc;

	guint refs;

	char *remote_user;
	MsnP2PVersion p2p_version;

	int slp_seq_id;

	GList *slp_calls;
	GList *slp_msgs;

	GQueue *slp_msg_queue;
};

MsnSlpLink *msn_slplink_ref(MsnSlpLink *slplink);
void msn_slplink_unref(MsnSlpLink *slplink);

/**
 * @return An MsnSlpLink for the given user, or NULL if there is no
 *         existing MsnSlpLink.
 */
MsnSlpLink *msn_session_find_slplink(MsnSession *session,
									 const char *who);

/**
 * @return An MsnSlpLink for the given user.  One will be created if
 *         it does not already exist.
 */
MsnSlpLink *msn_session_get_slplink(MsnSession *session, const char *username);

void msn_slplink_add_slpcall(MsnSlpLink *slplink, MsnSlpCall *slpcall);
void msn_slplink_remove_slpcall(MsnSlpLink *slplink, MsnSlpCall *slpcall);
MsnSlpCall *msn_slplink_find_slp_call(MsnSlpLink *slplink,
									  const char *id);
MsnSlpCall *msn_slplink_find_slp_call_with_session_id(MsnSlpLink *slplink, long id);
MsnP2PVersion msn_slplink_get_p2p_version(MsnSlpLink *slplink);

void msn_slplink_queue_slpmsg(MsnSlpLink *slplink, MsnSlpMessage *slpmsg);
void msn_slplink_send_slpmsg(MsnSlpLink *slplink,
							 MsnSlpMessage *slpmsg);
void msn_slplink_send_queued_slpmsgs(MsnSlpLink *slplink);
void msn_slplink_process_msg(MsnSlpLink *slplink, MsnSlpMessagePart *part);

/* Only exported for msn_xfer_write */
void msn_slplink_send_msgpart(MsnSlpLink *slplink, MsnSlpMessage *slpmsg);

void msn_slplink_request_object(MsnSlpLink *slplink,
								const char *info,
								MsnSlpCb cb,
								MsnSlpEndCb end_cb,
								const MsnObject *obj);

MsnSlpCall *msn_slp_process_msg(MsnSlpLink *slplink, MsnSlpMessage *slpmsg);

#endif /* MSN_SLPLINK_H */
