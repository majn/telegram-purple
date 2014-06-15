/**
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

#include <glib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// test

// Libpurple Plugin Includes
#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "account.h"
#include "accountopt.h"
#include "blist.h"
#include "cmds.h"
#include "conversation.h"
#include "connection.h"
#include "debug.h"
#include "privacy.h"
#include "prpl.h"
#include "roomlist.h"
#include "status.h"
#include "util.h"
#include "prpl.h"
#include "prefs.h"
#include "util.h"

// Telegram Includes
#include <tg-cli.h>
#include "msglog.h"

// telegram-purple includes
#include "loop.h"
#include "telegram-purple.h"

static PurplePlugin *_telegram_protocol = NULL;

/**
 * Redirect the msglog of the telegram-cli application to the libpurple
 *	logger
 */
void tg_cli_log_cb(const char* format, va_list ap)
{
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, ap);
	purple_debug_info(PLUGIN_ID, "%s", buffer);
}

/**
 * Returns the base icon name for the given buddy and account.
 * If buddy is NULL and the account is non-NULL, it will return the
 * name to use for the account's icon. If both are NULL, it will
 * return the name to use for the protocol's icon.
 *
 * This must be implemented.
 */
static const char *tgprpl_list_icon(PurpleAccount * acct, PurpleBuddy * buddy)
{
    purple_debug_info(PLUGIN_ID, "tgrpl_list_icon()\n");
	return "telegram";
}

/**
 * Gets a short string representing this buddy's status.  This will
 * be shown on the buddy list.
 */
static char *tgprpl_status_text(PurpleBuddy * buddy)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_status_text()\n");
	return "status text";
}

/**
 * Allows the prpl to add text to a buddy's tooltip.
 */
static void tgprpl_tooltip_text(PurpleBuddy * buddy, PurpleNotifyUserInfo * info, gboolean full)
{
	purple_debug_info(PLUGIN_ID, "tgprpl_tooltip_text()\n");
}

/**
 * This must be implemented.
 */
static void tgprpl_login(PurpleAccount * acct)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_login()\n");
    PurpleConnection *gc = purple_account_get_connection(acct);

    const char *username = purple_account_get_username(acct);
    const char *code     = purple_account_get_string(acct, "verification_key", NULL);
	const char *hash 	 = purple_account_get_string(acct, "verification_hash", NULL);
    const char *hostname = purple_account_get_string(acct, "server", TELEGRAM_TEST_SERVER);
    // const char *verificationType = purple_account_get_string(acct, "verification_type", TELEGRAM_AUTH_MODE_SMS);
    // int port = purple_account_get_int(acct, "port", TELEGRAM_DEFAULT_PORT);

    purple_debug_info(PLUGIN_ID, "username: %s\n", username);
    purple_debug_info(PLUGIN_ID, "code: %s\n", code);
    purple_debug_info(PLUGIN_ID, "verification_hash: %s\n", hash);
    purple_debug_info(PLUGIN_ID, "hostname: %s\n", hostname);

	// ensure config-file exists an
    purple_debug_info(PLUGIN_ID, "running_for_first_time()\n");
    running_for_first_time();

	// load all settings: the known network topology, log and configuration file paths
    purple_debug_info(PLUGIN_ID, "parse_config()\n");
    parse_config ();
    purple_debug_info(PLUGIN_ID, "set_default_username()\n");
    set_default_username (username);

	// Connect to the network
    purple_debug_info(PLUGIN_ID, "Connecting to the Telegram network...\n");
    network_connect();

	// Login
	if (!network_phone_is_registered()) {
		purple_debug_info(PLUGIN_ID, "Phone is not registered, registering...\n");
	} 

	if (!network_client_is_registered()) {
		purple_debug_info(PLUGIN_ID, "Client is not registered\n");
	
	    if (code && hash) {
		   purple_debug_info(PLUGIN_ID, "SMS code provided, trying to verify \n");
		   purple_debug_info(PLUGIN_ID, "pointer - code:%p hash:%p\n", code, hash);
		   purple_debug_info(PLUGIN_ID, "string - code%s hash:%s\n", code, hash);
	       int verified = network_verify_registration(code, hash);
		   if (verified) {
		      store_config();
		   } else {
				purple_connection_set_state(gc, PURPLE_DISCONNECTED);
				purple_notify_message(_telegram_protocol, PURPLE_NOTIFY_MSG_INFO, "Verification Failed", 
					"Please make sure you entered the correct verification code.", NULL, NULL, NULL);
				return;
		   }
	    } else {
		   // TODO: we should find a way to request the key
		   //			only once.
		   purple_debug_info(PLUGIN_ID, "Device not registered, requesting new authentication code.\n");
		   char *new_hash = network_request_registration();
		   purple_debug_info(PLUGIN_ID, "Saving verification_hash: '%s'", new_hash);
		   purple_account_set_string(acct, "verification_hash", new_hash);
		}
	} 

	// Our protocol data, that will be delivered to us
	// through purple connection
    telegram_conn *conn = g_new0(telegram_conn, 1);
    conn->gc 	  = gc;
    conn->account = acct;

    purple_connection_set_protocol_data(gc, conn);
    gc->proto_data = conn;
	
	// TODO: probably needs to be in callback after connection
	purple_connection_set_state(gc, PURPLE_CONNECTED);
    purple_debug_info(PLUGIN_ID, "Logged in...\n");
}

