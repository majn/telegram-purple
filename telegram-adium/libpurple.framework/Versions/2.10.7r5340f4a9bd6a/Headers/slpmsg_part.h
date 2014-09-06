/**
 * @file slpmsg_part.h MSNSLP Parts
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

#ifndef MSN_SLPMSG_PART_H
#define MSN_SLPMSG_PART_H

#include "p2p.h"

typedef struct _MsnSlpMessagePart MsnSlpMessagePart;
typedef void (*MsnSlpPartCb)(MsnSlpMessagePart *part, void *data);

struct _MsnSlpMessagePart
{
	guint ref_count;

	MsnP2PInfo *info;

	MsnSlpPartCb ack_cb;
	MsnSlpPartCb nak_cb;
	void *ack_data;

	guchar *buffer;
	size_t size;
};

MsnSlpMessagePart *msn_slpmsgpart_new(MsnP2PInfo *info);

MsnSlpMessagePart *msn_slpmsgpart_new_from_data(MsnP2PVersion p2p, const char *data, size_t data_len);

MsnSlpMessagePart *msn_slpmsgpart_ref(MsnSlpMessagePart *part);

void msn_slpmsgpart_unref(MsnSlpMessagePart *part);

void msn_slpmsgpart_set_bin_data(MsnSlpMessagePart *part, const void *data, size_t len);

char *msn_slpmsgpart_serialize(MsnSlpMessagePart *part, size_t *ret_size);

void msn_slpmsgpart_ack(MsnSlpMessagePart *part, void *data);

void msn_slpmsgpart_nak(MsnSlpMessagePart *part, void *data);

void msn_slpmsgpart_to_string(MsnSlpMessagePart *part, GString *str);

#endif /* MSN_SLPMSG_PART_H */

