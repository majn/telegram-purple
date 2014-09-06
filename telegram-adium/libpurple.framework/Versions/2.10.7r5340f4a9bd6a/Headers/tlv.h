/**
 * @file tlv.h MSN TLV functions
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

#ifndef MSN_TLV_H
#define MSN_TLV_H

#include "msn.h"

/* TLV structure */
typedef struct msn_tlv_s
{
	guint8 type;
	guint8 length;
	guint8 *value;
} msn_tlv_t;

/* TLV handling functions */
char *msn_tlv_getvalue_as_string(msn_tlv_t *tlv);

msn_tlv_t *msn_tlv_gettlv(GSList *list, const guint8 type, const int nth);
int msn_tlv_getlength(GSList *list, const guint8 type, const int nth);
char *msn_tlv_getstr(GSList *list, const guint8 type, const int nth);
guint8 msn_tlv_get8(GSList *list, const guint8 type, const int nth);
guint16 msn_tlv_get16(GSList *list, const guint8 type, const int nth);
guint32 msn_tlv_get32(GSList *list, const guint8 type, const int nth);

/* TLV list handling functions */
GSList *msn_tlvlist_read(const char *bs, size_t bs_len);
GSList *msn_tlvlist_copy(GSList *orig);

int msn_tlvlist_count(GSList *list);
size_t msn_tlvlist_size(GSList *list);
gboolean msn_tlvlist_equal(GSList *one, GSList *two);
char *msn_tlvlist_write(GSList *list, size_t *out_len);
void msn_tlvlist_free(GSList *list);

int msn_tlvlist_add_raw(GSList **list, const guint8 type, const guint8 length, const char *value);
int msn_tlvlist_add_empty(GSList **list, const guint8 type);
int msn_tlvlist_add_8(GSList **list, const guint8 type, const guint8 value);
int msn_tlvlist_add_16(GSList **list, const guint8 type, const guint16 value);
int msn_tlvlist_add_32(GSList **list, const guint8 type, const guint32 value);
int msn_tlvlist_add_str(GSList **list, const guint8 type, const char *value);
int msn_tlvlist_add_tlv(GSList **list, const msn_tlv_t *tlv);

int msn_tlvlist_replace_raw(GSList **list, const guint8 type, const guint8 lenth, const char *value);
int msn_tlvlist_replace_str(GSList **list, const guint8 type, const char *str);
int msn_tlvlist_replace_empty(GSList **list, const guint8 type);
int msn_tlvlist_replace_8(GSList **list, const guint8 type, const guint8 value);
int msn_tlvlist_replace_16(GSList **list, const guint8 type, const guint16 value);
int msn_tlvlist_replace_32(GSList **list, const guint8 type, const guint32 value);
int msn_tlvlist_replace_tlv(GSList **list, const msn_tlv_t *tlv);

void msn_tlvlist_remove(GSList **list, const guint8 type);

#endif /* MSN_TLV_H */

