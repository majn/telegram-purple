/**
 * @file account.h Account API
 * @ingroup core
 * @see @ref account-signals
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
#ifndef _PURPLE_ACCOUNT_H_
#define _PURPLE_ACCOUNT_H_

#include <glib.h>
#include <glib-object.h>

/** @copydoc _PurpleAccountUiOps */
typedef struct _PurpleAccountUiOps PurpleAccountUiOps;
/** @copydoc _PurpleAccount */
typedef struct _PurpleAccount      PurpleAccount;

typedef gboolean (*PurpleFilterAccountFunc)(PurpleAccount *account);
typedef void (*PurpleAccountRequestAuthorizationCb)(void *);
typedef void (*PurpleAccountRegistrationCb)(PurpleAccount *account, gboolean succeeded, void *user_data);
typedef void (*PurpleAccountUnregistrationCb)(PurpleAccount *account, gboolean succeeded, void *user_data);
typedef void (*PurpleSetPublicAliasSuccessCallback)(PurpleAccount *account, const char *new_alias);
typedef void (*PurpleSetPublicAliasFailureCallback)(PurpleAccount *account, const char *error);
typedef void (*PurpleGetPublicAliasSuccessCallback)(PurpleAccount *account, const char *alias);
typedef void (*PurpleGetPublicAliasFailureCallback)(PurpleAccount *account, const char *error);

#include "connection.h"
#include "log.h"
#include "privacy.h"
#include "proxy.h"
#include "prpl.h"
#include "status.h"

/**
 * Account request types.
 */
typedef enum
{
	PURPLE_ACCOUNT_REQUEST_AUTHORIZATION = 0 /* Account authorization request */
} PurpleAccountRequestType;

/**
 * Account request response types
 */
typedef enum
{
	PURPLE_ACCOUNT_RESPONSE_IGNORE = -2,
	PURPLE_ACCOUNT_RESPONSE_DENY = -1,
	PURPLE_ACCOUNT_RESPONSE_PASS = 0,
	PURPLE_ACCOUNT_RESPONSE_ACCEPT = 1
} PurpleAccountRequestResponse;

/**  Account UI operations, used to notify the user of status changes and when
 *   buddies add this account to their buddy lists.
 */
struct _PurpleAccountUiOps
{
	/** A buddy who is already on this account's buddy list added this account
	 *  to their buddy list.
	 */
	void (*notify_added)(PurpleAccount *account,
	                     const char *remote_user,
	                     const char *id,
	                     const char *alias,
	                     const char *message);

	/** This account's status changed. */
	void (*status_changed)(PurpleAccount *account,
	                       PurpleStatus *status);

	/** Someone we don't have on our list added us; prompt to add them. */
	void (*request_add)(PurpleAccount *account,
	                    const char *remote_user,
	                    const char *id,
	                    const char *alias,
	                    const char *message);

	/** Prompt for authorization when someone adds this account to their buddy
	 * list.  To authorize them to see this account's presence, call \a
	 * authorize_cb (\a user_data); otherwise call \a deny_cb (\a user_data);
	 * @return a UI-specific handle, as passed to #close_account_request.
	 */
	void *(*request_authorize)(PurpleAccount *account,
	                           const char *remote_user,
	                           const char *id,
	                           const char *alias,
	                           const char *message,
	                           gboolean on_list,
	                           PurpleAccountRequestAuthorizationCb authorize_cb,
	                           PurpleAccountRequestAuthorizationCb deny_cb,
	                           void *user_data);

	/** Close a pending request for authorization.  \a ui_handle is a handle
	 *  as returned by #request_authorize.
	 */
	void (*close_account_request)(void *ui_handle);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

/** Structure representing an account.
 */
struct _PurpleAccount
{
	char *username;             /**< The username.                          */
	char *alias;                /**< How you appear to yourself.            */
	char *password;             /**< The account password.                  */
	char *user_info;            /**< User information.                      */

	char *buddy_icon_path;      /**< The buddy icon's non-cached path.      */

	gboolean remember_pass;     /**< Remember the password.                 */

	char *protocol_id;          /**< The ID of the protocol.                */