/**
 * This must be implemented.
 */
static void tgprpl_close(PurpleConnection * gc)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_close()\n");
}

/**
 * This PRPL function should return a positive value on success.
 * If the message is too big to be sent, return -E2BIG.  If
 * the account is not connected, return -ENOTCONN.  If the
 * PRPL is unable to send the message for another reason, return
 * some other negative value.  You can use one of the valid
 * errno values, or just big something.  If the message should
 * not be echoed to the conversation window, return 0.
 */
static int tgprpl_send_im(PurpleConnection * gc, const char *who, const char *message, PurpleMessageFlags flags)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_send_im()\n");
	
    return -1;
}

/**
 * Send a message to a chat.
 * This PRPL function should return a positive value on success.
 * If the message is too big to be sent, return -E2BIG.  If
 * the account is not connected, return -ENOTCONN.  If the
 * PRPL is unable to send the message for another reason, return
 * some other negative value.  You can use one of the valid
 * errno values, or just big something.
 *
 * @param id      The id of the chat to send the message to.
 * @param message The message to send to the chat.
 * @param flags   A bitwise OR of #PurpleMessageFlags representing
 *                message flags.
 * @return 	  A positive number or 0 in case of success,
 *                a negative error number in case of failure.
 */
static int tgprpl_send_chat(PurpleConnection * gc, int id, const char *message, PurpleMessageFlags flags)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_send_chat()\n");
    return -1;
}

/**
 * Add a buddy to a group on the server.
 *
 * This PRPL function may be called in situations in which the buddy is
 * already in the specified group. If the protocol supports
 * authorization and the user is not already authorized to see the
 * status of \a buddy, \a add_buddy should request authorization.
 *
 * @deprecated Since 2.8.0, add_buddy_with_invite is preferred.
 * @see add_buddy_with_invite
 */
static void tgprpl_add_buddy(PurpleConnection * gc, PurpleBuddy * buddy, PurpleGroup * group)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_add_buddy()\n");
}

static void tgprpl_add_buddies(PurpleConnection * gc, GList * buddies, GList * groups)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_add_buddies()\n");
}

static void tgprpl_remove_buddy(PurpleConnection * gc, PurpleBuddy * buddy, PurpleGroup * group)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_remove_buddy()\n");
}

static void tgprpl_remove_buddies(PurpleConnection * gc, GList * buddies, GList * groups)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_remove_buddies()\n");

}

static void tgprpl_convo_closed(PurpleConnection * gc, const char *who)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_convo_closed()\n");

}

static void tgprpl_add_deny(PurpleConnection * gc, const char *name)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_add_deny()\n");

}

static void tgprpl_rem_deny(PurpleConnection * gc, const char *name)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_rem_deny()\n");

}

