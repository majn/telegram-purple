/**
 * @file prpl.h Protocol Plugin functions
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

/* this file should be all that prpls need to include. therefore, by including
 * this file, they should get glib, proxy, purple_connection, prpl, etc. */

#ifndef _PURPLE_PRPL_H_
#define _PURPLE_PRPL_H_

typedef struct _PurplePluginProtocolInfo PurplePluginProtocolInfo;
/** @copydoc _PurpleAttentionType */
typedef struct _PurpleAttentionType PurpleAttentionType;

/**************************************************************************/
/** @name Basic Protocol Information                                      */
/**************************************************************************/

typedef enum {
	PURPLE_ICON_SCALE_DISPLAY = 0x01,		/**< We scale the icon when we display it */
	PURPLE_ICON_SCALE_SEND = 0x02			/**< We scale the icon before we send it to the server */
} PurpleIconScaleRules;


/**
 * A description of a Buddy Icon specification.  This tells Purple what kind of image file
 * it should give this prpl, and what kind of image file it should expect back.
 * Dimensions less than 1 should be ignored and the image not scaled.
 */
typedef struct _PurpleBuddyIconSpec PurpleBuddyIconSpec;

/**
 * A description of a file transfer thumbnail specification.
 * This tells the UI if and what image formats the prpl support for file
 * transfer thumbnails.
 */
typedef struct _PurpleThumbnailSpec PurpleThumbnailSpec;

/**
 * This \#define exists just to make it easier to fill out the buddy icon
 * field in the prpl info struct for protocols that couldn't care less.
 */
#define NO_BUDDY_ICONS {NULL, 0, 0, 0, 0, 0, 0}

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "blist.h"
#include "conversation.h"
#include "ft.h"
#include "imgstore.h"
#include "media.h"
#include "notify.h"
#include "proxy.h"
#include "plugin.h"
#include "roomlist.h"
#include "status.h"
#include "whiteboard.h"


/** @copydoc PurpleBuddyIconSpec */
struct _PurpleBuddyIconSpec {
	/** This is a comma-delimited list of image formats or @c NULL if icons
	 *  are not supported.  Neither the core nor the prpl will actually
	 *  check to see if the data it's given matches this; it's entirely up
	 *  to the UI to do what it wants
	 */
	char *format;

	int min_width;                     /**< Minimum width of this icon  */
	int min_height;                    /**< Minimum height of this icon */
	int max_width;                     /**< Maximum width of this icon  */
	int max_height;                    /**< Maximum height of this icon */
	size_t max_filesize;               /**< Maximum size in bytes */
	PurpleIconScaleRules scale_rules;  /**< How to stretch this icon */
};

/** Represents an entry containing information that must be supplied by the
 *  user when joining a chat.
 */
struct proto_chat_entry {
	const char *label;       /**< User-friendly name of the entry */
	const char *identifier;  /**< Used by the PRPL to identify the option */
	gboolean required;       /**< True if it's required */
	gboolean is_int;         /**< True if the entry expects an integer */
	int min;                 /**< Minimum value in case of integer */
	int max;                 /**< Maximum value in case of integer */
	gboolean secret;         /**< True if the entry is secret (password) */
};

/** Represents "nudges" and "buzzes" that you may send to a buddy to attract
 *  their attention (or vice-versa).
 */
struct _PurpleAttentionType
{
	const char *name;                  /**< Shown in GUI elements */
	const char *incoming_description;  /**< Shown when sent */
	const char *outgoing_description;  /**< Shown when receied */
	const char *icon_name;             /**< Icon to display (optional) */
	const char *unlocalized_name;      /**< Unlocalized name for UIs needing it */

	/* Reserved fields for future purposes */
	gpointer _reserved2;
	gpointer _reserved3;
	gpointer _reserved4;
};

/**
 * Protocol options
 *
 * These should all be stuff that some plugins can do and others can't.
 */
