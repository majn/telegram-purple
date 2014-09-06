/**
 * @file user.h User functions
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
#ifndef MSN_USER_H
#define MSN_USER_H

typedef struct _MsnUser  MsnUser;

typedef enum
{
	MSN_NETWORK_UNKNOWN      = 0,
	MSN_NETWORK_PASSPORT     = 1,
	MSN_NETWORK_COMMUNICATOR = 2,
	MSN_NETWORK_MOBILE       = 4,
	MSN_NETWORK_MNI          = 8,
	MSN_NETWORK_CIRCLE       = 9,
	MSN_NETWORK_TEMP_GROUP   = 10,
	MSN_NETWORK_CID          = 11,
	MSN_NETWORK_CONNECT      = 13,
	MSN_NETWORK_REMOTE       = 14,
	MSN_NETWORK_SMTP         = 16,
	MSN_NETWORK_YAHOO        = 32
} MsnNetwork;

/**
 * Current media.
 */
typedef enum
{
	CURRENT_MEDIA_UNKNOWN,
	CURRENT_MEDIA_MUSIC,
	CURRENT_MEDIA_GAMES,
	CURRENT_MEDIA_OFFICE
} CurrentMediaType;

#include "object.h"
#include "session.h"
#include "userlist.h"

/**
 * Contains optional info about a user that is fairly uncommon.  We
 * put this info in in a separate struct to save memory because we
 * allocate an MsnUser struct for each buddy, but we generally only
 * need this information for a small percentage of our buddies
 * (usually less than 1%).  Putting it in a separate struct makes
 * MsnUser smaller by the size of a few pointers.
 */
typedef struct _MsnUserExtendedInfo
{
	CurrentMediaType media_type; /**< Type of the user's current media.   */
	char *media_title;  /**< Title of the user's current media.  */
	char *media_artist; /**< Artist of the user's current media. */
	char *media_album;  /**< Album of the user's current media.  */

	char *phone_home;   /**< E.T. uses this.                     */
	char *phone_work;   /**< Work phone number.                  */
	char *phone_mobile; /**< Mobile phone number.                */
} MsnUserExtendedInfo;

/**
 * A user.
 */
struct _MsnUser
{
	MsnUserList *userlist;

	guint8 refcount;        /**< The reference count of this object */

	char *passport;         /**< The passport account.          */
	char *friendly_name;    /**< The friendly name.             */

	char *uid;              /*< User ID                         */
	GSList *endpoints;      /*< Endpoint-specific data          */

	const char *status;     /**< The state of the user.         */
	char *statusline;       /**< The state of the user.         */

	gboolean idle;          /**< The idle state of the user.    */

	MsnUserExtendedInfo *extinfo; /**< Extended info for the user. */

	gboolean authorized;    /**< Authorized to add this user.   */
	gboolean mobile;        /**< Signed up with MSN Mobile.     */

	GList *group_ids;       /**< The group IDs.                 */
	char *pending_group;    /**< A pending group to add.        */

	MsnObject *msnobj;      /**< The user's MSN Object.         */

	GHashTable *clientcaps; /**< The client's capabilities.     */

	guint clientid;         /**< The client's ID                */
	guint extcaps;			/**< The client's extended capabilities */

	MsnNetwork networkid;   /**< The user's network             */

	MsnListOp list_op;      /**< Which lists the user is in     */

	/**
	 * The membershipId for this buddy on our pending list.  Sent by
	 * the contact's server
	 */
	guint member_id_on_pending_list;

	char *invite_message;   /**< Invite message of user request */
};

/**
 * A specific user endpoint.
 */
typedef struct MsnUserEndpoint {
	char *id;               /**< The client's endpoint ID          */
	char *name;             /**< The client's endpoint's name      */
	int type;               /**< The client's endpoint type        */
	guint clientid;         /**< The client's ID                   */
	guint extcaps;          /**< The client's extended capabilites */

} MsnUserEndpoint;

/**************************************************************************
 ** @name User API                                                        *
 **************************************************************************/
/*@{*/

/**
 * Creates a new user structure.
 *
 * @param session      The MSN session.
 * @param passport     The initial passport.
 * @param stored_name  The initial stored name.
 *
 * @return A new user structure.  It will have a reference count of 1.
 */
MsnUser *msn_user_new(MsnUserList *userlist, const char *passport,
					  const char *friendly_name);

