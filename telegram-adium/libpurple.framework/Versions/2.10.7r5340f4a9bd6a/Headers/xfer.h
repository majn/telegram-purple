/**
 * @file xfer.h MSN File Transfer functions
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

#ifndef MSN_XFER_H
#define MSN_XFER_H

#include "slpcall.h"

#define MAX_FILE_NAME_LEN 260 /* MAX_PATH in Windows */

/**
 * The context data for a file transfer request
 */
typedef struct
{
	guint32   length;       /*< Length of header */
	guint32   version;      /*< MSN version */
	guint64   file_size;    /*< Size of file */
	guint32   type;         /*< Transfer type */
	gunichar2 file_name[MAX_FILE_NAME_LEN]; /*< Self-explanatory */
	gchar     unknown1[30]; /*< Used somehow for background sharing */
	guint32   unknown2;     /*< Possibly for background sharing as well */
	gchar     *preview;     /*< File preview data, 96x96 PNG */
	gsize     preview_len;
} MsnFileContext;

#define MSN_FILE_CONTEXT_SIZE (4*4 + 1*8 + 2*MAX_FILE_NAME_LEN + 30)

void msn_xfer_init(PurpleXfer *xfer);
void msn_xfer_cancel(PurpleXfer *xfer);

gssize msn_xfer_write(const guchar *data, gsize len, PurpleXfer *xfer);
gssize msn_xfer_read(guchar **data, PurpleXfer *xfer);

void msn_xfer_completed_cb(MsnSlpCall *slpcall,
						   const guchar *body, gsize size);
void msn_xfer_end_cb(MsnSlpCall *slpcall, MsnSession *session);

gchar *
msn_file_context_to_wire(MsnFileContext *context);

MsnFileContext *
msn_file_context_from_wire(const char *buf, gsize len);

#endif /* MSN_XFER_H */