typedef enum
{
	/**
	 * User names are unique to a chat and are not shared between rooms.
	 *
	 * XMPP lets you choose what name you want in chats, so it shouldn't
	 * be pulling the aliases from the buddy list for the chat list;
	 * it gets annoying.
	 */
	OPT_PROTO_UNIQUE_CHATNAME = 0x00000004,

	/**
	 * Chat rooms have topics.
	 *
	 * IRC and XMPP support this.
	 */
	OPT_PROTO_CHAT_TOPIC = 0x00000008,

	/**
	 * Don't require passwords for sign-in.
	 *
	 * Zephyr doesn't require passwords, so there's no
	 * need for a password prompt.
	 */
	OPT_PROTO_NO_PASSWORD = 0x00000010,

	/**
	 * Notify on new mail.
	 *
	 * MSN and Yahoo notify you when you have new mail.
	 */
	OPT_PROTO_MAIL_CHECK = 0x00000020,

	/**
	 * Images in IMs.
	 *
	 * Oscar lets you send images in direct IMs.
	 */
	OPT_PROTO_IM_IMAGE = 0x00000040,

	/**
	 * Allow passwords to be optional.
	 *
	 * Passwords in IRC are optional, and are needed for certain
	 * functionality.
	 */
	OPT_PROTO_PASSWORD_OPTIONAL = 0x00000080,

	/**
	 * Allows font size to be specified in sane point size
	 *
	 * Probably just XMPP and Y!M
	 */
	OPT_PROTO_USE_POINTSIZE = 0x00000100,

	/**
	 * Set the Register button active even when the username has not
	 * been specified.
	 *
	 * Gadu-Gadu doesn't need a username to register new account (because
	 * usernames are assigned by the server).
	 */
	OPT_PROTO_REGISTER_NOSCREENNAME = 0x00000200,

	/**
	 * Indicates that slash commands are native to this protocol.
	 * Used as a hint that unknown commands should not be sent as messages.
	 * @since 2.1.0
	 */
	OPT_PROTO_SLASH_COMMANDS_NATIVE = 0x00000400,

	/**
	 * Indicates that this protocol supports sending a user-supplied message
	 * along with an invitation.
	 * @since 2.8.0
	 */
	OPT_PROTO_INVITE_MESSAGE = 0x00000800

} PurpleProtocolOptions;

/**
 * A protocol plugin information structure.
 *
 * Every protocol plugin initializes this structure. It is the gateway
 * between purple and the protocol plugin.  Many of these callbacks can be
 * NULL.  If a callback must be implemented, it has a comment indicating so.
 */
struct _PurplePluginProtocolInfo
{
	PurpleProtocolOptions options;  /**< Protocol options.          */

	GList *user_splits;      /**< A GList of PurpleAccountUserSplit */
	GList *protocol_options; /**< A GList of PurpleAccountOption    */

	PurpleBuddyIconSpec icon_spec; /**< The icon spec. */

	/**
	 * Returns the base icon name for the given buddy and account.
	 * If buddy is NULL and the account is non-NULL, it will return the
	 * name to use for the account's icon. If both are NULL, it will
	 * return the name to use for the protocol's icon.
	 *
	 * This must be implemented.
	 */
	const char *(*list_icon)(PurpleAccount *account, PurpleBuddy *buddy);

	/**
	 * Fills the four char**'s with string identifiers for "emblems"
	 * that the UI will interpret and display as relevant
	 */
	const char *(*list_emblem)(PurpleBuddy *buddy);

	/**
	 * Gets a short string representing this buddy's status.  This will
	 * be shown on the buddy list.
	 */
	char *(*status_text)(PurpleBuddy *buddy);

	/**
	 * Allows the prpl to add text to a buddy's tooltip.
	 */
	void (*tooltip_text)(PurpleBuddy *buddy, PurpleNotifyUserInfo *user_info, gboolean full);

	/**
	 * Returns a list of #PurpleStatusType which exist for this account;
	 * this must be implemented, and must add at least the offline and
	 * online states.
	 */
	GList *(*status_types)(PurpleAccount *account);

	/**
	 * Returns a list of #PurpleMenuAction structs, which represent extra
	 * actions to be shown in (for example) the right-click menu for @a
	 * node.
	 */
	GList *(*blist_node_menu)(PurpleBlistNode *node);