/**
 * Increment the reference count.
 *
 * @param user 	The user.
 *
 * @return 		user.
 */
MsnUser *msn_user_ref(MsnUser *user);

/**
 * Decrement the reference count.  When the count reaches 0 the object is
 * automatically freed.
 *
 * @param user 	The user
 */
void msn_user_unref(MsnUser *user);

/**
 * Updates the user.
 *
 * Communicates with the core to update the ui, etc.
 *
 * @param user The user to update.
 */
void msn_user_update(MsnUser *user);

 /**
  *  Sets the new statusline of user.
  *
  *  @param user The user.
  *  @param state The statusline string.
  */
void msn_user_set_statusline(MsnUser *user, const char *statusline);

/**
 * Sets the new state of user.
 *
 * @param user The user.
 * @param state The state string.
 */
void msn_user_set_state(MsnUser *user, const char *state);

/**
 * Sets the passport account for a user.
 *
 * @param user     The user.
 * @param passport The passport account.
 */
void msn_user_set_passport(MsnUser *user, const char *passport);

/**
 * Sets the friendly name for a user.
 *
 * @param user The user.
 * @param name The friendly name.
 *
 * @returns TRUE is name actually changed, FALSE otherwise.
 */
gboolean msn_user_set_friendly_name(MsnUser *user, const char *name);

/**
 * Sets the buddy icon for a local user.
 *
 * @param user     The user.
 * @param img      The buddy icon image
 */
void msn_user_set_buddy_icon(MsnUser *user, PurpleStoredImage *img);

/**
 * Sets the group ID list for a user.
 *
 * @param user The user.
 * @param ids  The group ID list.
 */
void msn_user_set_group_ids(MsnUser *user, GList *ids);

/**
 * Adds the group ID for a user.
 *
 * @param user The user.
 * @param id   The group ID.
 */
void msn_user_add_group_id(MsnUser *user, const char * id);

/**
 * Removes the group ID from a user.
 *
 * @param user The user.
 * @param id   The group ID.
 */
void msn_user_remove_group_id(MsnUser *user, const char * id);

/**
 * Sets the pending group for a user.
 *
 * @param user  The user.
 * @param group The group name.
 */
void msn_user_set_pending_group(MsnUser *user, const char *group);

/**
 * Removes the pending group from a user.
 *
 * @param user The user.
 *
 * @return Returns the pending group name.
 */
char *msn_user_remove_pending_group(MsnUser *user);

/**
 * Sets the home phone number for a user.
 *
 * @param user   The user.
 * @param number The home phone number.
 */
void msn_user_set_home_phone(MsnUser *user, const char *number);

/**
 * Sets the work phone number for a user.
 *
 * @param user   The user.
 * @param number The work phone number.
 */
void msn_user_set_work_phone(MsnUser *user, const char *number);

void msn_user_set_uid(MsnUser *user, const char *uid);

/**
 * Sets endpoint data for a user.
 *
 * @param user     The user.
 * @param endpoint The endpoint.
 * @param data     The endpoint data.
 */
void
msn_user_set_endpoint_data(MsnUser *user, const char *endpoint, MsnUserEndpoint *data);

/**
 * Clears all endpoint data for a user.
 *
 * @param user     The user.
 */
void
msn_user_clear_endpoints(MsnUser *user);

/**
 * Sets the client id for a user.
 *
 * @param user     The user.
 * @param clientid The client id.
 */
void msn_user_set_clientid(MsnUser *user, guint clientid);

/**
 * Sets the client id for a user.
 *
 * @param user     The user.
 * @param extcaps  The client's extended capabilities.
 */
void msn_user_set_extcaps(MsnUser *user, guint extcaps);

/**
 * Sets the network id for a user.
 *
 * @param user    The user.
 * @param network The network id.
 */
void msn_user_set_network(MsnUser *user, MsnNetwork network);

/**
 * Sets the mobile phone number for a user.
 *
 * @param user   The user.
 * @param number The mobile phone number.
 */
void msn_user_set_mobile_phone(MsnUser *user, const char *number);

/**
 * Sets the MSNObject for a user.
 *
 * @param user The user.
 * @param obj  The MSNObject.
 */
void msn_user_set_object(MsnUser *user, MsnObject *obj);

/**
 * Sets the client information for a user.
 *
 * @param user The user.
 * @param info The client information.
 */
void msn_user_set_client_caps(MsnUser *user, GHashTable *info);