/**
 * @return If this protocol requires the PURPLE_TYPING message to
 *         be sent repeatedly to signify that the user is still
 *         typing, then the PRPL should return the number of
 *         seconds to wait before sending a subsequent notification.
 *         Otherwise the PRPL should return 0.
 */
static unsigned int tgprpl_send_typing(PurpleConnection * gc, const char *who, PurpleTypingState typing)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_send_typing()\n");
    return 0;
}

/**
 * Set the buddy icon for the given connection to @a img.  The prpl
 * does NOT own a reference to @a img; if it needs one, it must
 * #purple_imgstore_ref(@a img) itself.
 */
static void tgprpl_set_buddy_icon(PurpleConnection * gc, PurpleStoredImage * img)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_set_buddy_icon()\n");

}

/**
 * File transfer callback
 */
static gboolean tgprpl_can_receive_file(PurpleConnection * gc, const char *who)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_can_receive_file()\n");
    return FALSE;
}

/**
 *  Checks whether offline messages to @a buddy are supported.

 *  @return @c TRUE if @a buddy can be sent messages while they are
 *          offline, or @c FALSE if not.
 */
static gboolean tgprpl_offline_message(const PurpleBuddy * buddy)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_offline_message()\n");
    return FALSE;
}

/**
 * Returns a list of #PurpleStatusType which exist for this account;
 * this must be implemented, and must add at least the offline and
 * online states.
 */
static GList *tgprpl_status_types(PurpleAccount * acct)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_status_types()\n");
    GList *types = NULL;
    PurpleStatusType *type;
    type = purple_status_type_new_with_attrs(PURPLE_STATUS_AVAILABLE, "available", NULL, TRUE, TRUE, FALSE, "message", "Message", purple_value_new(PURPLE_TYPE_STRING), NULL);
    types = g_list_prepend(types, type);

    type = purple_status_type_new_with_attrs(PURPLE_STATUS_AWAY, "unavailable", NULL, TRUE, TRUE, FALSE, "message", "Message", purple_value_new(PURPLE_TYPE_STRING), NULL);
    types = g_list_prepend(types, type);

    type = purple_status_type_new(PURPLE_STATUS_OFFLINE, NULL, NULL, TRUE);
    types = g_list_append(types, type);

    return g_list_reverse(types);
}

static void tgprpl_set_status(PurpleAccount * acct, PurpleStatus * status)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_set_status()\n");
}

/**
 * Should arrange for purple_notify_userinfo() to be called with
 * @a who's user info.
 */
static void tgprpl_get_info(PurpleConnection * gc, const char *username)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_get_info()\n");

}

/**
 * change a buddy's group on a server list/roster
 */
static void tgprpl_group_buddy(PurpleConnection * gc, const char *who, const char *old_group, const char *new_group)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_group_buddy()\n");

}

/**
 * rename a group on a server list/roster
 */
static void tgprpl_rename_group(PurpleConnection * gc, const char *old_name, PurpleGroup * group, GList * moved_buddies)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_rename_group()\n");

}

/**
 * Returns a list of #proto_chat_entry structs, which represent
 * information required by the PRPL to join a chat. libpurple will
 * call join_chat along with the information filled by the user.
 *
 * @return A list of #proto_chat_entry structs
 */
static GList *tgprpl_chat_join_info(PurpleConnection * gc)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_chat_join_info()\n");
    return NULL;
}

/**
 * Returns a hashtable which maps #proto_chat_entry struct identifiers
 * to default options as strings based on chat_name. The resulting
 * hashtable should be created with g_hash_table_new_full(g_str_hash,
 * g_str_equal, NULL, g_free);. Use #get_chat_name if you instead need
 * to extract a chat name from a hashtable.
 *
 * @param chat_name The chat name to be turned into components
 * @return Hashtable containing the information extracted from chat_name
 */
static GHashTable *tgprpl_chat_info_defaults(PurpleConnection * gc, const char *chat_name)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_chat_info_defaults()\n");
    return NULL;
}

/**
 * Called when the user requests joining a chat. Should arrange for
 * #serv_got_joined_chat to be called.
 *
 * @param components A hashtable containing information required to
 *                   join the chat as described by the entries returned
 *                   by #chat_info. It may also be called when accepting
 *                   an invitation, in which case this matches the
 *                   data parameter passed to #serv_got_chat_invite.
 */