	/**
	 * Returns a list of #proto_chat_entry structs, which represent
	 * information required by the PRPL to join a chat. libpurple will
	 * call join_chat along with the information filled by the user.
	 *
	 * @return A list of #proto_chat_entry structs
	 */
	GList *(*chat_info)(PurpleConnection *);

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
	GHashTable *(*chat_info_defaults)(PurpleConnection *, const char *chat_name);

	/* All the server-related functions */

	/** This must be implemented. */
	void (*login)(PurpleAccount *);

	/** This must be implemented. */
	void (*close)(PurpleConnection *);

	/**
	 * This PRPL function should return a positive value on success.
	 * If the message is too big to be sent, return -E2BIG.  If
	 * the account is not connected, return -ENOTCONN.  If the
	 * PRPL is unable to send the message for another reason, return
	 * some other negative value.  You can use one of the valid
	 * errno values, or just big something.  If the message should
	 * not be echoed to the conversation window, return 0.
	 */
	int  (*send_im)(PurpleConnection *, const char *who,
					const char *message,
					PurpleMessageFlags flags);

	void (*set_info)(PurpleConnection *, const char *info);

	/**
	 * @return If this protocol requires the PURPLE_TYPING message to
	 *         be sent repeatedly to signify that the user is still
	 *         typing, then the PRPL should return the number of
	 *         seconds to wait before sending a subsequent notification.
	 *         Otherwise the PRPL should return 0.
	 */
	unsigned int (*send_typing)(PurpleConnection *, const char *name, PurpleTypingState state);

	/**
	 * Should arrange for purple_notify_userinfo() to be called with
	 * @a who's user info.
	 */
	void (*get_info)(PurpleConnection *, const char *who);
	void (*set_status)(PurpleAccount *account, PurpleStatus *status);

	void (*set_idle)(PurpleConnection *, int idletime);
	void (*change_passwd)(PurpleConnection *, const char *old_pass,
						  const char *new_pass);
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
	void (*add_buddy)(PurpleConnection *, PurpleBuddy *buddy, PurpleGroup *group);
	void (*add_buddies)(PurpleConnection *, GList *buddies, GList *groups);
	void (*remove_buddy)(PurpleConnection *, PurpleBuddy *buddy, PurpleGroup *group);
	void (*remove_buddies)(PurpleConnection *, GList *buddies, GList *groups);
	void (*add_permit)(PurpleConnection *, const char *name);
	void (*add_deny)(PurpleConnection *, const char *name);
	void (*rem_permit)(PurpleConnection *, const char *name);
	void (*rem_deny)(PurpleConnection *, const char *name);
	void (*set_permit_deny)(PurpleConnection *);

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
	void (*join_chat)(PurpleConnection *, GHashTable *components);

	/**
	 * Called when the user refuses a chat invitation.
	 *
	 * @param components A hashtable containing information required to
	 *                   join the chat as passed to #serv_got_chat_invite.
	 */
	void (*reject_chat)(PurpleConnection *, GHashTable *components);

	/**
	 * Returns a chat name based on the information in components. Use
	 * #chat_info_defaults if you instead need to generate a hashtable
	 * from a chat name.
	 *
	 * @param components A hashtable containing information about the chat.
	 */
	char *(*get_chat_name)(GHashTable *components);

	/**
	 * Invite a user to join a chat.
	 *
	 * @param id      The id of the chat to invite the user to.
	 * @param message A message displayed to the user when the invitation
	 *                is received.
	 * @param who     The name of the user to send the invation to.
	 */
	void (*chat_invite)(PurpleConnection *, int id,
						const char *message, const char *who);
	/**
	 * Called when the user requests leaving a chat.
	 *
	 * @param id The id of the chat to leave
	 */
	void (*chat_leave)(PurpleConnection *, int id);

	/**
	 * Send a whisper to a user in a chat.
	 *
	 * @param id      The id of the chat.
	 * @param who     The name of the user to send the whisper to.
	 * @param message The message of the whisper.
	 */
	void (*chat_whisper)(PurpleConnection *, int id,
						 const char *who, const char *message);