/**
 * Sets the invite message for a user.
 *
 * @param user    The user.
 * @param message The invite message for a user.
 */
void msn_user_set_invite_message(MsnUser *user, const char *message);


/**
 * Returns the passport account for a user.
 *
 * @param user The user.
 *
 * @return The passport account.
 */
const char *msn_user_get_passport(const MsnUser *user);

/**
 * Returns the friendly name for a user.
 *
 * @param user The user.
 *
 * @return The friendly name.
 */
const char *msn_user_get_friendly_name(const MsnUser *user);

/**
 * Returns the home phone number for a user.
 *
 * @param user The user.
 *
 * @return The user's home phone number.
 */
const char *msn_user_get_home_phone(const MsnUser *user);

/**
 * Returns the work phone number for a user.
 *
 * @param user The user.
 *
 * @return The user's work phone number.
 */
const char *msn_user_get_work_phone(const MsnUser *user);

/**
 * Returns the mobile phone number for a user.
 *
 * @param user The user.
 *
 * @return The user's mobile phone number.
 */
const char *msn_user_get_mobile_phone(const MsnUser *user);

/**
 * Gets endpoint data for a user.
 *
 * @param user     The user.
 * @param endpoint The endpoint.
 *
 * @return The user's endpoint data.
 */
MsnUserEndpoint *
msn_user_get_endpoint_data(MsnUser *user, const char *endpoint);

/**
 * Returns the client id for a user.
 *
 * @param user    The user.
 *
 * @return The user's client id.
 */
guint msn_user_get_clientid(const MsnUser *user);

/**
 * Returns the extended capabilities for a user.
 *
 * @param user    The user.
 *
 * @return The user's extended capabilities.
 */
guint msn_user_get_extcaps(const MsnUser *user);

/**************************************************************************
 * Utility functions
 **************************************************************************/


/**
 * Check if the user is part of the group.
 *
 * @param user 		The user we are asking group membership.
 * @param group_id 	The group where the user may be in.
 *
 * @return TRUE if user is part of the group. Otherwise, FALSE.
 */
gboolean msn_user_is_in_group(MsnUser *user, const char * group_id);

/**
 * Check if user is on list.
 *
 * @param user 		The user we are asking list membership.
 * @param list_id 	The list where the user may be in.
 *
 * @return TRUE if the user is on the list, else FALSE.
 */
gboolean msn_user_is_in_list(MsnUser *user, MsnListId list_id);
/**
 * Returns the network id for a user.
 *
 * @param user    The user.
 *
 * @return The user's network id.
 */
MsnNetwork msn_user_get_network(const MsnUser *user);

/**
 * Returns the MSNObject for a user.
 *
 * @param user The user.
 *
 * @return The MSNObject.
 */
MsnObject *msn_user_get_object(const MsnUser *user);

/**
 * Returns the client information for a user.
 *
 * @param user The user.
 *
 * @return The client information.
 */
GHashTable *msn_user_get_client_caps(const MsnUser *user);

/**
 * Returns the invite message for a user.
 *
 * @param user The user.
 *
 * @return The user's invite message.
 */
const char *msn_user_get_invite_message(const MsnUser *user);

/**
 * check to see if user is online
 */
gboolean msn_user_is_online(PurpleAccount *account, const char *name);

/**
 * check to see if user is Yahoo User
 */
gboolean msn_user_is_yahoo(PurpleAccount *account, const char *name);

void msn_user_set_op(MsnUser *user, MsnListOp list_op);
void msn_user_unset_op(MsnUser *user, MsnListOp list_op);

/**
 * Compare the given passport with the one of the user
 *
 * @param user 	User to compare.
 * @oaran passport 	Passport to compare.
 *
 * @return Zero if the passport match with the one of the user, otherwise
 * a positive integer if the user passport is greather than the one given
 * and a negative integer if it is less.
 */
int msn_user_passport_cmp(MsnUser *user, const char *passport);

/**
 * Checks whether a user is capable of some task.
 *
 * @param user       The user.
 * @param endpoint   The endpoint. Can be @NULL to check overall capabilities.
 * @param capability The capability (including client version).
 * @param extcap     The extended capability.
 *
 * @return Whether the user supports the capability.
 */
gboolean
msn_user_is_capable(MsnUser *user, char *endpoint, guint capability, guint extcap);

/*@}*/

#endif /* MSN_USER_H */