	PurpleConnection *gc;         /**< The connection handle.                 */
	gboolean disconnecting;     /**< The account is currently disconnecting */

	GHashTable *settings;       /**< Protocol-specific settings.            */
	GHashTable *ui_settings;    /**< UI-specific settings.                  */

	PurpleProxyInfo *proxy_info;  /**< Proxy information.  This will be set   */
								/*   to NULL when the account inherits      */
								/*   proxy settings from global prefs.      */

	/*
	 * TODO: Supplementing the next two linked lists with hash tables
	 * should help performance a lot when these lists are long.  This
	 * matters quite a bit for protocols like MSN, where all your
	 * buddies are added to your permit list.  Currently we have to
	 * iterate through the entire list if we want to check if someone
	 * is permitted or denied.  We should do this for 3.0.0.
	 * Or maybe use a GTree.
	 */
	GSList *permit;             /**< Permit list.                           */
	GSList *deny;               /**< Deny list.                             */
	PurplePrivacyType perm_deny;  /**< The permit/deny setting.               */

	GList *status_types;        /**< Status types.                          */

	PurplePresence *presence;     /**< Presence.                              */
	PurpleLog *system_log;        /**< The system log                         */

	void *ui_data;              /**< The UI can put data here.              */
	PurpleAccountRegistrationCb registration_cb;
	void *registration_cb_user_data;