	/**
	 * Send a message to a chat.
	 * This PRPL function should return a positive value on success.
	 * If the message is too big to be sent, return -E2BIG.  If
	 * the account is not connected, return -ENOTCONN.  If the
	 * PRPL is unable to send the message for another reason, return
	 * some other negative value.  You can use one of the valid
	 * errno values, or just big something.  If the message should
	 * not be echoed to the conversation window, return 0.
	 *
	 * @param id      The id of the chat to send the message to.
	 * @param message The message to send to the chat.
	 * @param flags   A bitwise OR of #PurpleMessageFlags representing
	 *                message flags.
	 * @return 	  A positive number or 0 in case of succes,
	 *                a negative error number in case of failure.
	 */
	int  (*chat_send)(PurpleConnection *, int id, const char *message, PurpleMessageFlags flags);

	/** If implemented, this will be called regularly for this prpl's
	 *  active connections.  You'd want to do this if you need to repeatedly
	 *  send some kind of keepalive packet to the server to avoid being
	 *  disconnected.  ("Regularly" is defined by
	 *  <code>KEEPALIVE_INTERVAL</code> in <tt>libpurple/connection.c</tt>.)
	 */
	void (*keepalive)(PurpleConnection *);

	/** new user registration */
	void (*register_user)(PurpleAccount *);

	/**
	 * @deprecated Use #PurplePluginProtocolInfo.get_info instead.
	 */
	void (*get_cb_info)(PurpleConnection *, int, const char *who);
	/**
	 * @deprecated Use #PurplePluginProtocolInfo.get_cb_real_name and
	 *             #PurplePluginProtocolInfo.status_text instead.
	 */
	void (*get_cb_away)(PurpleConnection *, int, const char *who);

	/** save/store buddy's alias on server list/roster */
	void (*alias_buddy)(PurpleConnection *, const char *who,
						const char *alias);

	/** change a buddy's group on a server list/roster */
	void (*group_buddy)(PurpleConnection *, const char *who,
						const char *old_group, const char *new_group);

	/** rename a group on a server list/roster */
	void (*rename_group)(PurpleConnection *, const char *old_name,
						 PurpleGroup *group, GList *moved_buddies);

	void (*buddy_free)(PurpleBuddy *);

	void (*convo_closed)(PurpleConnection *, const char *who);

	/**
	 *  Convert the username @a who to its canonical form.  (For example,
	 *  AIM treats "fOo BaR" and "foobar" as the same user; this function
	 *  should return the same normalized string for both of those.)
	 */
	const char *(*normalize)(const PurpleAccount *, const char *who);

	/**
	 * Set the buddy icon for the given connection to @a img.  The prpl
	 * does NOT own a reference to @a img; if it needs one, it must
	 * #purple_imgstore_ref(@a img) itself.
	 */
	void (*set_buddy_icon)(PurpleConnection *, PurpleStoredImage *img);

	void (*remove_group)(PurpleConnection *gc, PurpleGroup *group);

	/** Gets the real name of a participant in a chat.  For example, on
	 *  XMPP this turns a chat room nick <tt>foo</tt> into
	 *  <tt>room\@server/foo</tt>
	 *  @param gc  the connection on which the room is.
	 *  @param id  the ID of the chat room.
	 *  @param who the nickname of the chat participant.
	 *  @return    the real name of the participant.  This string must be
	 *             freed by the caller.
	 */
	char *(*get_cb_real_name)(PurpleConnection *gc, int id, const char *who);

	void (*set_chat_topic)(PurpleConnection *gc, int id, const char *topic);

	PurpleChat *(*find_blist_chat)(PurpleAccount *account, const char *name);

	/* room listing prpl callbacks */
	PurpleRoomlist *(*roomlist_get_list)(PurpleConnection *gc);
	void (*roomlist_cancel)(PurpleRoomlist *list);
	void (*roomlist_expand_category)(PurpleRoomlist *list, PurpleRoomlistRoom *category);

	/* file transfer callbacks */
	gboolean (*can_receive_file)(PurpleConnection *, const char *who);
	void (*send_file)(PurpleConnection *, const char *who, const char *filename);
	PurpleXfer *(*new_xfer)(PurpleConnection *, const char *who);

	/** Checks whether offline messages to @a buddy are supported.
	 *  @return @c TRUE if @a buddy can be sent messages while they are
	 *          offline, or @c FALSE if not.
	 */
	gboolean (*offline_message)(const PurpleBuddy *buddy);

