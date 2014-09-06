/**
 * @file slpcall.h SLP Call functions
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
#ifndef MSN_SLPCALL_H
#define MSN_SLPCALL_H

typedef struct _MsnSlpCall MsnSlpCall;

typedef enum
{
	MSN_SLPCALL_ANY,
	MSN_SLPCALL_DC
} MsnSlpCallType;

#include "internal.h"

#include "slplink.h"

/* The official client seems to timeout slp calls after 5 minutes */
#define MSN_SLPCALL_TIMEOUT 300

struct _MsnSlpCall
{
	/* Our parent slplink */
	MsnSlpLink *slplink;

	MsnSlpCallType type;

	/* Call-ID */
	char *id;
	char *branch;

	long session_id;
	long app_id;

	gboolean pending; /**< A flag that states if we should wait for this
						slpcall to start and do not time out. */
	gboolean progress; /**< A flag that states if there has been progress since
						 the last time out. */
	gboolean wasted; /**< A flag that states if this slpcall is going to be
					   destroyed. */
	gboolean started; /**< A flag that states if this slpcall's session has
						been initiated. */

	gboolean wait_for_socket;

	void (*progress_cb)(MsnSlpCall *slpcall,
						gsize total_length, gsize len);
	void (*session_init_cb)(MsnSlpCall *slpcall);

	/* Can be checksum, or smile */
	char *data_info;

	PurpleXfer *xfer;
	union {
		GByteArray *incoming_data;
		struct {
			gsize len;
			const guchar *data;
		} outgoing;
	} u;
	MsnSlpMessage *xfer_msg; /* A dirty hack */

	MsnSlpCb cb;
	void (*end_cb)(MsnSlpCall *slpcall, MsnSession *session);

	guint timer;
};

MsnSlpCall *msn_slpcall_new(MsnSlpLink *slplink);
void msn_slpcall_init(MsnSlpCall *slpcall, MsnSlpCallType type);
void msn_slpcall_session_init(MsnSlpCall *slpcall);
void msn_slpcall_destroy(MsnSlpCall *slpcall);
void msn_slpcall_invite(MsnSlpCall *slpcall, const char *euf_guid,
						 MsnP2PAppId app_id, const char *context);
void msn_slpcall_close(MsnSlpCall *slpcall);

#endif /* MSN_SLPCALL_H */