	gpointer priv;              /**< Pointer to opaque private data. */
};

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Account API                                                     */
/**************************************************************************/
/*@{*/

/**
 * Creates a new account.
 *
 * @param username    The username.
 * @param protocol_id The protocol ID.
 *
 * @return The new account.
 */
PurpleAccount *purple_account_new(const char *username, const char *protocol_id);

/**
 * Destroys an account.
 *
 * @param account The account to destroy.
 */
void purple_account_destroy(PurpleAccount *account);

/**
 * Connects to an account.
 *
 * @param account The account to connect to.
 */
void purple_account_connect(PurpleAccount *account);

/**
 * Sets the callback for successful registration.
 *
 * @param account	The account for which this callback should be used
 * @param cb	The callback
 * @param user_data	The user data passed to the callback
 */
void purple_account_set_register_callback(PurpleAccount *account, PurpleAccountRegistrationCb cb, void *user_data);

/**
 * Registers an account.
 *
 * @param account The account to register.
 */
void purple_account_register(PurpleAccount *account);

/**
 * Unregisters an account (deleting it from the server).
 *
 * @param account The account to unregister.
 * @param cb Optional callback to be called when unregistration is complete
 * @param user_data user data to pass to the callback
 */
void purple_account_unregister(PurpleAccount *account, PurpleAccountUnregistrationCb cb, void *user_data);

/**
 * Disconnects from an account.
 *
 * @param account The account to disconnect from.
 */
void purple_account_disconnect(PurpleAccount *account);

/**
 * Notifies the user that the account was added to a remote user's
 * buddy list.
 *
 * This will present a dialog informing the user that he was added to the
 * remote user's buddy list.
 *
 * @param account     The account that was added.
 * @param remote_user The name of the user that added this account.
 * @param id          The optional ID of the local account. Rarely used.
 * @param alias       The optional alias of the user.
 * @param message     The optional message sent from the user adding you.
 */
void purple_account_notify_added(PurpleAccount *account, const char *remote_user,
                               const char *id, const char *alias,
                               const char *message);

/**
 * Notifies the user that the account was addded to a remote user's buddy
 * list and asks ther user if they want to add the remote user to their buddy
 * list.
 *
 * This will present a dialog informing the local user that the remote user
 * added them to the remote user's buddy list and will ask if they want to add
 * the remote user to the buddy list.
 *
 * @param account     The account that was added.
 * @param remote_user The name of the user that added this account.
 * @param id          The optional ID of the local account. Rarely used.
 * @param alias       The optional alias of the user.
 * @param message     The optional message sent from the user adding you.
 */
void purple_account_request_add(PurpleAccount *account, const char *remote_user,
                              const char *id, const char *alias,
                              const char *message);

/**
 * Notifies the user that a remote user has wants to add the local user
 * to his or her buddy list and requires authorization to do so.
 *
 * This will present a dialog informing the user of this and ask if the
 * user authorizes or denies the remote user from adding him.
 *
 * @param account      The account that was added
 * @param remote_user  The name of the user that added this account.
 * @param id           The optional ID of the local account. Rarely used.
 * @param alias        The optional alias of the remote user.
 * @param message      The optional message sent by the user wanting to add you.
 * @param on_list      Is the remote user already on the buddy list?
 * @param auth_cb      The callback called when the local user accepts
 * @param deny_cb      The callback called when the local user rejects
 * @param user_data    Data to be passed back to the above callbacks
 *
 * @return A UI-specific handle.
 */
void *purple_account_request_authorization(PurpleAccount *account, const char *remote_user,
					const char *id, const char *alias, const char *message, gboolean on_list,
					PurpleAccountRequestAuthorizationCb auth_cb, PurpleAccountRequestAuthorizationCb deny_cb, void *user_data);

/**
 * Close account requests registered for the given PurpleAccount
 *
 * @param account	   The account for which requests should be closed
 */
void purple_account_request_close_with_account(PurpleAccount *account);

/**
 * Close the account request for the given ui handle
 *
 * @param ui_handle	   The ui specific handle for which requests should be closed
 */
void purple_account_request_close(void *ui_handle);

/**
 * Requests a password from the user for the account. Does not set the
 * account password on success; do that in ok_cb if desired.
 *
 * @param account     The account to request the password for.
 * @param ok_cb       The callback for the OK button.
 * @param cancel_cb   The callback for the cancel button.
 * @param user_data   User data to be passed into callbacks.
 */
void purple_account_request_password(PurpleAccount *account, GCallback ok_cb,
				     GCallback cancel_cb, void *user_data);

/**
 * Requests information from the user to change the account's password.
 *
 * @param account The account to change the password on.
 */
void purple_account_request_change_password(PurpleAccount *account);

/**
 * Requests information from the user to change the account's
 * user information.
 *
 * @param account The account to change the user information on.
 */
void purple_account_request_change_user_info(PurpleAccount *account);

/**
 * Sets the account's username.
 *
 * @param account  The account.
 * @param username The username.
 */
void purple_account_set_username(PurpleAccount *account, const char *username);

/**
 * Sets the account's password.
 *
 * @param account  The account.
 * @param password The password.
 */
void purple_account_set_password(PurpleAccount *account, const char *password);

/**
 * Sets the account's alias.
 *
 * @param account The account.
 * @param alias   The alias.
 */
void purple_account_set_alias(PurpleAccount *account, const char *alias);

/**
 * Sets the account's user information
 *
 * @param account   The account.
 * @param user_info The user information.
 */
void purple_account_set_user_info(PurpleAccount *account, const char *user_info);

/**
 * Sets the account's buddy icon path.
 *
 * @param account The account.
 * @param path	  The buddy icon non-cached path.
 */
void purple_account_set_buddy_icon_path(PurpleAccount *account, const char *path);

/**
 * Sets the account's protocol ID.
 *
 * @param account     The account.
 * @param protocol_id The protocol ID.
 */
void purple_account_set_protocol_id(PurpleAccount *account,
								  const char *protocol_id);

/**
 * Sets the account's connection.
 *
 * @param account The account.
 * @param gc      The connection.
 */
void purple_account_set_connection(PurpleAccount *account, PurpleConnection *gc);

/**
 * Sets whether or not this account should save its password.
 *
 * @param account The account.
 * @param value   @c TRUE if it should remember the password.
 */
void purple_account_set_remember_password(PurpleAccount *account, gboolean value);

/**
 * Sets whether or not this account should check for mail.
 *
 * @param account The account.
 * @param value   @c TRUE if it should check for mail.
 */
void purple_account_set_check_mail(PurpleAccount *account, gboolean value);

/**
 * Sets whether or not this account is enabled for the specified
 * UI.
 *
 * @param account The account.
 * @param ui      The UI.
 * @param value   @c TRUE if it is enabled.
 */
void purple_account_set_enabled(PurpleAccount *account, const char *ui,
			      gboolean value);

/**
 * Sets the account's proxy information.
 *
 * @param account The account.
 * @param info    The proxy information.
 */
void purple_account_set_proxy_info(PurpleAccount *account, PurpleProxyInfo *info);

/**
 * Sets the account's privacy type.
 *
 * @param account      The account.
 * @param privacy_type The privacy type.
 *
 * @since 2.7.0
 */
void purple_account_set_privacy_type(PurpleAccount *account, PurplePrivacyType privacy_type);

/**
 * Sets the account's status types.
 *
 * @param account      The account.
 * @param status_types The list of status types.
 */
void purple_account_set_status_types(PurpleAccount *account, GList *status_types);

/**
 * Variadic version of purple_account_set_status_list(); the variadic list
 * replaces @a attrs, and should be <tt>NULL</tt>-terminated.
 *
 * @copydoc purple_account_set_status_list()
 */
void purple_account_set_status(PurpleAccount *account, const char *status_id,
	gboolean active, ...) G_GNUC_NULL_TERMINATED;


/**
 * Activates or deactivates a status.  All changes to the statuses of
 * an account go through this function or purple_account_set_status().
 *
 * You can only deactivate an exclusive status by activating another exclusive
 * status.  So, if @a status_id is an exclusive status and @a active is @c
 * FALSE, this function does nothing.
 *
 * @param account   The account.
 * @param status_id The ID of the status.
 * @param active    Whether @a status_id is to be activated (<tt>TRUE</tt>) or
 *                  deactivated (<tt>FALSE</tt>).
 * @param attrs     A list of <tt>const char *</tt> attribute names followed by
 *                  <tt>const char *</tt> attribute values for the status.
 *                  (For example, one pair might be <tt>"message"</tt> followed
 *                  by <tt>"hello, talk to me!"</tt>.)
 */
void purple_account_set_status_list(PurpleAccount *account,
	const char *status_id, gboolean active, GList *attrs);

/**
 * Set a server-side (public) alias for this account.  The account
 * must already be connected.
 *
 * Currently, the public alias is not stored locally, although this
 * may change in a later version.
 *
 * @param account    The account
 * @param alias      The new public alias for this account or NULL
 *                   to unset the alias/nickname (or return it to
 *                   a protocol-specific "default", like the username)
 * @param success_cb A callback which will be called if the alias
 *                   is successfully set on the server (or NULL).
 * @param failure_cb A callback which will be called if the alias
 *                   is not successfully set on the server (or NULL).
 *
 * @since 2.7.0
 */
void purple_account_set_public_alias(PurpleAccount *account,
	const char *alias, PurpleSetPublicAliasSuccessCallback success_cb,
	PurpleSetPublicAliasFailureCallback failure_cb);

/**
 * Fetch the server-side (public) alias for this account.  The account
 * must already be connected.
 *
 * @param account    The account
 * @param success_cb A callback which will be called with the alias
 * @param failure_cb A callback which will be called if the prpl is
 *                   unable to retrieve the server-side alias.
 * @since 2.7.0
 */
void purple_account_get_public_alias(PurpleAccount *account,
	PurpleGetPublicAliasSuccessCallback success_cb,
	PurpleGetPublicAliasFailureCallback failure_cb);

/**
 * Return whether silence suppression is used during voice call.
 *
 * @param account The account.
 *
 * @return @c TRUE if suppression is used, or @c FALSE if not.
 */
gboolean purple_account_get_silence_suppression(const PurpleAccount *account);

/**
 * Sets whether silence suppression is used during voice call.
 *
 * @param account The account.
 * @param value   @c TRUE if suppression should be used.
 */
void purple_account_set_silence_suppression(PurpleAccount *account,
											gboolean value);

/**
 * Clears all protocol-specific settings on an account.
 *
 * @param account The account.
 */
void purple_account_clear_settings(PurpleAccount *account);

/**
 * Removes an account-specific setting by name.
 *
 * @param account The account.
 * @param setting The setting to remove.
 *
 * @since 2.6.0
 */
void purple_account_remove_setting(PurpleAccount *account, const char *setting);

/**
 * Sets a protocol-specific integer setting for an account.
 *
 * @param account The account.
 * @param name    The name of the setting.
 * @param value   The setting's value.
 */
void purple_account_set_int(PurpleAccount *account, const char *name, int value);

/**
 * Sets a protocol-specific string setting for an account.
 *
 * @param account The account.
 * @param name    The name of the setting.
 * @param value   The setting's value.
 */
void purple_account_set_string(PurpleAccount *account, const char *name,
							 const char *value);

/**
 * Sets a protocol-specific boolean setting for an account.
 *
 * @param account The account.
 * @param name    The name of the setting.
 * @param value   The setting's value.
 */
void purple_account_set_bool(PurpleAccount *account, const char *name,
						   gboolean value);

/**
 * Sets a UI-specific integer setting for an account.
 *
 * @param account The account.
 * @param ui      The UI name.
 * @param name    The name of the setting.
 * @param value   The setting's value.
 */
void purple_account_set_ui_int(PurpleAccount *account, const char *ui,
							 const char *name, int value);

/**
 * Sets a UI-specific string setting for an account.
 *
 * @param account The account.
 * @param ui      The UI name.
 * @param name    The name of the setting.
 * @param value   The setting's value.
 */
void purple_account_set_ui_string(PurpleAccount *account, const char *ui,
								const char *name, const char *value);

/**
 * Sets a UI-specific boolean setting for an account.
 *
 * @param account The account.
 * @param ui      The UI name.
 * @param name    The name of the setting.
 * @param value   The setting's value.
 */
void purple_account_set_ui_bool(PurpleAccount *account, const char *ui,
							  const char *name, gboolean value);

/**
 * Returns whether or not the account is connected.
 *
 * @param account The account.
 *
 * @return @c TRUE if connected, or @c FALSE otherwise.
 */
gboolean purple_account_is_connected(const PurpleAccount *account);

/**
 * Returns whether or not the account is connecting.
 *
 * @param account The account.
 *
 * @return @c TRUE if connecting, or @c FALSE otherwise.
 */
gboolean purple_account_is_connecting(const PurpleAccount *account);

/**
 * Returns whether or not the account is disconnected.
 *
 * @param account The account.
 *
 * @return @c TRUE if disconnected, or @c FALSE otherwise.
 */
gboolean purple_account_is_disconnected(const PurpleAccount *account);

/**
 * Returns the account's username.
 *
 * @param account The account.
 *
 * @return The username.
 */
const char *purple_account_get_username(const PurpleAccount *account);

/**
 * Returns the account's password.
 *
 * @param account The account.
 *
 * @return The password.
 */
const char *purple_account_get_password(const PurpleAccount *account);

/**
 * Returns the account's alias.
 *
 * @param account The account.
 *
 * @return The alias.
 */
const char *purple_account_get_alias(const PurpleAccount *account);

/**
 * Returns the account's user information.
 *
 * @param account The account.
 *
 * @return The user information.
 */
const char *purple_account_get_user_info(const PurpleAccount *account);

/**
 * Gets the account's buddy icon path.
 *
 * @param account The account.
 *
 * @return The buddy icon's non-cached path.
 */
const char *purple_account_get_buddy_icon_path(const PurpleAccount *account);

/**
 * Returns the account's protocol ID.
 *
 * @param account The account.
 *
 * @return The protocol ID.
 */
const char *purple_account_get_protocol_id(const PurpleAccount *account);

/**
 * Returns the account's protocol name.
 *
 * @param account The account.
 *
 * @return The protocol name.
 */
const char *purple_account_get_protocol_name(const PurpleAccount *account);

/**
 * Returns the account's connection.
 *
 * @param account The account.
 *
 * @return The connection.
 */
PurpleConnection *purple_account_get_connection(const PurpleAccount *account);

/**
 * Returns a name for this account appropriate for display to the user. In
 * order of preference: the account's alias; the contact or buddy alias (if
 * the account exists on its own buddy list); the connection's display name;
 * the account's username.
 *
 * @param account The account.
 *
 * @return The name to display.
 *
 * @since 2.7.0
 */
const gchar *purple_account_get_name_for_display(const PurpleAccount *account);

/**
 * Returns whether or not this account should save its password.
 *
 * @param account The account.
 *
 * @return @c TRUE if it should remember the password.
 */
gboolean purple_account_get_remember_password(const PurpleAccount *account);

/**
 * Returns whether or not this account should check for mail.
 *
 * @param account The account.
 *
 * @return @c TRUE if it should check for mail.
 */
gboolean purple_account_get_check_mail(const PurpleAccount *account);

/**
 * Returns whether or not this account is enabled for the
 * specified UI.
 *
 * @param account The account.
 * @param ui      The UI.
 *
 * @return @c TRUE if it enabled on this UI.
 */
gboolean purple_account_get_enabled(const PurpleAccount *account,
				  const char *ui);

/**
 * Returns the account's proxy information.
 *
 * @param account The account.
 *
 * @return The proxy information.
 */
PurpleProxyInfo *purple_account_get_proxy_info(const PurpleAccount *account);

/**
 * Returns the account's privacy type.
 *
 * @param account   The account.
 *
 * @return The privacy type.
 *
 * @since 2.7.0
 */
PurplePrivacyType purple_account_get_privacy_type(const PurpleAccount *account);

/**
 * Returns the active status for this account.  This looks through
 * the PurplePresence associated with this account and returns the
 * PurpleStatus that has its active flag set to "TRUE."  There can be
 * only one active PurpleStatus in a PurplePresence.
 *
 * @param account   The account.
 *
 * @return The active status.
 */
PurpleStatus *purple_account_get_active_status(const PurpleAccount *account);

/**
 * Returns the account status with the specified ID.
 *
 * Note that this works differently than purple_buddy_get_status() in that
 * it will only return NULL if the status was not registered.
 *
 * @param account   The account.
 * @param status_id The status ID.
 *
 * @return The status, or NULL if it was never registered.
 */
PurpleStatus *purple_account_get_status(const PurpleAccount *account,
									const char *status_id);

/**
 * Returns the account status type with the specified ID.
 *
 * @param account The account.
 * @param id      The ID of the status type to find.
 *
 * @return The status type if found, or NULL.
 */
PurpleStatusType *purple_account_get_status_type(const PurpleAccount *account,
											 const char *id);

/**
 * Returns the account status type with the specified primitive.
 * Note: It is possible for an account to have more than one
 * PurpleStatusType with the same primitive.  In this case, the
 * first PurpleStatusType is returned.
 *
 * @param account   The account.
 * @param primitive The type of the status type to find.
 *
 * @return The status if found, or NULL.
 */
PurpleStatusType *purple_account_get_status_type_with_primitive(
							const PurpleAccount *account,
							PurpleStatusPrimitive primitive);

/**
 * Returns the account's presence.
 *
 * @param account The account.
 *
 * @return The account's presence.
 */
PurplePresence *purple_account_get_presence(const PurpleAccount *account);

/**
 * Returns whether or not an account status is active.
 *
 * @param account   The account.
 * @param status_id The status ID.
 *
 * @return TRUE if active, or FALSE if not.
 */
gboolean purple_account_is_status_active(const PurpleAccount *account,
									   const char *status_id);

/**
 * Returns the account's status types.
 *
 * @param account The account.
 *
 * @constreturn The account's status types.
 */
GList *purple_account_get_status_types(const PurpleAccount *account);

/**
 * Returns a protocol-specific integer setting for an account.
 *
 * @param account       The account.
 * @param name          The name of the setting.
 * @param default_value The default value.
 *
 * @return The value.
 */
int purple_account_get_int(const PurpleAccount *account, const char *name,
						 int default_value);

/**
 * Returns a protocol-specific string setting for an account.
 *
 * @param account       The account.
 * @param name          The name of the setting.
 * @param default_value The default value.
 *
 * @return The value.
 */
const char *purple_account_get_string(const PurpleAccount *account,
									const char *name,
									const char *default_value);

/**
 * Returns a protocol-specific boolean setting for an account.
 *
 * @param account       The account.
 * @param name          The name of the setting.
 * @param default_value The default value.
 *
 * @return The value.
 */
gboolean purple_account_get_bool(const PurpleAccount *account, const char *name,
							   gboolean default_value);

/**
 * Returns a UI-specific integer setting for an account.
 *
 * @param account       The account.
 * @param ui            The UI name.
 * @param name          The name of the setting.
 * @param default_value The default value.
 *
 * @return The value.
 */
int purple_account_get_ui_int(const PurpleAccount *account, const char *ui,
							const char *name, int default_value);

/**
 * Returns a UI-specific string setting for an account.
 *
 * @param account       The account.
 * @param ui            The UI name.
 * @param name          The name of the setting.
 * @param default_value The default value.
 *
 * @return The value.
 */
const char *purple_account_get_ui_string(const PurpleAccount *account,
									   const char *ui, const char *name,
									   const char *default_value);

/**
 * Returns a UI-specific boolean setting for an account.
 *
 * @param account       The account.
 * @param ui            The UI name.
 * @param name          The name of the setting.
 * @param default_value The default value.
 *
 * @return The value.
 */
gboolean purple_account_get_ui_bool(const PurpleAccount *account, const char *ui,
								  const char *name, gboolean default_value);


/**
 * Returns the system log for an account.
 *
 * @param account The account.
 * @param create  Should it be created if it doesn't exist?
 *
 * @return The log.
 *
 * @note Callers should almost always pass @c FALSE for @a create.
 *       Passing @c TRUE could result in an existing log being reopened,
 *       if the log has already been closed, which not all loggers deal
 *       with appropriately.
 */
PurpleLog *purple_account_get_log(PurpleAccount *account, gboolean create);

/**
 * Frees the system log of an account
 *
 * @param account The account.
 */
void purple_account_destroy_log(PurpleAccount *account);

/**
 * Adds a buddy to the server-side buddy list for the specified account.
 *
 * @param account The account.
 * @param buddy The buddy to add.
 *
 * @deprecated Use purple_account_add_buddy_with_invite and \c NULL message.
 */
void purple_account_add_buddy(PurpleAccount *account, PurpleBuddy *buddy);
/**
 * Adds a buddy to the server-side buddy list for the specified account.
 *
 * @param account The account.
 * @param buddy The buddy to add.
 * @param message The invite message.  This may be ignored by a prpl.
 *
 * @since 2.8.0
 */
void purple_account_add_buddy_with_invite(PurpleAccount *account, PurpleBuddy *buddy, const char *message);

/**
 * Adds a list of buddies to the server-side buddy list.
 *
 * @param account The account.
 * @param buddies The list of PurpleBlistNodes representing the buddies to add.
 *
 * @deprecated Use purple_account_add_buddies_with_invite and \c NULL message.
 */
void purple_account_add_buddies(PurpleAccount *account, GList *buddies);
/**
 * Adds a list of buddies to the server-side buddy list.
 *
 * @param account The account.
 * @param buddies The list of PurpleBlistNodes representing the buddies to add.
 * @param message The invite message.  This may be ignored by a prpl.
 *
 * @since 2.8.0
 */
void purple_account_add_buddies_with_invite(PurpleAccount *account, GList *buddies, const char *message);

/**
 * Removes a buddy from the server-side buddy list.
 *
 * @param account The account.
 * @param buddy The buddy to remove.
 * @param group The group to remove the buddy from.
 */
void purple_account_remove_buddy(PurpleAccount *account, PurpleBuddy *buddy,
								PurpleGroup *group);

/**
 * Removes a list of buddies from the server-side buddy list.
 *
 * @note The lists buddies and groups are parallel lists.  Be sure that node n of
 *       groups matches node n of buddies.
 *
 * @param account The account.
 * @param buddies The list of buddies to remove.
 * @param groups The list of groups to remove buddies from.  Each node of this
 *               list should match the corresponding node of buddies.
 */
void purple_account_remove_buddies(PurpleAccount *account, GList *buddies,
									GList *groups);

/**
 * Removes a group from the server-side buddy list.
 *
 * @param account The account.
 * @param group The group to remove.
 */
void purple_account_remove_group(PurpleAccount *account, PurpleGroup *group);

/**
 * Changes the password on the specified account.
 *
 * @param account The account.
 * @param orig_pw The old password.
 * @param new_pw The new password.
 */
void purple_account_change_password(PurpleAccount *account, const char *orig_pw,
									const char *new_pw);

/**
 * Whether the account supports sending offline messages to buddy.
 *
 * @param account The account
 * @param buddy   The buddy
 */
gboolean purple_account_supports_offline_message(PurpleAccount *account, PurpleBuddy *buddy);

/**
 * Get the error that caused the account to be disconnected, or @c NULL if the
 * account is happily connected or disconnected without an error.
 *
 * @param account The account whose error should be retrieved.
 * @constreturn   The type of error and a human-readable description of the
 *                current error, or @c NULL if there is no current error.  This
 *                pointer is guaranteed to remain valid until the @ref
 *                account-error-changed signal is emitted for @a account.
 */
const PurpleConnectionErrorInfo *purple_account_get_current_error(PurpleAccount *account);

/**
 * Clear an account's current error state, resetting it to @c NULL.
 *
 * @param account The account whose error state should be cleared.
 */
void purple_account_clear_current_error(PurpleAccount *account);

/*@}*/

/**************************************************************************/
/** @name Accounts API                                                    */
/**************************************************************************/
/*@{*/

/**
 * Adds an account to the list of accounts.
 *
 * @param account The account.
 */
void purple_accounts_add(PurpleAccount *account);

/**
 * Removes an account from the list of accounts.
 *
 * @param account The account.
 */
void purple_accounts_remove(PurpleAccount *account);

/**
 * Deletes an account.
 *
 * This will remove any buddies from the buddy list that belong to this
 * account, buddy pounces that belong to this account, and will also
 * destroy @a account.
 *
 * @param account The account.
 */
void purple_accounts_delete(PurpleAccount *account);

/**
 * Reorders an account.
 *
 * @param account   The account to reorder.
 * @param new_index The new index for the account.
 */
void purple_accounts_reorder(PurpleAccount *account, gint new_index);

/**
 * Returns a list of all accounts.
 *
 * @constreturn A list of all accounts.
 */
GList *purple_accounts_get_all(void);

/**
 * Returns a list of all enabled accounts
 *
 * @return A list of all enabled accounts. The list is owned
 *         by the caller, and must be g_list_free()d to avoid
 *         leaking the nodes.
 */
GList *purple_accounts_get_all_active(void);

/**
 * Finds an account with the specified name and protocol id.
 *
 * @param name     The account username.
 * @param protocol The account protocol ID.
 *
 * @return The account, if found, or @c FALSE otherwise.
 */
PurpleAccount *purple_accounts_find(const char *name, const char *protocol);

/**
 * This is called by the core after all subsystems and what
 * not have been initialized.  It sets all enabled accounts
 * to their startup status by signing them on, setting them
 * away, etc.
 *
 * You probably shouldn't call this unless you really know
 * what you're doing.
 */
void purple_accounts_restore_current_statuses(void);

/*@}*/


/**************************************************************************/
/** @name UI Registration Functions                                       */
/**************************************************************************/
/*@{*/
/**
 * Sets the UI operations structure to be used for accounts.
 *
 * @param ops The UI operations structure.
 */
void purple_accounts_set_ui_ops(PurpleAccountUiOps *ops);

/**
 * Returns the UI operations structure used for accounts.
 *
 * @return The UI operations structure in use.
 */
PurpleAccountUiOps *purple_accounts_get_ui_ops(void);

/*@}*/


/**************************************************************************/
/** @name Accounts Subsystem                                              */
/**************************************************************************/
/*@{*/

/**
 * Returns the accounts subsystem handle.
 *
 * @return The accounts subsystem handle.
 */
void *purple_accounts_get_handle(void);

/**
 * Initializes the accounts subsystem.
 */
void purple_accounts_init(void);

/**
 * Uninitializes the accounts subsystem.
 */
void purple_accounts_uninit(void);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_ACCOUNT_H_ */