	PurpleWhiteboardPrplOps *whiteboard_prpl_ops;

	/** For use in plugins that may understand the underlying protocol */
	int (*send_raw)(PurpleConnection *gc, const char *buf, int len);

	/* room list serialize */
	char *(*roomlist_room_serialize)(PurpleRoomlistRoom *room);

	/** Remove the user from the server.  The account can either be
	 * connected or disconnected. After the removal is finished, the
	 * connection will stay open and has to be closed!
	 */
	/* This is here rather than next to register_user for API compatibility
	 * reasons.
	 */
	void (*unregister_user)(PurpleAccount *, PurpleAccountUnregistrationCb cb, void *user_data);

	/* Attention API for sending & receiving zaps/nudges/buzzes etc. */
	gboolean (*send_attention)(PurpleConnection *gc, const char *username, guint type);
	GList *(*get_attention_types)(PurpleAccount *acct);

	/**
	 * The size of the PurplePluginProtocolInfo. This should always be sizeof(PurplePluginProtocolInfo).
	 * This allows adding more functions to this struct without requiring a major version bump.
	 */
	unsigned long struct_size;

	/* NOTE:
	 * If more functions are added, they should accessed using the following syntax:
	 *
	 *		if (PURPLE_PROTOCOL_PLUGIN_HAS_FUNC(prpl, new_function))
	 *			prpl->new_function(...);
	 *
	 * instead of
	 *
	 *		if (prpl->new_function != NULL)
	 *			prpl->new_function(...);
	 *
	 * The PURPLE_PROTOCOL_PLUGIN_HAS_FUNC macro can be used for the older member
	 * functions (e.g. login, send_im etc.) too.
	 */

	/** This allows protocols to specify additional strings to be used for
	 * various purposes.  The idea is to stuff a bunch of strings in this hash
	 * table instead of expanding the struct for every addition.  This hash
	 * table is allocated every call and MUST be unrefed by the caller.
	 *
	 * @param account The account to specify.  This can be NULL.
	 * @return The protocol's string hash table. The hash table should be
	 *         destroyed by the caller when it's no longer needed.
	 */
	GHashTable *(*get_account_text_table)(PurpleAccount *account);

	/**
	 * Initiate a media session with the given contact.
	 *
	 * @param account The account to initiate the media session on.
	 * @param who The remote user to initiate the session with.
	 * @param type The type of media session to initiate.
	 * @return TRUE if the call succeeded else FALSE. (Doesn't imply the media session or stream will be successfully created)
	 */
	gboolean (*initiate_media)(PurpleAccount *account, const char *who,
					PurpleMediaSessionType type);

	/**
	 * Checks to see if the given contact supports the given type of media session.
	 *
	 * @param account The account the contact is on.
	 * @param who The remote user to check for media capability with.
	 * @return The media caps the contact supports.
	 */
	PurpleMediaCaps (*get_media_caps)(PurpleAccount *account,
					  const char *who);

	/**
	 * Returns an array of "PurpleMood"s, with the last one having
	 * "mood" set to @c NULL.
	 * @since 2.7.0
	 */
	PurpleMood *(*get_moods)(PurpleAccount *account);

	/**
	 * Set the user's "friendly name" (or alias or nickname or
	 * whatever term you want to call it) on the server.  The
	 * protocol plugin should call success_cb or failure_cb
	 * *asynchronously* (if it knows immediately that the set will fail,
	 * call one of the callbacks from an idle/0-second timeout) depending
	 * on if the nickname is set successfully.
	 *
	 * @param gc    The connection for which to set an alias
	 * @param alias The new server-side alias/nickname for this account,
	 *              or NULL to unset the alias/nickname (or return it to
	 *              a protocol-specific "default").
	 * @param success_cb Callback to be called if the public alias is set
	 * @param failure_cb Callback to be called if setting the public alias
	 *                   fails
	 * @see purple_account_set_public_alias
	 * @since 2.7.0
	 */
	void (*set_public_alias)(PurpleConnection *gc, const char *alias,
	                         PurpleSetPublicAliasSuccessCallback success_cb,
	                         PurpleSetPublicAliasFailureCallback failure_cb);
	/**
	 * Retrieve the user's "friendly name" as set on the server.
	 * The protocol plugin should call success_cb or failure_cb
	 * *asynchronously* (even if it knows immediately that the get will fail,
	 * call one of the callbacks from an idle/0-second timeout) depending
	 * on if the nickname is retrieved.
	 *
	 * @param gc    The connection for which to retireve the alias
	 * @param success_cb Callback to be called with the retrieved alias
	 * @param failure_cb Callback to be called if the prpl is unable to
	 *                   retrieve the alias
	 * @see purple_account_get_public_alias
	 * @since 2.7.0
	 */
	void (*get_public_alias)(PurpleConnection *gc,
	                         PurpleGetPublicAliasSuccessCallback success_cb,
	                         PurpleGetPublicAliasFailureCallback failure_cb);

