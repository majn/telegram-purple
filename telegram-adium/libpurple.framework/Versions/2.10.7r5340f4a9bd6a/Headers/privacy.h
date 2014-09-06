/**
 * @file privacy.h Privacy API
 * @ingroup core
 */

/* purple
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
#ifndef _PURPLE_PRIVACY_H_
#define _PURPLE_PRIVACY_H_

/**
 * Privacy data types.
 */
typedef enum _PurplePrivacyType
{
	PURPLE_PRIVACY_ALLOW_ALL = 1,
	PURPLE_PRIVACY_DENY_ALL,
	PURPLE_PRIVACY_ALLOW_USERS,
	PURPLE_PRIVACY_DENY_USERS,
	PURPLE_PRIVACY_ALLOW_BUDDYLIST
} PurplePrivacyType;

#include "account.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Privacy core/UI operations.
 */
typedef struct
{
	void (*permit_added)(PurpleAccount *account, const char *name);
	void (*permit_removed)(PurpleAccount *account, const char *name);
	void (*deny_added)(PurpleAccount *account, const char *name);
	void (*deny_removed)(PurpleAccount *account, const char *name);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
} PurplePrivacyUiOps;

/**
 * Adds a user to the account's permit list.
 *
 * @param account    The account.
 * @param name       The name of the user to add to the list.
 * @param local_only If TRUE, only the local list is updated, and not
 *                   the server.
 *
 * @return TRUE if the user was added successfully, or @c FALSE otherwise.
 */
gboolean purple_privacy_permit_add(PurpleAccount *account, const char *name,
								 gboolean local_only);

/**
 * Removes a user from the account's permit list.
 *
 * @param account    The account.
 * @param name       The name of the user to add to the list.
 * @param local_only If TRUE, only the local list is updated, and not
 *                   the server.
 *
 * @return TRUE if the user was removed successfully, or @c FALSE otherwise.
 */
gboolean purple_privacy_permit_remove(PurpleAccount *account, const char *name,
									gboolean local_only);

/**
 * Adds a user to the account's deny list.
 *
 * @param account    The account.
 * @param name       The name of the user to add to the list.
 * @param local_only If TRUE, only the local list is updated, and not
 *                   the server.
 *
 * @return TRUE if the user was added successfully, or @c FALSE otherwise.
 */
gboolean purple_privacy_deny_add(PurpleAccount *account, const char *name,
							   gboolean local_only);

/**
 * Removes a user from the account's deny list.
 *
 * @param account    The account.
 * @param name       The name of the user to add to the list.
 * @param local_only If TRUE, only the local list is updated, and not
 *                   the server.
 *
 * @return TRUE if the user was removed successfully, or @c FALSE otherwise.
 */
gboolean purple_privacy_deny_remove(PurpleAccount *account, const char *name,
								  gboolean local_only);

/**
 * Allow a user to send messages. If current privacy setting for the account is:
 *		PURPLE_PRIVACY_ALLOW_USERS:	The user is added to the allow-list.
 *		PURPLE_PRIVACY_DENY_USERS	:	The user is removed from the deny-list.
 *		PURPLE_PRIVACY_ALLOW_ALL	:	No changes made.
 *		PURPLE_PRIVACY_DENY_ALL	:	The privacy setting is changed to
 *									PURPLE_PRIVACY_ALLOW_USERS and the user
 *									is added to the allow-list.
 *		PURPLE_PRIVACY_ALLOW_BUDDYLIST: No changes made if the user is already in
 *									the buddy-list. Otherwise the setting is
 *									changed to PURPLE_PRIVACY_ALLOW_USERS, all the
 *									buddies are added to the allow-list, and the
 *									user is also added to the allow-list.
 *
 * @param account	The account.
 * @param who		The name of the user.
 * @param local		Whether the change is local-only.
 * @param restore	Should the previous allow/deny list be restored if the
 *					privacy setting is changed.
 */
void purple_privacy_allow(PurpleAccount *account, const char *who, gboolean local,
						gboolean restore);

/**
 * Block messages from a user. If current privacy setting for the account is:
 *		PURPLE_PRIVACY_ALLOW_USERS:	The user is removed from the allow-list.
 *		PURPLE_PRIVACY_DENY_USERS	:	The user is added to the deny-list.
 *		PURPLE_PRIVACY_DENY_ALL	:	No changes made.
 *		PURPLE_PRIVACY_ALLOW_ALL	:	The privacy setting is changed to
 *									PURPLE_PRIVACY_DENY_USERS and the user is
 *									added to the deny-list.
 *		PURPLE_PRIVACY_ALLOW_BUDDYLIST: If the user is not in the buddy-list,
 *									then no changes made. Otherwise, the setting
 *									is changed to PURPLE_PRIVACY_ALLOW_USERS, all
 *									the buddies are added to the allow-list, and
 *									this user is removed from the list.
 *
 * @param account	The account.
 * @param who		The name of the user.
 * @param local		Whether the change is local-only.
 * @param restore	Should the previous allow/deny list be restored if the
 *					privacy setting is changed.
 */
void purple_privacy_deny(PurpleAccount *account, const char *who, gboolean local,
						gboolean restore);

/**
 * Check the privacy-setting for a user.
 *
 * @param account	The account.
 * @param who		The name of the user.
 *
 * @return @c FALSE if the specified account's privacy settings block the user or @c TRUE otherwise. The meaning of "block" is protocol-dependent and generally relates to status and/or sending of messages.
 */
gboolean purple_privacy_check(PurpleAccount *account, const char *who);

/**
 * Sets the UI operations structure for the privacy subsystem.
 *
 * @param ops The UI operations structure.
 */
void purple_privacy_set_ui_ops(PurplePrivacyUiOps *ops);

/**
 * Returns the UI operations structure for the privacy subsystem.
 *
 * @return The UI operations structure.
 */
PurplePrivacyUiOps *purple_privacy_get_ui_ops(void);

/**
 * Initializes the privacy subsystem.
 */
void purple_privacy_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_PRIVACY_H_ */
