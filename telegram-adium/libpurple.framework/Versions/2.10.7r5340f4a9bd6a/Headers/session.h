/**
 * @file session.h MSN session functions
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
#ifndef MSN_SESSION_H
#define MSN_SESSION_H

typedef struct _MsnSession MsnSession;

/**
 * Types of errors.
 */
typedef enum
{
	MSN_ERROR_SERVCONN,
	MSN_ERROR_UNSUPPORTED_PROTOCOL,
	MSN_ERROR_HTTP_MALFORMED,
	MSN_ERROR_AUTH,
	MSN_ERROR_BAD_BLIST,
	MSN_ERROR_SIGN_OTHER,
	MSN_ERROR_SERV_DOWN,
	MSN_ERROR_SERV_UNAVAILABLE
} MsnErrorType;

/**
 * Login steps.
 */
typedef enum
{
	MSN_LOGIN_STEP_START,
	MSN_LOGIN_STEP_HANDSHAKE,
	MSN_LOGIN_STEP_TRANSFER,
	MSN_LOGIN_STEP_HANDSHAKE2,
	MSN_LOGIN_STEP_AUTH_START,
	MSN_LOGIN_STEP_GET_COOKIE,
	MSN_LOGIN_STEP_AUTH_END,
	MSN_LOGIN_STEP_SYN,
	MSN_LOGIN_STEP_END
} MsnLoginStep;

#define MSN_LOGIN_STEPS MSN_LOGIN_STEP_END

#define MSN_LOGIN_FQY_TIMEOUT 30

#define MSN_LOGIN_FQY_TIMEOUT 30

#include "nexus.h"
#include "notification.h"
#include "oim.h"
#include "switchboard.h"
#include "user.h"
#include "userlist.h"

struct _MsnSession
{
	PurpleAccount *account;
	MsnUser *user;

	guint protocol_ver;

	MsnLoginStep login_step; /**< The current step in the login process. */

	gboolean connected:1;
	gboolean logged_in:1; /**< A temporal flag to ignore local buddy list adds. */
	gboolean destroying:1; /**< A flag that states if the session is being destroyed. */
	gboolean http_method:1;
	gboolean enable_mpop:1; /**< Use Multiple Points of Presence? */
	int      adl_fqy; /**< A count of ADL/FQY so status is only changed once. */
	guint    login_timeout; /**< Timeout to force status change if ADL/FQY fail. */

	MsnNotification *notification;
	MsnNexus        *nexus;
	MsnOim          *oim;
	MsnUserList     *userlist;
	char            *abch_cachekey;

	int servconns_count; /**< The count of server connections. */
	GList *switches; /**< The list of all the switchboards. */
	GList *slplinks; /**< The list of all the slplinks. */

	/*psm info*/
	char *psm;

#if 0
	char *blocked_text;
#endif

	struct
	{
		char *sid;
		char *mspauth;
		unsigned long sl;
		char *client_ip;
		int client_port;
		char *mail_url;
		gulong mail_timestamp;
		gboolean email_enabled;
	} passport_info;

	GHashTable *soap_table;
	guint soap_cleanup_handle;
	char *guid;

	GSList *url_datas; /**< PurpleUtilFetchUrlData to be cancelled on exit */
};

/**
 * Creates an MSN session.
 *
 * @param account The account.
 *
 * @return The new MSN session.
 */
MsnSession *msn_session_new(PurpleAccount *account);

/**
 * Destroys an MSN session.
 *
 * @param session The MSN session to destroy.
 */
void msn_session_destroy(MsnSession *session);

/**
 * Connects to and initiates an MSN session.
 *
 * @param session     The MSN session.
 * @param host        The dispatch server host.
 * @param port        The dispatch server port.
 * @param http_method Whether to use or not http_method.
 *
 * @return @c TRUE on success, @c FALSE on failure.
 */
gboolean msn_session_connect(MsnSession *session,
							 const char *host, int port,
							 gboolean http_method);

/**
 * Disconnects from an MSN session.
 *
 * @param session The MSN session.
 */
void msn_session_disconnect(MsnSession *session);

 /**
 * Finds a switchboard with the given username.
 *
 * @param session The MSN session.
 * @param username The username to search for.
 *
 * @return The switchboard, if found.
 */
MsnSwitchBoard *msn_session_find_swboard(MsnSession *session,
										 const char *username);

 /**
 * Finds a switchboard with the given conversation.
 *
 * @param session The MSN session.
 * @param conv    The conversation to search for.
 *
 * @return The switchboard, if found.
 */
MsnSwitchBoard *msn_session_find_swboard_with_conv(MsnSession *session,
												   PurpleConversation *conv);
/**
 * Finds a switchboard with the given chat ID.
 *
 * @param session The MSN session.
 * @param chat_id The chat ID to search for.
 *
 * @return The switchboard, if found.
 */
MsnSwitchBoard *msn_session_find_swboard_with_id(const MsnSession *session,
												 int chat_id);

/**
 * Returns a switchboard to communicate with certain username.
 *
 * @param session The MSN session.
 * @param username The username to search for.
 * @param flag The flag of the switchboard
 *
 * @return The switchboard.
 */
MsnSwitchBoard *msn_session_get_swboard(MsnSession *session,
										const char *username, MsnSBFlag flag);

/**
 * Sets an error for the MSN session.
 *
 * @param session The MSN session.
 * @param error The error.
 * @param info Extra information.
 */
void msn_session_set_error(MsnSession *session, MsnErrorType error,
						   const char *info);

/**
 * Starts a timeout to initiate finishing login. Sometimes the server ignores
 * our FQY requests, so this forces ourselves online eventually.
 *
 * @param session The MSN session.
 */
void
msn_session_activate_login_timeout(MsnSession *session);

/**
 * Sets the current step in the login process.
 *
 * @param session The MSN session.
 * @param step The current step.
 */
void msn_session_set_login_step(MsnSession *session, MsnLoginStep step);

/**
 * Finish the login proccess.
 *
 * @param session The MSN session.
 */
void msn_session_finish_login(MsnSession *session);

/*post message to User*/
void msn_session_report_user(MsnSession *session,const char *passport,
							const char *msg,PurpleMessageFlags flags);

#endif /* MSN_SESSION_H */