	/**
	 * Add a buddy to a group on the server.
	 *
	 * This PRPL function may be called in situations in which the buddy is
	 * already in the specified group. If the protocol supports
	 * authorization and the user is not already authorized to see the
	 * status of \a buddy, \a add_buddy should request authorization.
	 *
	 * If authorization is required, then use the supplied invite message.
	 *
	 * @since 2.8.0
	 */
	void (*add_buddy_with_invite)(PurpleConnection *pc, PurpleBuddy *buddy, PurpleGroup *group, const char *message);
	void (*add_buddies_with_invite)(PurpleConnection *pc, GList *buddies, GList *groups, const char *message);
};

#define PURPLE_PROTOCOL_PLUGIN_HAS_FUNC(prpl, member) \
	(((G_STRUCT_OFFSET(PurplePluginProtocolInfo, member) < G_STRUCT_OFFSET(PurplePluginProtocolInfo, struct_size)) \
	  || (G_STRUCT_OFFSET(PurplePluginProtocolInfo, member) < prpl->struct_size)) && \
	 prpl->member != NULL)


#define PURPLE_IS_PROTOCOL_PLUGIN(plugin) \
	((plugin)->info->type == PURPLE_PLUGIN_PROTOCOL)

#define PURPLE_PLUGIN_PROTOCOL_INFO(plugin) \
	((PurplePluginProtocolInfo *)(plugin)->info->extra_info)

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Attention Type API                                              */
/**************************************************************************/
/*@{*/

/**
 * Creates a new #PurpleAttentionType object and sets its mandatory parameters.
 *
 * @param ulname A non-localized string that can be used by UIs in need of such
 *               non-localized strings.  This should be the same as @a name,
 *               without localization.
 * @param name A localized string that the UI may display for the event. This
 *             should be the same string as @a ulname, with localization.
 * @param inc_desc A localized description shown when the event is received.
 * @param out_desc A localized description shown when the event is sent.
 * @return A pointer to the new object.
 * @since 2.4.0
 */
PurpleAttentionType *purple_attention_type_new(const char *ulname, const char *name,
								const char *inc_desc, const char *out_desc);

/**
 * Sets the displayed name of the attention-demanding event.
 *
 * @param type The attention type.
 * @param name The localized name that will be displayed by UIs. This should be
 *             the same string given as the unlocalized name, but with
 *             localization.
 * @since 2.4.0
 */
void purple_attention_type_set_name(PurpleAttentionType *type, const char *name);

/**
 * Sets the description of the attention-demanding event shown in  conversations
 * when the event is received.
 *
 * @param type The attention type.
 * @param desc The localized description for incoming events.
 * @since 2.4.0
 */
void purple_attention_type_set_incoming_desc(PurpleAttentionType *type, const char *desc);

/**
 * Sets the description of the attention-demanding event shown in conversations
 * when the event is sent.
 *
 * @param type The attention type.
 * @param desc The localized description for outgoing events.
 * @since 2.4.0
 */
void purple_attention_type_set_outgoing_desc(PurpleAttentionType *type, const char *desc);

/**
 * Sets the name of the icon to display for the attention event; this is optional.
 *
 * @param type The attention type.
 * @param name The icon's name.
 * @note Icons are optional for attention events.
 * @since 2.4.0
 */
void purple_attention_type_set_icon_name(PurpleAttentionType *type, const char *name);

