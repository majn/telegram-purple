/**
 * @file group.h Group functions
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
#ifndef MSN_GROUP_H
#define MSN_GROUP_H

typedef struct _MsnGroup  MsnGroup;

#include "internal.h"

#include "session.h"
#include "user.h"
#include "userlist.h"

#define MSN_INDIVIDUALS_GROUP_ID	"1983"
#define MSN_INDIVIDUALS_GROUP_NAME	_("Other Contacts")

#define MSN_NON_IM_GROUP_ID		"email"
#define MSN_NON_IM_GROUP_NAME	_("Non-IM Contacts")

/**
 * A group.
 */
struct _MsnGroup
{
	MsnSession *session;    /**< The MSN session.           */

	char *id;                 /**< The group ID.              */
	char *name;             /**< The name of the group.     */
};

/**************************************************************************
 ** @name Group API                                                       *
 **************************************************************************/
/*@{*/

/**
 * Creates a new group structure.
 *
 * @param session The MSN session.
 * @param id      The group ID.
 * @param name    The name of the group.
 *
 * @return A new group structure.
 */
MsnGroup *msn_group_new(MsnUserList *userlist, const char *id, const char *name);

/**
 * Destroys a group structure.
 *
 * @param group The group to destroy.
 */
void msn_group_destroy(MsnGroup *group);

/**
 * Sets the ID for a group.
 *
 * @param group The group.
 * @param id    The ID.
 */
void msn_group_set_id(MsnGroup *group, const char *id);

/**
 * Sets the name for a group.
 *
 * @param group The group.
 * @param name  The name.
 */
void msn_group_set_name(MsnGroup *group, const char *name);

/**
 * Returns the ID for a group.
 *
 * @param group The group.
 *
 * @return The ID.
 */
char* msn_group_get_id(const MsnGroup *group);

/**
 * Returns the name for a group.
 *
 * @param group The group.
 *
 * @return The name.
 */
const char *msn_group_get_name(const MsnGroup *group);

#endif /* MSN_GROUP_H */
