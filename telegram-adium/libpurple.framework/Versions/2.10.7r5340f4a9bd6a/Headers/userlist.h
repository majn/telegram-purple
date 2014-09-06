/**
 * @file userlist.h MSN user list support
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
#ifndef MSN_USERLIST_H
#define MSN_USERLIST_H

typedef struct _MsnUserList MsnUserList;

typedef enum
{
	MSN_LIST_FL, /**< Forward list */
	MSN_LIST_AL, /**< Allow list */
	MSN_LIST_BL, /**< Block list */
	MSN_LIST_RL, /**< Reverse list */
	MSN_LIST_PL  /**< Pending list */
} MsnListId;

typedef enum
{
	MSN_LIST_FL_OP = 0x01,
	MSN_LIST_AL_OP = 0x02,
	MSN_LIST_BL_OP = 0x04,
	MSN_LIST_RL_OP = 0x08,
	MSN_LIST_PL_OP = 0x10
} MsnListOp;
#define MSN_LIST_OP_MASK	0x07

#include "group.h"
#include "msn.h"
#include "user.h"

struct _MsnUserList
{
	MsnSession *session;

	GList *users; /* Contains MsnUsers */
	GList *groups; /* Contains MsnGroups */

	GQueue *buddy_icon_requests;
	int buddy_icon_window;
	guint buddy_icon_request_timer;

};

void msn_got_lst_user(MsnSession *session, MsnUser *user,
					  MsnListOp list_op, GSList *group_ids);

MsnUserList *msn_userlist_new(MsnSession *session);
void msn_userlist_destroy(MsnUserList *userlist);

void msn_userlist_add_user(MsnUserList *userlist, MsnUser *user);
void msn_userlist_remove_user(MsnUserList *userlist, MsnUser *user);

MsnUser * msn_userlist_find_user(MsnUserList *userlist, const char *passport);
MsnUser * msn_userlist_find_add_user(MsnUserList *userlist,
				const char *passport, const char *friendly_name);
MsnUser * msn_userlist_find_user_with_id(MsnUserList *userlist, const char *uid);
MsnUser * msn_userlist_find_user_with_mobile_phone(MsnUserList *userlist, const char *number);

void msn_userlist_add_group(MsnUserList *userlist, MsnGroup *group);
void msn_userlist_remove_group(MsnUserList *userlist, MsnGroup *group);
MsnGroup *msn_userlist_find_group_with_id(MsnUserList *userlist, const char *id);
MsnGroup *msn_userlist_find_group_with_name(MsnUserList *userlist, const char *name);
const char * msn_userlist_find_group_id(MsnUserList *userlist,
					const char *group_name);
const char *msn_userlist_find_group_name(MsnUserList *userlist, const char *group_id);
void msn_userlist_rename_group_id(MsnUserList *userlist, const char *group_id,
				  const char *new_name);
void msn_userlist_remove_group_id(MsnUserList *userlist, const char *group_id);

void msn_userlist_rem_buddy(MsnUserList *userlist, const char *who);
void msn_userlist_add_buddy(MsnUserList *userlist,
			    const char *who, const char *group_name);
void msn_userlist_move_buddy(MsnUserList *userlist, const char *who,
						    const char *old_group_name,
						    const char *new_group_name);

gboolean msn_userlist_add_buddy_to_group(MsnUserList *userlist, const char *who,
					 const char *group_name);
gboolean msn_userlist_rem_buddy_from_group(MsnUserList *userlist,
					   const char *who,
					   const char *group_name);

void msn_userlist_add_buddy_to_list(MsnUserList *userlist, const char *who,
				    MsnListId list_id);
void msn_userlist_rem_buddy_from_list(MsnUserList *userlist, const char *who,
				      MsnListId list_id);
void msn_release_buddy_icon_request(MsnUserList *userlist);

void msn_userlist_load(MsnSession *session);

#endif /* MSN_USERLIST_H */