/**
 * Sets the unlocalized name of the attention event; some UIs may need this,
 * thus it is required.
 *
 * @param type The attention type.
 * @param ulname The unlocalized name.  This should be the same string given as
 *               the localized name, but without localization.
 * @since 2.4.0
 */
void purple_attention_type_set_unlocalized_name(PurpleAttentionType *type, const char *ulname);

/**
 * Get the attention type's name as displayed by the UI.
 *
 * @param type The attention type.
 * @return The name.
 * @since 2.4.0
 */
const char *purple_attention_type_get_name(const PurpleAttentionType *type);

/**
 * Get the attention type's description shown when the event is received.
 *
 * @param type The attention type.
 * @return The description.
 * @since 2.4.0
 */
const char *purple_attention_type_get_incoming_desc(const PurpleAttentionType *type);

/**
 * Get the attention type's description shown when the event is sent.
 *
 * @param type The attention type.
 * @return The description.
 * @since 2.4.0
 */
const char *purple_attention_type_get_outgoing_desc(const PurpleAttentionType *type);

/**
 * Get the attention type's icon name.
 *
 * @param type The attention type.
 * @return The icon name or @c NULL if unset/empty.
 * @note Icons are optional for attention events.
 * @since 2.4.0
 */
const char *purple_attention_type_get_icon_name(const PurpleAttentionType *type);

/**
 * Get the attention type's unlocalized name; this is useful for some UIs.
 *
 * @param type The attention type
 * @return The unlocalized name.
 * @since 2.4.0
 */
const char *purple_attention_type_get_unlocalized_name(const PurpleAttentionType *type);

/*@}*/

/**************************************************************************/
/** @name Protocol Plugin API                                             */
/**************************************************************************/
/*@{*/

/**
 * Notifies Purple that our account's idle state and time have changed.
 *
 * This is meant to be called from protocol plugins.
 *
 * @param account   The account.
 * @param idle      The user's idle state.
 * @param idle_time The user's idle time.
 */
void purple_prpl_got_account_idle(PurpleAccount *account, gboolean idle,
								time_t idle_time);

/**
 * Notifies Purple of our account's log-in time.
 *
 * This is meant to be called from protocol plugins.
 *
 * @param account    The account the user is on.
 * @param login_time The user's log-in time.
 */
void purple_prpl_got_account_login_time(PurpleAccount *account, time_t login_time);

/**
 * Notifies Purple that our account's status has changed.
 *
 * This is meant to be called from protocol plugins.
 *
 * @param account   The account the user is on.
 * @param status_id The status ID.
 * @param ...       A NULL-terminated list of attribute IDs and values,
 *                  beginning with the value for @a attr_id.
 */
void purple_prpl_got_account_status(PurpleAccount *account,
								  const char *status_id, ...) G_GNUC_NULL_TERMINATED;

/**
 * Notifies Purple that our account's actions have changed. This is only
 * called after the initial connection. Emits the account-actions-changed
 * signal.
 *
 * This is meant to be called from protocol plugins.
 *
 * @param account   The account.
 *
 * @see account-actions-changed
 * @since 2.6.0
 */
void purple_prpl_got_account_actions(PurpleAccount *account);

/**
 * Notifies Purple that a buddy's idle state and time have changed.
 *
 * This is meant to be called from protocol plugins.
 *
 * @param account   The account the user is on.
 * @param name      The name of the buddy.
 * @param idle      The user's idle state.
 * @param idle_time The user's idle time.  This is the time at
 *                  which the user became idle, in seconds since
 *                  the epoch.  If the PRPL does not know this value
 *                  then it should pass 0.
 */
void purple_prpl_got_user_idle(PurpleAccount *account, const char *name,
							 gboolean idle, time_t idle_time);

/**
 * Notifies Purple of a buddy's log-in time.
 *
 * This is meant to be called from protocol plugins.
 *
 * @param account    The account the user is on.
 * @param name       The name of the buddy.
 * @param login_time The user's log-in time.
 */
void purple_prpl_got_user_login_time(PurpleAccount *account, const char *name,
								   time_t login_time);

