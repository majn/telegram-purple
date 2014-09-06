/**
 * @file buddylist.h
 *
 * purple
 *
 * Copyright (C) 2005  Bartosz Oler <bartosz@bzimage.us>
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


#ifndef _PURPLE_GG_BUDDYLIST_H
#define _PURPLE_GG_BUDDYLIST_H

#include "connection.h"
#include "account.h"

void
ggp_buddylist_send(PurpleConnection *gc);

/**
 * Load buddylist from server into the roster.
 *
 * @param gc PurpleConnection
 * @param buddylist Pointer to the buddylist that will be loaded.
 */
/* void ggp_buddylist_load(PurpleConnection *gc, char *buddylist) {{{ */
void
ggp_buddylist_load(PurpleConnection *gc, char *buddylist);

/**
 * Get all the buddies in the current account.
 *
 * @param account Current account.
 *
 * @return List of buddies.
 */
char *
ggp_buddylist_dump(PurpleAccount *account);


#endif /* _PURPLE_GG_BUDDYLIST_H */


/* vim: set ts=8 sts=0 sw=8 noet: */
