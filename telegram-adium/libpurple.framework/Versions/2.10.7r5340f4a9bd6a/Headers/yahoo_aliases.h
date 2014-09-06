/*
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
 *
 */


#include "internal.h"

#include "account.h"
#include "accountopt.h"
#include "blist.h"
#include "debug.h"
#include "util.h"
#include "version.h"
#include "libymsg.h"
#include "yahoo_packet.h"

void yahoo_update_alias(PurpleConnection *gc, const char *who, const char *alias);
void yahoo_fetch_aliases(PurpleConnection *gc);
void yahoo_set_userinfo(PurpleConnection *gc);
void yahoo_set_userinfo_for_buddy(PurpleConnection *gc, PurpleBuddy *buddy);
void yahoo_personal_details_reset(YahooPersonalDetails *ypd, gboolean all);
void yahoo_process_contact_details(PurpleConnection *gc, struct yahoo_packet *pkt);