/**
 * Notifies Purple that a buddy's status has been activated.
 *
 * This is meant to be called from protocol plugins.
 *
 * @param account   The account the user is on.
 * @param name      The name of the buddy.
 * @param status_id The status ID.
 * @param ...       A NULL-terminated list of attribute IDs and values,
 *                  beginning with the value for @a attr_id.
 */
void purple_prpl_got_user_status(PurpleAccount *account, const char *name,
							   const char *status_id, ...) G_GNUC_NULL_TERMINATED;

/**
 * Notifies libpurple that a buddy's status has been deactivated
 *
 * This is meant to be called from protocol plugins.
 *
 * @param account   The account the user is on.
 * @param name      The name of the buddy.
 * @param status_id The status ID.
 */
void purple_prpl_got_user_status_deactive(PurpleAccount *account, const char *name,
					const char *status_id);

/**
 * Informs the server that our account's status changed.
 *
 * @param account    The account the user is on.
 * @param old_status The previous status.
 * @param new_status The status that was activated, or deactivated
 *                   (in the case of independent statuses).
 */
void purple_prpl_change_account_status(PurpleAccount *account,
									 PurpleStatus *old_status,
									 PurpleStatus *new_status);

/**
 * Retrieves the list of stock status types from a prpl.
 *
 * @param account The account the user is on.
 * @param presence The presence for which we're going to get statuses
 *
 * @return List of statuses
 */
GList *purple_prpl_get_statuses(PurpleAccount *account, PurplePresence *presence);

/**
 * Send an attention request message.
 *
 * @param gc The connection to send the message on.
 * @param who Whose attention to request.
 * @param type_code An index into the prpl's attention_types list determining the type
 *        of the attention request command to send. 0 if prpl only defines one
 *        (for example, Yahoo and MSN), but some protocols define more (MySpaceIM).
 *
 * Note that you can't send arbitrary PurpleAttentionType's, because there is
 * only a fixed set of attention commands.
 *
 * @since 2.5.0
 */
void purple_prpl_send_attention(PurpleConnection *gc, const char *who, guint type_code);

/**
 * Process an incoming attention message.
 *
 * @param gc The connection that received the attention message.
 * @param who Who requested your attention.
 * @param type_code An index into the prpl's attention_types list determining the type
 *        of the attention request command to send.
 *
 * @since 2.5.0
 */
void purple_prpl_got_attention(PurpleConnection *gc, const char *who, guint type_code);

/**
 * Process an incoming attention message in a chat.
 *
 * @param gc The connection that received the attention message.
 * @param id The chat id.
 * @param who Who requested your attention.
 * @param type_code An index into the prpl's attention_types list determining the type
 *        of the attention request command to send.
 *
 * @since 2.5.0
 */
void purple_prpl_got_attention_in_chat(PurpleConnection *gc, int id, const char *who, guint type_code);

/**
 * Determines if the contact supports the given media session type.
 *
 * @param account The account the user is on.
 * @param who The name of the contact to check capabilities for.
 *
 * @return The media caps the contact supports.
 */
PurpleMediaCaps purple_prpl_get_media_caps(PurpleAccount *account,
				  const char *who);

/**
 * Initiates a media session with the given contact.
 *
 * @param account The account the user is on.
 * @param who The name of the contact to start a session with.
 * @param type The type of media session to start.
 *
 * @return TRUE if the call succeeded else FALSE. (Doesn't imply the media session or stream will be successfully created)
 */
gboolean purple_prpl_initiate_media(PurpleAccount *account,
					const char *who,
					PurpleMediaSessionType type);

/**
 * Signals that the prpl received capabilities for the given contact.
 *
 * This function is intended to be used only by prpls.
 *
 * @param account The account the user is on.
 * @param who The name of the contact for which capabilities have been received.
 * @since 2.7.0
 */
void purple_prpl_got_media_caps(PurpleAccount *account, const char *who);

/*@}*/

/**************************************************************************/
/** @name Protocol Plugin Subsystem API                                   */
/**************************************************************************/
/*@{*/

/**
 * Finds a protocol plugin structure of the specified type.
 *
 * @param id The protocol plugin;
 */
PurplePlugin *purple_find_prpl(const char *id);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PRPL_H_ */