static void tgprpl_chat_join(PurpleConnection * gc, GHashTable * data)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_chat_join()\n");
}

/**
 * Invite a user to join a chat.
 *
 * @param id      The id of the chat to invite the user to.
 * @param message A message displayed to the user when the invitation
 *                is received.
 * @param who     The name of the user to send the invation to.
 */
static void tgprpl_chat_invite(PurpleConnection * gc, int id, const char *message, const char *name)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_chat_invite()\n");
}

/**
 * Returns a chat name based on the information in components. Use
 * #chat_info_defaults if you instead need to generate a hashtable
 * from a chat name.
 *
 * @param components A hashtable containing information about the chat.
 */
static char *tgprpl_get_chat_name(GHashTable * data)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_get_chat_name()\n");
    return "chat name";
}

/**
 * File transfer callback.
 */
static PurpleXfer *tgprpl_new_xfer(PurpleConnection * gc, const char *who)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_new_xfer()\n");
    return (PurpleXfer *)NULL;
}

/**
 * File transfer callback.
 */
static void tgprpl_send_file(PurpleConnection * gc, const char *who, const char *file)
{
    purple_debug_info(PLUGIN_ID, "tgprpl_send_file()\n");

}

// SEE prpl.h
static PurplePluginProtocolInfo prpl_info = {
	OPT_PROTO_NO_PASSWORD,                    /* options */
	NULL,			                              /* user_splits, initialized in tgprpl_init() */
	NULL,			                              /* protocol_options, initialized in tgprpl_init() */

	{                                         /* icon_spec, a PurpleBuddyIconSpec */
		"png",		                           /* format */
		1,			                              /* min_width */
		1,			                              /* min_height */
		512,		                              /* max_width */
		512,		                              /* max_height */
		64000,		                           /* max_filesize */
		PURPLE_ICON_SCALE_SEND,	               /* scale_rules */
	},
	tgprpl_list_icon,
	NULL,
	tgprpl_status_text,
	tgprpl_tooltip_text,
	tgprpl_status_types,
	NULL,                                     /* blist_node_menu */
    tgprpl_chat_join_info,
    tgprpl_chat_info_defaults,	            /* chat_info_defaults */
	tgprpl_login,                             /* login */
	tgprpl_close,		                        /* close */
	tgprpl_send_im,		                     /* send_im */
	NULL,                                     /* set_info */
	tgprpl_send_typing,	                     /* send_typing */
	tgprpl_get_info,	                        /* get_info */
	tgprpl_set_status,	                     /* set_status */
	NULL,                                     /* set_idle */
	NULL,                                     /* change_passwd */
	tgprpl_add_buddy,	                        /* add_buddy */
	tgprpl_add_buddies,	                     /* add_buddies */
	tgprpl_remove_buddy,	                     /* remove_buddy */
	tgprpl_remove_buddies,	                  /* remove_buddies */
	NULL,                                     /* add_permit */
	tgprpl_add_deny,	                        /* add_deny */
	NULL,                                     /* rem_permit */
	tgprpl_rem_deny,	                        /* rem_deny */
	NULL,                                     /* set_permit_deny */
	tgprpl_chat_join,	/* join_chat */
	NULL,                                     /* reject_chat */
	tgprpl_get_chat_name,	                  /* get_chat_name */
	tgprpl_chat_invite,	                     /* chat_invite */
	NULL,                                     /* chat_leave */
	NULL,                                     /* chat_whisper */
	tgprpl_send_chat,	                        /* chat_send */
	NULL,                                     /* keepalive */
	NULL,                                     /* register_user */
	NULL,                                     /* get_cb_info */
	NULL,                                     /* get_cb_away */
	NULL,                                     /* alias_buddy */
	tgprpl_group_buddy,	                     /* group_buddy */
	tgprpl_rename_group,	                     /* rename_group */
	NULL,                                     /* buddy_free */
	tgprpl_convo_closed,	                     /* convo_closed */
	purple_normalize_nocase,	               /* normalize */
	tgprpl_set_buddy_icon,	                  /* set_buddy_icon */
	NULL,                                     /* remove_group */
	NULL,                                     /* get_cb_real_name */
	NULL,                                     /* set_chat_topic */
	NULL,                                     /* find_blist_chat */
	NULL,                                     /* roomlist_get_list */
	NULL,                                     /* roomlist_cancel */
	NULL,                                     /* roomlist_expand_category */
	tgprpl_can_receive_file,	               /* can_receive_file */
	tgprpl_send_file,	                        /* send_file */
	tgprpl_new_xfer,	                        /* new_xfer */
	tgprpl_offline_message,	                  /* offline_message */
	NULL,                                     /* whiteboard_prpl_ops */
	NULL,                                     /* send_raw */
	NULL,                                     /* roomlist_room_serialize */
	NULL,                                     /* unregister_user */
	NULL,                                     /* send_attention */
	NULL,                                     /* get_attention_types */
	sizeof(PurplePluginProtocolInfo),	      /* struct_size */
	NULL,                                     /* get_account_text_table */
	NULL,                                     /* initiate_media */
	NULL,                                     /* get_media_caps */
	NULL,                                     /* get_moods */
	NULL,                                     /* set_public_alias */
	NULL,                                     /* get_public_alias */
	NULL,                                     /* add_buddy_with_invite */
	NULL			                              /* add_buddies_with_invite */
};

