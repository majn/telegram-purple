/**
 * @file slpmsg.h SLP Message functions
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
#ifndef _MSN_SLPMSG_H_
#define _MSN_SLPMSG_H_

typedef struct _MsnSlpMessage MsnSlpMessage;

#include "imgstore.h"

#include "slpcall.h"
#include "slplink.h"
#include "session.h"
#include "p2p.h"

#include "slp.h"

/**
 * A SLP Message  This contains everything that we will need to send a SLP
 * Message even if has to be sent in several parts.
 */
struct _MsnSlpMessage
{
	MsnSlpCall *slpcall; /**< The slpcall to which this slp message belongs (if applicable). */
	MsnSlpLink *slplink; /**< The slplink through which this slp message is being sent. */
	MsnSession *session;

	MsnP2PInfo *p2p_info;

	long id;

	gboolean ft;
	PurpleStoredImage *img;
	guchar *buffer;

	/**
	 * This is the size of buffer, unless this is an outgoing file transfer,
	 * in which case this is the size of the file.
	 */
	long long size;

	GList *parts; /**< A list with the SlpMsgParts */

	const char *info;
	gboolean text_body;
};

/**
 * Creates a new slp message
 *
 * @param slplink The slplink through which this slp message will be sent.
 * If it's set to NULL, it is a temporary SlpMessage.
 * @return The created slp message.
 */
MsnSlpMessage *msn_slpmsg_new(MsnSlpLink *slplink, MsnSlpCall *slpcall);

/**
 * Destroys a slp message
 *
 * @param slpmsg The slp message to destory.
 */
void msn_slpmsg_destroy(MsnSlpMessage *slpmsg);

/**
 * Relate this SlpMessage with an existing SlpLink
 *
 * @param slplink 	The SlpLink that will send this message.
 */
void msn_slpmsg_set_slplink(MsnSlpMessage *slpmsg, MsnSlpLink *slplink);

void msn_slpmsg_set_body(MsnSlpMessage *slpmsg, const char *body,
						 long long size);
void msn_slpmsg_set_image(MsnSlpMessage *slpmsg, PurpleStoredImage *img);
MsnSlpMessage * msn_slpmsg_sip_new(MsnSlpCall *slpcall, int cseq,
								   const char *header,
								   const char *branch,
								   const char *content_type,
								   const char *content);

/**
 * Create a new SLP Ack message
 *
 * @param header the value of the header in this slpmsg.
 *
 * @return A new SlpMessage with ACK headers
 */
MsnSlpMessage *msn_slpmsg_ack_new(MsnSlpLink *slplink, MsnP2PInfo *info);

/**
 * Create a new SLP message for MsnObject data.
 *
 * @param slpcall 	The slpcall that manages this message.
 * @param img 		The image to be sent in this message.
 *
 * @return A new SlpMessage with MsnObject info.
 */
MsnSlpMessage *msn_slpmsg_obj_new(MsnSlpCall *slpcall, PurpleStoredImage *img);

/**
 * Create a new SLP message for data preparation.
 *
 * @param slpcall 	The slpcall that manages this message.
 *
 * @return A new SlpMessage with data preparation info.
 */
MsnSlpMessage *msn_slpmsg_dataprep_new(MsnSlpCall *slpcall);

/**
 * Create a new SLP message for File transfer.
 *
 * @param slpcall 	The slpcall that manages this message.
 * @param size 		The size of the file being transsmited.
 *
 * @return A new SlpMessage with the file transfer info.
 */
MsnSlpMessage *msn_slpmsg_file_new(MsnSlpCall *slpcall, size_t size);

/**
 * Serialize the MsnSlpMessage in a way it can be used to be transmited
 *
 * @param slpmsg 	The MsnSlpMessage.
 * @param ret_size 	The size of the buffer cointaining the message.
 *
 * @return a buffer with the serialized data.
 */
char *msn_slpmsg_serialize(MsnSlpMessage *slpmsg, size_t *ret_size);

void msn_slpmsg_show_readable(MsnSlpMessage *slpmsg);

#endif /* _MSN_SLPMSG_H_ */