static void tgprpl_init(PurplePlugin *plugin)
{
    PurpleAccountOption *option;
	PurpleAccountUserSplit *split;
	GList *verification_values = NULL;

	// intialise logging
    set_log_cb(&tg_cli_log_cb);

	// Required Verification-Key
//	split = purple_account_user_split_new("Verification key", NULL, '@');
//	purple_account_user_split_set_reverse(split, FALSE);
//	prpl_info.user_splits = g_list_append(prpl_info.user_splits, split);

	// Extra Options
 #define ADD_VALUE(list, desc, v) { \
     PurpleKeyValuePair *kvp = g_new0(PurpleKeyValuePair, 1); \
     kvp->key = g_strdup((desc)); \
     kvp->value = g_strdup((v)); \
     list = g_list_prepend(list, kvp); \
 }
	ADD_VALUE(verification_values, "Phone", TELEGRAM_AUTH_MODE_PHONE);
	ADD_VALUE(verification_values, "SMS", TELEGRAM_AUTH_MODE_SMS);
	option = purple_account_option_list_new("Verification type", "verification_type", verification_values);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

	// option = purple_account_option_string_new("Server", "server", TELEGRAM_TEST_SERVER);
	// prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

    option = purple_account_option_string_new("Verification key", "verification_key", NULL);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

    option = purple_account_option_string_new("Verification hash", "verification_hash", NULL);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

	// TODO: Path to public key (When you can change the server hostname,
    //        you should also be able to change the public key)

	option = purple_account_option_int_new("Port", "port", TELEGRAM_DEFAULT_PORT);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);

	_telegram_protocol = plugin;
}

static GList *tgprpl_actions(PurplePlugin * plugin, gpointer context)
{
    // return possible actions (See Libpurple doc)
	return (GList *)NULL;
}

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_PROTOCOL,
    NULL,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,
    PLUGIN_ID,
    "Telegram",
    "0.1",
    "Telegram integration.",
    "Includes support for the Telegram protocol into libpurple.",
    "Christopher Althaus <althaus.christopher@gmail.com>, Markus Endres <endresma45241@th-nuernberg.de>, Matthias Jentsch <mtthsjntsch@gmail.com>",
    "https://bitbucket.org/telegrampurple/telegram-purple",
    NULL,           // on load
    NULL,           // on unload
    NULL,           // on destroy
    NULL,           // ui specific struct
    &prpl_info,     // plugin info struct
    NULL,           // prefs info
    tgprpl_actions, // actions
    NULL,           // reserved
    NULL,           // reserved
    NULL,           // reserved
    NULL            // reserved
};


PURPLE_INIT_PLUGIN(telegram, tgprpl_init, info)
