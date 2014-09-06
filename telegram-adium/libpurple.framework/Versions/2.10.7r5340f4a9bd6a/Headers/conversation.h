/**
 * @file conversation.h Conversation API
 * @ingroup core
 * @see @ref conversation-signals
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
#ifndef _PURPLE_CONVERSATION_H_
#define _PURPLE_CONVERSATION_H_

/**************************************************************************/
/** Data Structures                                                       */
/**************************************************************************/


/** @copydoc _PurpleConversationUiOps */
typedef struct _PurpleConversationUiOps PurpleConversationUiOps;
/** @copydoc _PurpleConversation */
typedef struct _PurpleConversation      PurpleConversation;
/** @copydoc _PurpleConvIm */
typedef struct _PurpleConvIm            PurpleConvIm;
/** @copydoc _PurpleConvChat */
typedef struct _PurpleConvChat          PurpleConvChat;
/** @copydoc _PurpleConvChatBuddy */
typedef struct _PurpleConvChatBuddy     PurpleConvChatBuddy;
/** @copydoc _PurpleConvMessage */
typedef struct _PurpleConvMessage       PurpleConvMessage;

/**
 * A type of conversation.
 */
typedef enum
{
	PURPLE_CONV_TYPE_UNKNOWN = 0, /**< Unknown conversation type. */
	PURPLE_CONV_TYPE_IM,          /**< Instant Message.           */
	PURPLE_CONV_TYPE_CHAT,        /**< Chat room.                 */
	PURPLE_CONV_TYPE_MISC,        /**< A misc. conversation.      */
	PURPLE_CONV_TYPE_ANY          /**< Any type of conversation.  */

} PurpleConversationType;

/**
 * Conversation update type.
 */
typedef enum
{
	PURPLE_CONV_UPDATE_ADD = 0, /**< The buddy associated with the conversation
	                               was added.   */
	PURPLE_CONV_UPDATE_REMOVE,  /**< The buddy associated with the conversation
	                               was removed. */
	PURPLE_CONV_UPDATE_ACCOUNT, /**< The purple_account was changed. */
	PURPLE_CONV_UPDATE_TYPING,  /**< The typing state was updated. */
	PURPLE_CONV_UPDATE_UNSEEN,  /**< The unseen state was updated. */
	PURPLE_CONV_UPDATE_LOGGING, /**< Logging for this conversation was
	                               enabled or disabled. */
	PURPLE_CONV_UPDATE_TOPIC,   /**< The topic for a chat was updated. */
	/*
	 * XXX These need to go when we implement a more generic core/UI event
	 * system.
	 */
	PURPLE_CONV_ACCOUNT_ONLINE,  /**< One of the user's accounts went online.  */
	PURPLE_CONV_ACCOUNT_OFFLINE, /**< One of the user's accounts went offline. */
	PURPLE_CONV_UPDATE_AWAY,     /**< The other user went away.                */
	PURPLE_CONV_UPDATE_ICON,     /**< The other user's buddy icon changed.     */
	PURPLE_CONV_UPDATE_TITLE,
	PURPLE_CONV_UPDATE_CHATLEFT,

	PURPLE_CONV_UPDATE_FEATURES  /**< The features for a chat have changed */

} PurpleConvUpdateType;

/**
 * The typing state of a user.
 */
typedef enum
{
	PURPLE_NOT_TYPING = 0,  /**< Not typing.                 */
	PURPLE_TYPING,          /**< Currently typing.           */
	PURPLE_TYPED            /**< Stopped typing momentarily. */

} PurpleTypingState;

/**
 * Flags applicable to a message. Most will have send, recv or system.
 */
typedef enum
{
	PURPLE_MESSAGE_SEND        = 0x0001, /**< Outgoing message.        */
	PURPLE_MESSAGE_RECV        = 0x0002, /**< Incoming message.        */
	PURPLE_MESSAGE_SYSTEM      = 0x0004, /**< System message.          */
	PURPLE_MESSAGE_AUTO_RESP   = 0x0008, /**< Auto response.           */
	PURPLE_MESSAGE_ACTIVE_ONLY = 0x0010,  /**< Hint to the UI that this
	                                        message should not be
	                                        shown in conversations
	                                        which are only open for
	                                        internal UI purposes
	                                        (e.g. for contact-aware
	                                         conversations).           */
	PURPLE_MESSAGE_NICK        = 0x0020, /**< Contains your nick.      */
	PURPLE_MESSAGE_NO_LOG      = 0x0040, /**< Do not log.              */
	PURPLE_MESSAGE_WHISPER     = 0x0080, /**< Whispered message.       */
	PURPLE_MESSAGE_ERROR       = 0x0200, /**< Error message.           */
	PURPLE_MESSAGE_DELAYED     = 0x0400, /**< Delayed message.         */
	PURPLE_MESSAGE_RAW         = 0x0800, /**< "Raw" message - don't
	                                        apply formatting         */
	PURPLE_MESSAGE_IMAGES      = 0x1000, /**< Message contains images  */
	PURPLE_MESSAGE_NOTIFY      = 0x2000, /**< Message is a notification */
	PURPLE_MESSAGE_NO_LINKIFY  = 0x4000, /**< Message should not be auto-
										   linkified @since 2.1.0 */
	PURPLE_MESSAGE_INVISIBLE   = 0x8000  /**< Message should not be displayed */
} PurpleMessageFlags;

/**
 * Flags applicable to users in Chats.
 */
typedef enum
{
	PURPLE_CBFLAGS_NONE          = 0x0000, /**< No flags                     */
	PURPLE_CBFLAGS_VOICE         = 0x0001, /**< Voiced user or "Participant" */
	PURPLE_CBFLAGS_HALFOP        = 0x0002, /**< Half-op                      */
	PURPLE_CBFLAGS_OP            = 0x0004, /**< Channel Op or Moderator      */
	PURPLE_CBFLAGS_FOUNDER       = 0x0008, /**< Channel Founder              */
	PURPLE_CBFLAGS_TYPING        = 0x0010, /**< Currently typing             */
	PURPLE_CBFLAGS_AWAY          = 0x0020  /**< Currently away. @since 2.8.0 */

} PurpleConvChatBuddyFlags;

#include "account.h"
#include "buddyicon.h"
#include "log.h"
#include "server.h"

/**
 * Conversation operations and events.
 *
 * Any UI representing a conversation must assign a filled-out
 * PurpleConversationUiOps structure to the PurpleConversation.
 */
struct _PurpleConversationUiOps
{
	/** Called when @a conv is created (but before the @ref
	 *  conversation-created signal is emitted).
	 */
	void (*create_conversation)(PurpleConversation *conv);

	/** Called just before @a conv is freed. */
	void (*destroy_conversation)(PurpleConversation *conv);
	/** Write a message to a chat.  If this field is @c NULL, libpurple will
	 *  fall back to using #write_conv.
	 *  @see purple_conv_chat_write()
	 */
	void (*write_chat)(PurpleConversation *conv, const char *who,
	                   const char *message, PurpleMessageFlags flags,
	                   time_t mtime);
	/** Write a message to an IM conversation.  If this field is @c NULL,
	 *  libpurple will fall back to using #write_conv.
	 *  @see purple_conv_im_write()
	 */
	void (*write_im)(PurpleConversation *conv, const char *who,
	                 const char *message, PurpleMessageFlags flags,
	                 time_t mtime);
	/** Write a message to a conversation.  This is used rather than the
	 *  chat- or im-specific ops for errors, system messages (such as "x is
	 *  now know as y"), and as the fallback if #write_im and #write_chat
	 *  are not implemented.  It should be implemented, or the UI will miss
	 *  conversation error messages and your users will hate you.
	 *
	 *  @see purple_conversation_write()
	 */
	void (*write_conv)(PurpleConversation *conv,
	                   const char *name,
	                   const char *alias,
	                   const char *message,
	                   PurpleMessageFlags flags,
	                   time_t mtime);

	/** Add @a cbuddies to a chat.
	 *  @param cbuddies      A @c GList of #PurpleConvChatBuddy structs.
	 *  @param new_arrivals  Whether join notices should be shown.
	 *                       (Join notices are actually written to the
	 *                       conversation by #purple_conv_chat_add_users().)
	 */
	void (*chat_add_users)(PurpleConversation *conv,
	                       GList *cbuddies,
	                       gboolean new_arrivals);
	/** Rename the user in this chat named @a old_name to @a new_name.  (The
	 *  rename message is written to the conversation by libpurple.)
	 *  @param new_alias  @a new_name's new alias, if they have one.
	 *  @see purple_conv_chat_add_users()
	 */
	void (*chat_rename_user)(PurpleConversation *conv, const char *old_name,
	                         const char *new_name, const char *new_alias);
	/** Remove @a users from a chat.
	 *  @param users    A @c GList of <tt>const char *</tt>s.
	 *  @see purple_conv_chat_rename_user()
	 */
	void (*chat_remove_users)(PurpleConversation *conv, GList *users);
	/** Called when a user's flags are changed.
	 *  @see purple_conv_chat_user_set_flags()
	 */
	void (*chat_update_user)(PurpleConversation *conv, const char *user);

	/** Present this conversation to the user; for example, by displaying
	 *  the IM dialog.
	 */
	void (*present)(PurpleConversation *conv);

	/** If this UI has a concept of focus (as in a windowing system) and
	 *  this conversation has the focus, return @c TRUE; otherwise, return
	 *  @c FALSE.
	 */
	gboolean (*has_focus)(PurpleConversation *conv);

	/* Custom Smileys */
	gboolean (*custom_smiley_add)(PurpleConversation *conv, const char *smile, gboolean remote);
	void (*custom_smiley_write)(PurpleConversation *conv, const char *smile,
	                            const guchar *data, gsize size);
	void (*custom_smiley_close)(PurpleConversation *conv, const char *smile);

	/** Prompt the user for confirmation to send @a message.  This function
	 *  should arrange for the message to be sent if the user accepts.  If
	 *  this field is @c NULL, libpurple will fall back to using
	 *  #purple_request_action().
	 */
	void (*send_confirm)(PurpleConversation *conv, const char *message);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

/**
 * Data specific to Instant Messages.
 */
struct _PurpleConvIm
{
	PurpleConversation *conv;            /**< The parent conversation.     */

	PurpleTypingState typing_state;      /**< The current typing state.    */
	guint  typing_timeout;             /**< The typing timer handle.     */
	time_t type_again;                 /**< The type again time.         */
	guint  send_typed_timeout;         /**< The type again timer handle. */

	PurpleBuddyIcon *icon;               /**< The buddy icon.              */
};

/**
 * Data specific to Chats.
 */
struct _PurpleConvChat
{
	PurpleConversation *conv;          /**< The parent conversation.      */

	GList *in_room;                  /**< The users in the room.
	                                  *   @deprecated Will be removed in 3.0.0
									  */
	GList *ignored;                  /**< Ignored users.                */
	char  *who;                      /**< The person who set the topic. */
	char  *topic;                    /**< The topic.                    */
	int    id;                       /**< The chat ID.                  */
	char *nick;                      /**< Your nick in this chat.       */

	gboolean left;                   /**< We left the chat and kept the window open */
	GHashTable *users;               /**< Hash table of the users in the room.
	                                  *   @since 2.9.0
	                                  */
};

/**
 * Data for "Chat Buddies"
 */
struct _PurpleConvChatBuddy
{
	char *name;                      /**< The chat participant's name in the chat. */
	char *alias;                     /**< The chat participant's alias, if known;
	                                  *   @a NULL otherwise.
	                                  */
	char *alias_key;                 /**< A string by which this buddy will be sorted,
	                                  *   or @c NULL if the buddy should be sorted by
	                                  *   its @c name.  (This is currently always @c
	                                  *   NULL.)
	                                  */
	gboolean buddy;                  /**< @a TRUE if this chat participant is on the
	                                  *   buddy list; @a FALSE otherwise.
	                                  */
	PurpleConvChatBuddyFlags flags;  /**< A bitwise OR of flags for this participant,
	                                  *   such as whether they are a channel operator.
	                                  */
	GHashTable *attributes;          /**< A hash table of attributes about the user, such as
                                    *   real name, user@host, etc.
                                    */
	gpointer ui_data;                /** < The UI can put whatever it wants here. */
};

/**
 * Description of a conversation message
 *
 * @since 2.2.0
 */
struct _PurpleConvMessage
{
	char *who;
	char *what;
	PurpleMessageFlags flags;
	time_t when;
	PurpleConversation *conv;  /**< @since 2.3.0 */
	char *alias;               /**< @since 2.3.0 */
};

/**
 * A core representation of a conversation between two or more people.
 *
 * The conversation can be an IM or a chat.
 */
struct _PurpleConversation
{
	PurpleConversationType type;  /**< The type of conversation.          */

	PurpleAccount *account;       /**< The user using this conversation.  */


	char *name;                 /**< The name of the conversation.      */
	char *title;                /**< The window title.                  */

	gboolean logging;           /**< The status of logging.             */

	GList *logs;                /**< This conversation's logs           */

	union
	{
		PurpleConvIm   *im;       /**< IM-specific data.                  */
		PurpleConvChat *chat;     /**< Chat-specific data.                */
		void *misc;             /**< Misc. data.                        */

	} u;

	PurpleConversationUiOps *ui_ops;           /**< UI-specific operations. */
	void *ui_data;                           /**< UI-specific data.       */

	GHashTable *data;                        /**< Plugin-specific data.   */

	PurpleConnectionFlags features; /**< The supported features */
	GList *message_history;         /**< Message history, as a GList of PurpleConvMessage's */
};

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Conversation API                                                */
/**************************************************************************/
/*@{*/

/**
 * Creates a new conversation of the specified type.
 *
 * @param type    The type of conversation.
 * @param account The account opening the conversation window on the purple
 *                user's end.
 * @param name    The name of the conversation.  For PURPLE_CONV_TYPE_IM,
 *                this is the name of the buddy.
 *
 * @return The new conversation.
 */
PurpleConversation *purple_conversation_new(PurpleConversationType type,
										PurpleAccount *account,
										const char *name);

/**
 * Destroys the specified conversation and removes it from the parent
 * window.
 *
 * If this conversation is the only one contained in the parent window,
 * that window is also destroyed.
 *
 * @param conv The conversation to destroy.
 */
void purple_conversation_destroy(PurpleConversation *conv);


/**
 * Present a conversation to the user. This allows core code to initiate a
 * conversation by displaying the IM dialog.
 * @param conv The conversation to present
 */
void purple_conversation_present(PurpleConversation *conv);


/**
 * Returns the specified conversation's type.
 *
 * @param conv The conversation.
 *
 * @return The conversation's type.
 */
PurpleConversationType purple_conversation_get_type(const PurpleConversation *conv);

/**
 * Sets the specified conversation's UI operations structure.
 *
 * @param conv The conversation.
 * @param ops  The UI conversation operations structure.
 */
void purple_conversation_set_ui_ops(PurpleConversation *conv,
								  PurpleConversationUiOps *ops);

/**
 * Sets the default conversation UI operations structure.
 *
 * @param ops  The UI conversation operations structure.
 */
void purple_conversations_set_ui_ops(PurpleConversationUiOps *ops);

/**
 * Returns the specified conversation's UI operations structure.
 *
 * @param conv The conversation.
 *
 * @return The operations structure.
 */
PurpleConversationUiOps *purple_conversation_get_ui_ops(
		const PurpleConversation *conv);

/**
 * Sets the specified conversation's purple_account.
 *
 * This purple_account represents the user using purple, not the person the user
 * is having a conversation/chat/flame with.
 *
 * @param conv The conversation.
 * @param account The purple_account.
 */
void purple_conversation_set_account(PurpleConversation *conv,
                                   PurpleAccount *account);

/**
 * Returns the specified conversation's purple_account.
 *
 * This purple_account represents the user using purple, not the person the user
 * is having a conversation/chat/flame with.
 *
 * @param conv The conversation.
 *
 * @return The conversation's purple_account.
 */
PurpleAccount *purple_conversation_get_account(const PurpleConversation *conv);

/**
 * Returns the specified conversation's purple_connection.
 *
 * This is the same as purple_conversation_get_user(conv)->gc.
 *
 * @param conv The conversation.
 *
 * @return The conversation's purple_connection.
 */
PurpleConnection *purple_conversation_get_gc(const PurpleConversation *conv);

/**
 * Sets the specified conversation's title.
 *
 * @param conv  The conversation.
 * @param title The title.
 */
void purple_conversation_set_title(PurpleConversation *conv, const char *title);

/**
 * Returns the specified conversation's title.
 *
 * @param conv The conversation.
 *
 * @return The title.
 */
const char *purple_conversation_get_title(const PurpleConversation *conv);

/**
 * Automatically sets the specified conversation's title.
 *
 * This function takes OPT_IM_ALIAS_TAB into account, as well as the
 * user's alias.
 *
 * @param conv The conversation.
 */
void purple_conversation_autoset_title(PurpleConversation *conv);

/**
 * Sets the specified conversation's name.
 *
 * @param conv The conversation.
 * @param name The conversation's name.
 */
void purple_conversation_set_name(PurpleConversation *conv, const char *name);

/**
 * Returns the specified conversation's name.
 *
 * @param conv The conversation.
 *
 * @return The conversation's name. If the conversation is an IM with a PurpleBuddy,
 *         then it's the name of the PurpleBuddy.
 */
const char *purple_conversation_get_name(const PurpleConversation *conv);

/**
 * Get an attribute of a chat buddy
 *
 * @param cb	The chat buddy.
 * @param key	The key of the attribute.
 *
 * @return The value of the attribute key.
 */
const char *purple_conv_chat_cb_get_attribute(PurpleConvChatBuddy *cb, const char *key);

/**
 * Get the keys of all atributes of a chat buddy
 *
 * @param cb	The chat buddy.
 *
 * @return A list of the attributes of a chat buddy.
 */
GList *purple_conv_chat_cb_get_attribute_keys(PurpleConvChatBuddy *cb);
	
/**
 * Set an attribute of a chat buddy
 *
 * @param chat	The chat.
 * @param cb	The chat buddy.
 * @param key	The key of the attribute.
 * @param value	The value of the attribute.
 */
void purple_conv_chat_cb_set_attribute(PurpleConvChat *chat, PurpleConvChatBuddy *cb, const char *key, const char *value);

/**
 * Set attributes of a chat buddy
 *
 * @param chat	The chat.
 * @param cb	The chat buddy.
 * @param keys	A GList of the keys.
 * @param values A GList of the values.
 */
void
purple_conv_chat_cb_set_attributes(PurpleConvChat *chat, PurpleConvChatBuddy *cb, GList *keys, GList *values);

/**
 * Enables or disables logging for this conversation.
 *
 * @param conv The conversation.
 * @param log  @c TRUE if logging should be enabled, or @c FALSE otherwise.
 */
void purple_conversation_set_logging(PurpleConversation *conv, gboolean log);

/**
 * Returns whether or not logging is enabled for this conversation.
 *
 * @param conv The conversation.
 *
 * @return @c TRUE if logging is enabled, or @c FALSE otherwise.
 */
gboolean purple_conversation_is_logging(const PurpleConversation *conv);

/**
 * Closes any open logs for this conversation.
 *
 * Note that new logs will be opened as necessary (e.g. upon receipt of a
 * message, if the conversation has logging enabled. To disable logging for
 * the remainder of the conversation, use purple_conversation_set_logging().
 *
 * @param conv The conversation.
 */
void purple_conversation_close_logs(PurpleConversation *conv);

/**
 * Returns the specified conversation's IM-specific data.
 *
 * If the conversation type is not PURPLE_CONV_TYPE_IM, this will return @c NULL.
 *
 * @param conv The conversation.
 *
 * @return The IM-specific data.
 */
PurpleConvIm *purple_conversation_get_im_data(const PurpleConversation *conv);

#define PURPLE_CONV_IM(c) (purple_conversation_get_im_data(c))

/**
 * Returns the specified conversation's chat-specific data.
 *
 * If the conversation type is not PURPLE_CONV_TYPE_CHAT, this will return @c NULL.
 *
 * @param conv The conversation.
 *
 * @return The chat-specific data.
 */
PurpleConvChat *purple_conversation_get_chat_data(const PurpleConversation *conv);

#define PURPLE_CONV_CHAT(c) (purple_conversation_get_chat_data(c))

/**
 * Sets extra data for a conversation.
 *
 * @param conv The conversation.
 * @param key  The unique key.
 * @param data The data to assign.
 */
void purple_conversation_set_data(PurpleConversation *conv, const char *key,
								gpointer data);

/**
 * Returns extra data in a conversation.
 *
 * @param conv The conversation.
 * @param key  The unqiue key.
 *
 * @return The data associated with the key.
 */
gpointer purple_conversation_get_data(PurpleConversation *conv, const char *key);

/**
 * Returns a list of all conversations.
 *
 * This list includes both IMs and chats.
 *
 * @constreturn A GList of all conversations.
 */
GList *purple_get_conversations(void);

/**
 * Returns a list of all IMs.
 *
 * @constreturn A GList of all IMs.
 */
GList *purple_get_ims(void);

/**
 * Returns a list of all chats.
 *
 * @constreturn A GList of all chats.
 */
GList *purple_get_chats(void);

/**
 * Finds a conversation with the specified type, name, and Purple account.
 *
 * @param type The type of the conversation.
 * @param name The name of the conversation.
 * @param account The purple_account associated with the conversation.
 *
 * @return The conversation if found, or @c NULL otherwise.
 */
PurpleConversation *purple_find_conversation_with_account(
		PurpleConversationType type, const char *name,
		const PurpleAccount *account);

/**
 * Writes to a conversation window.
 *
 * This function should not be used to write IM or chat messages. Use
 * purple_conv_im_write() and purple_conv_chat_write() instead. Those functions will
 * most likely call this anyway, but they may do their own formatting,
 * sound playback, etc.
 *
 * This can be used to write generic messages, such as "so and so closed
 * the conversation window."
 *
 * @param conv    The conversation.
 * @param who     The user who sent the message.
 * @param message The message.
 * @param flags   The message flags.
 * @param mtime   The time the message was sent.
 *
 * @see purple_conv_im_write()
 * @see purple_conv_chat_write()
 */
void purple_conversation_write(PurpleConversation *conv, const char *who,
		const char *message, PurpleMessageFlags flags,
		time_t mtime);

/**
	Set the features as supported for the given conversation.
	@param conv      The conversation
	@param features  Bitset defining supported features
*/
void purple_conversation_set_features(PurpleConversation *conv,
		PurpleConnectionFlags features);


/**
	Get the features supported by the given conversation.
	@param conv  The conversation
*/
PurpleConnectionFlags purple_conversation_get_features(PurpleConversation *conv);

/**
 * Determines if a conversation has focus
 *
 * @param conv    The conversation.
 *
 * @return @c TRUE if the conversation has focus, @c FALSE if
 * it does not or the UI does not have a concept of conversation focus
 */
gboolean purple_conversation_has_focus(PurpleConversation *conv);

/**
 * Updates the visual status and UI of a conversation.
 *
 * @param conv The conversation.
 * @param type The update type.
 */
void purple_conversation_update(PurpleConversation *conv, PurpleConvUpdateType type);

/**
 * Calls a function on each conversation.
 *
 * @param func The function.
 */
void purple_conversation_foreach(void (*func)(PurpleConversation *conv));

/**
 * Retrieve the message history of a conversation.
 *
 * @param conv   The conversation
 *
 * @return  A GList of PurpleConvMessage's. The must not modify the list or the data within.
 *          The list contains the newest message at the beginning, and the oldest message at
 *          the end.
 *
 * @since 2.2.0
 */
GList *purple_conversation_get_message_history(PurpleConversation *conv);

/**
 * Clear the message history of a conversation.
 *
 * @param conv  The conversation
 *
 * @since 2.2.0
 */
void purple_conversation_clear_message_history(PurpleConversation *conv);

/**
 * Get the sender from a PurpleConvMessage
 *
 * @param msg   A PurpleConvMessage
 *
 * @return   The name of the sender of the message
 *
 * @since 2.2.0
 */
const char *purple_conversation_message_get_sender(PurpleConvMessage *msg);

/**
 * Get the message from a PurpleConvMessage
 *
 * @param msg   A PurpleConvMessage
 *
 * @return   The name of the sender of the message
 *
 * @since 2.2.0
 */
const char *purple_conversation_message_get_message(PurpleConvMessage *msg);

/**
 * Get the message-flags of a PurpleConvMessage
 *
 * @param msg   A PurpleConvMessage
 *
 * @return   The message flags
 *
 * @since 2.2.0
 */
PurpleMessageFlags purple_conversation_message_get_flags(PurpleConvMessage *msg);

/**
 * Get the timestamp of a PurpleConvMessage
 *
 * @param msg   A PurpleConvMessage
 *
 * @return   The timestamp of the message
 *
 * @since 2.2.0
 */
time_t purple_conversation_message_get_timestamp(PurpleConvMessage *msg);

/*@}*/


/**************************************************************************/
/** @name IM Conversation API                                             */
/**************************************************************************/
/*@{*/

/**
 * Gets an IM's parent conversation.
 *
 * @param im The IM.
 *
 * @return The parent conversation.
 */
PurpleConversation *purple_conv_im_get_conversation(const PurpleConvIm *im);

/**
 * Sets the IM's buddy icon.
 *
 * This should only be called from within Purple. You probably want to
 * call purple_buddy_icon_set_data().
 *
 * @param im   The IM.
 * @param icon The buddy icon.
 *
 * @see purple_buddy_icon_set_data()
 */
void purple_conv_im_set_icon(PurpleConvIm *im, PurpleBuddyIcon *icon);

/**
 * Returns the IM's buddy icon.
 *
 * @param im The IM.
 *
 * @return The buddy icon.
 */
PurpleBuddyIcon *purple_conv_im_get_icon(const PurpleConvIm *im);

/**
 * Sets the IM's typing state.
 *
 * @param im    The IM.
 * @param state The typing state.
 */
void purple_conv_im_set_typing_state(PurpleConvIm *im, PurpleTypingState state);

/**
 * Returns the IM's typing state.
 *
 * @param im The IM.
 *
 * @return The IM's typing state.
 */
PurpleTypingState purple_conv_im_get_typing_state(const PurpleConvIm *im);

/**
 * Starts the IM's typing timeout.
 *
 * @param im      The IM.
 * @param timeout The timeout.
 */
void purple_conv_im_start_typing_timeout(PurpleConvIm *im, int timeout);

/**
 * Stops the IM's typing timeout.
 *
 * @param im The IM.
 */
void purple_conv_im_stop_typing_timeout(PurpleConvIm *im);

/**
 * Returns the IM's typing timeout.
 *
 * @param im The IM.
 *
 * @return The timeout.
 */
guint purple_conv_im_get_typing_timeout(const PurpleConvIm *im);

/**
 * Sets the quiet-time when no PURPLE_TYPING messages will be sent.
 * Few protocols need this (maybe only MSN).  If the user is still
 * typing after this quiet-period, then another PURPLE_TYPING message
 * will be sent.
 *
 * @param im  The IM.
 * @param val The number of seconds to wait before allowing another
 *            PURPLE_TYPING message to be sent to the user.  Or 0 to
 *            not send another PURPLE_TYPING message.
 */
void purple_conv_im_set_type_again(PurpleConvIm *im, unsigned int val);

/**
 * Returns the time after which another PURPLE_TYPING message should be sent.
 *
 * @param im The IM.
 *
 * @return The time in seconds since the epoch.  Or 0 if no additional
 *         PURPLE_TYPING message should be sent.
 */
time_t purple_conv_im_get_type_again(const PurpleConvIm *im);

/**
 * Starts the IM's type again timeout.
 *
 * @param im      The IM.
 */
void purple_conv_im_start_send_typed_timeout(PurpleConvIm *im);

/**
 * Stops the IM's type again timeout.
 *
 * @param im The IM.
 */
void purple_conv_im_stop_send_typed_timeout(PurpleConvIm *im);

/**
 * Returns the IM's type again timeout interval.
 *
 * @param im The IM.
 *
 * @return The type again timeout interval.
 */
guint purple_conv_im_get_send_typed_timeout(const PurpleConvIm *im);

/**
 * Updates the visual typing notification for an IM conversation.
 *
 * @param im The IM.
 */
void purple_conv_im_update_typing(PurpleConvIm *im);

/**
 * Writes to an IM.
 *
 * @param im      The IM.
 * @param who     The user who sent the message.
 * @param message The message to write.
 * @param flags   The message flags.
 * @param mtime   The time the message was sent.
 */
void purple_conv_im_write(PurpleConvIm *im, const char *who,
						const char *message, PurpleMessageFlags flags,
						time_t mtime);

/**
 * Presents an IM-error to the user
 *
 * This is a helper function to find a conversation, write an error to it, and
 * raise the window.  If a conversation with this user doesn't already exist,
 * the function will return FALSE and the calling function can attempt to present
 * the error another way (purple_notify_error, most likely)
 *
 * @param who     The user this error is about
 * @param account The account this error is on
 * @param what    The error
 * @return        TRUE if the error was presented, else FALSE
 */
gboolean purple_conv_present_error(const char *who, PurpleAccount *account, const char *what);

/**
 * Sends a message to this IM conversation.
 *
 * @param im      The IM.
 * @param message The message to send.
 */
void purple_conv_im_send(PurpleConvIm *im, const char *message);

/**
 * Sends a message to a conversation after confirming with
 * the user.
 *
 * This function is intended for use in cases where the user
 * hasn't explicitly and knowingly caused a message to be sent.
 * The confirmation ensures that the user isn't sending a
 * message by mistake.
 *
 * @param conv    The conversation.
 * @param message The message to send.
 */
void purple_conv_send_confirm(PurpleConversation *conv, const char *message);

/**
 * Sends a message to this IM conversation with specified flags.
 *
 * @param im      The IM.
 * @param message The message to send.
 * @param flags   The PurpleMessageFlags flags to use in addition to PURPLE_MESSAGE_SEND.
 */
void purple_conv_im_send_with_flags(PurpleConvIm *im, const char *message, PurpleMessageFlags flags);

/**
 * Adds a smiley to the conversation's smiley tree. If this returns
 * @c TRUE you should call purple_conv_custom_smiley_write() one or more
 * times, and then purple_conv_custom_smiley_close(). If this returns
 * @c FALSE, either the conv or smile were invalid, or the icon was
 * found in the cache. In either case, calling write or close would
 * be an error.
 *
 * @param conv The conversation to associate the smiley with.
 * @param smile The text associated with the smiley
 * @param cksum_type The type of checksum.
 * @param chksum The checksum, as a NUL terminated base64 string.
 * @param remote @c TRUE if the custom smiley is set by the remote user (buddy).
 * @return      @c TRUE if an icon is expected, else FALSE. Note that
 *              it is an error to never call purple_conv_custom_smiley_close if
 *              this function returns @c TRUE, but an error to call it if
 *              @c FALSE is returned.
 */

gboolean purple_conv_custom_smiley_add(PurpleConversation *conv, const char *smile,
                                      const char *cksum_type, const char *chksum,
									  gboolean remote);


/**
 * Updates the image associated with the current smiley.
 *
 * @param conv The conversation associated with the smiley.
 * @param smile The text associated with the smiley.
 * @param data The actual image data.
 * @param size The length of the data.
 */

void purple_conv_custom_smiley_write(PurpleConversation *conv,
                                   const char *smile,
                                   const guchar *data,
                                   gsize size);

/**
 * Close the custom smiley, all data has been written with
 * purple_conv_custom_smiley_write, and it is no longer valid
 * to call that function on that smiley.
 *
 * @param conv The purple conversation associated with the smiley.
 * @param smile The text associated with the smiley
 */

void purple_conv_custom_smiley_close(PurpleConversation *conv, const char *smile);

/*@}*/


/**************************************************************************/
/** @name Chat Conversation API                                           */
/**************************************************************************/
/*@{*/

/**
 * Gets a chat's parent conversation.
 *
 * @param chat The chat.
 *
 * @return The parent conversation.
 */
PurpleConversation *purple_conv_chat_get_conversation(const PurpleConvChat *chat);

/**
 * Sets the list of users in the chat room.
 *
 * @note Calling this function will not update the display of the users.
 *       Please use purple_conv_chat_add_user(), purple_conv_chat_add_users(),
 *       purple_conv_chat_remove_user(), and purple_conv_chat_remove_users() instead.
 *
 * @param chat  The chat.
 * @param users The list of users.
 *
 * @return The list passed.
 *
 * @deprecated This function will be removed in 3.0.0.  You shouldn't be using it anyway.
 */
GList *purple_conv_chat_set_users(PurpleConvChat *chat, GList *users);

/**
 * Returns a list of users in the chat room.  The members of the list
 * are PurpleConvChatBuddy objects.
 *
 * @param chat The chat.
 *
 * @constreturn The list of users.
 */
GList *purple_conv_chat_get_users(const PurpleConvChat *chat);

/**
 * Ignores a user in a chat room.
 *
 * @param chat The chat.
 * @param name The name of the user.
 */
void purple_conv_chat_ignore(PurpleConvChat *chat, const char *name);

/**
 * Unignores a user in a chat room.
 *
 * @param chat The chat.
 * @param name The name of the user.
 */
void purple_conv_chat_unignore(PurpleConvChat *chat, const char *name);

/**
 * Sets the list of ignored users in the chat room.
 *
 * @param chat    The chat.
 * @param ignored The list of ignored users.
 *
 * @return The list passed.
 */
GList *purple_conv_chat_set_ignored(PurpleConvChat *chat, GList *ignored);

/**
 * Returns the list of ignored users in the chat room.
 *
 * @param chat The chat.
 *
 * @constreturn The list of ignored users.
 */
GList *purple_conv_chat_get_ignored(const PurpleConvChat *chat);

/**
 * Returns the actual name of the specified ignored user, if it exists in
 * the ignore list.
 *
 * If the user found contains a prefix, such as '+' or '\@', this is also
 * returned. The username passed to the function does not have to have this
 * formatting.
 *
 * @param chat The chat.
 * @param user The user to check in the ignore list.
 *
 * @return The ignored user if found, complete with prefixes, or @c NULL
 *         if not found.
 */
const char *purple_conv_chat_get_ignored_user(const PurpleConvChat *chat,
											const char *user);

/**
 * Returns @c TRUE if the specified user is ignored.
 *
 * @param chat The chat.
 * @param user The user.
 *
 * @return @c TRUE if the user is in the ignore list; @c FALSE otherwise.
 */
gboolean purple_conv_chat_is_user_ignored(const PurpleConvChat *chat,
										const char *user);

/**
 * Sets the chat room's topic.
 *
 * @param chat  The chat.
 * @param who   The user that set the topic.
 * @param topic The topic.
 */
void purple_conv_chat_set_topic(PurpleConvChat *chat, const char *who,
							  const char *topic);

/**
 * Returns the chat room's topic.
 *
 * @param chat The chat.
 *
 * @return The chat's topic.
 */
const char *purple_conv_chat_get_topic(const PurpleConvChat *chat);

/**
 * Sets the chat room's ID.
 *
 * @param chat The chat.
 * @param id   The ID.
 */
void purple_conv_chat_set_id(PurpleConvChat *chat, int id);

/**
 * Returns the chat room's ID.
 *
 * @param chat The chat.
 *
 * @return The ID.
 */
int purple_conv_chat_get_id(const PurpleConvChat *chat);

/**
 * Writes to a chat.
 *
 * @param chat    The chat.
 * @param who     The user who sent the message.
 * @param message The message to write.
 * @param flags   The flags.
 * @param mtime   The time the message was sent.
 */
void purple_conv_chat_write(PurpleConvChat *chat, const char *who,
						  const char *message, PurpleMessageFlags flags,
						  time_t mtime);

/**
 * Sends a message to this chat conversation.
 *
 * @param chat    The chat.
 * @param message The message to send.
 */
void purple_conv_chat_send(PurpleConvChat *chat, const char *message);

/**
 * Sends a message to this chat conversation with specified flags.
 *
 * @param chat    The chat.
 * @param message The message to send.
 * @param flags   The PurpleMessageFlags flags to use.
 */
void purple_conv_chat_send_with_flags(PurpleConvChat *chat, const char *message, PurpleMessageFlags flags);

/**
 * Adds a user to a chat.
 *
 * @param chat        The chat.
 * @param user        The user to add.
 * @param extra_msg   An extra message to display with the join message.
 * @param flags       The users flags
 * @param new_arrival Decides whether or not to show a join notice.
 */
void purple_conv_chat_add_user(PurpleConvChat *chat, const char *user,
							 const char *extra_msg, PurpleConvChatBuddyFlags flags,
							 gboolean new_arrival);

/**
 * Adds a list of users to a chat.
 *
 * The data is copied from @a users, @a extra_msgs, and @a flags, so it is up to
 * the caller to free this list after calling this function.
 *
 * @param chat         The chat.
 * @param users        The list of users to add.
 * @param extra_msgs   An extra message to display with the join message for each
 *                     user.  This list may be shorter than @a users, in which
 *                     case, the users after the end of extra_msgs will not have
 *                     an extra message.  By extension, this means that extra_msgs
 *                     can simply be @c NULL and none of the users will have an
 *                     extra message.
 * @param flags        The list of flags for each user.
 * @param new_arrivals Decides whether or not to show join notices.
 */
void purple_conv_chat_add_users(PurpleConvChat *chat, GList *users, GList *extra_msgs,
							  GList *flags, gboolean new_arrivals);

/**
 * Renames a user in a chat.
 *
 * @param chat     The chat.
 * @param old_user The old username.
 * @param new_user The new username.
 */
void purple_conv_chat_rename_user(PurpleConvChat *chat, const char *old_user,
								const char *new_user);

/**
 * Removes a user from a chat, optionally with a reason.
 *
 * It is up to the developer to free this list after calling this function.
 *
 * @param chat   The chat.
 * @param user   The user that is being removed.
 * @param reason The optional reason given for the removal. Can be @c NULL.
 */
void purple_conv_chat_remove_user(PurpleConvChat *chat, const char *user,
								const char *reason);

/**
 * Removes a list of users from a chat, optionally with a single reason.
 *
 * @param chat   The chat.
 * @param users  The users that are being removed.
 * @param reason The optional reason given for the removal. Can be @c NULL.
 */
void purple_conv_chat_remove_users(PurpleConvChat *chat, GList *users,
								 const char *reason);

/**
 * Finds a user in a chat
 *
 * @param chat   The chat.
 * @param user   The user to look for.
 *
 * @return TRUE if the user is in the chat, FALSE if not
 */
gboolean purple_conv_chat_find_user(PurpleConvChat *chat, const char *user);

/**
 * Set a users flags in a chat
 *
 * @param chat   The chat.
 * @param user   The user to update.
 * @param flags  The new flags.
 */
void purple_conv_chat_user_set_flags(PurpleConvChat *chat, const char *user,
								   PurpleConvChatBuddyFlags flags);

/**
 * Get the flags for a user in a chat
 *
 * @param chat   The chat.
 * @param user   The user to find the flags for
 *
 * @return The flags for the user
 */
PurpleConvChatBuddyFlags purple_conv_chat_user_get_flags(PurpleConvChat *chat,
													 const char *user);

/**
 * Clears all users from a chat.
 *
 * @param chat The chat.
 */
void purple_conv_chat_clear_users(PurpleConvChat *chat);

/**
 * Sets your nickname (used for hilighting) for a chat.
 *
 * @param chat The chat.
 * @param nick The nick.
 */
void purple_conv_chat_set_nick(PurpleConvChat *chat, const char *nick);

/**
 * Gets your nickname (used for hilighting) for a chat.
 *
 * @param chat The chat.
 * @return  The nick.
 */
const char *purple_conv_chat_get_nick(PurpleConvChat *chat);

/**
 * Finds a chat with the specified chat ID.
 *
 * @param gc The purple_connection.
 * @param id The chat ID.
 *
 * @return The chat conversation.
 */
PurpleConversation *purple_find_chat(const PurpleConnection *gc, int id);

/**
 * Lets the core know we left a chat, without destroying it.
 * Called from serv_got_chat_left().
 *
 * @param chat The chat.
 */
void purple_conv_chat_left(PurpleConvChat *chat);

/**
 * Invite a user to a chat.
 * The user will be prompted to enter the user's name or a message if one is
 * not given.
 *
 * @param chat     The chat.
 * @param user     The user to invite to the chat.
 * @param message  The message to send with the invitation.
 * @param confirm  Prompt before sending the invitation. The user is always
 *                 prompted if either \a user or \a message is @c NULL.
 *
 * @since 2.6.0
 */
void purple_conv_chat_invite_user(PurpleConvChat *chat, const char *user,
		const char *message, gboolean confirm);

/**
 * Returns true if we're no longer in this chat,
 * and just left the window open.
 *
 * @param chat The chat.
 *
 * @return @c TRUE if we left the chat already, @c FALSE if
 * we're still there.
 */
gboolean purple_conv_chat_has_left(PurpleConvChat *chat);

/**
 * Creates a new chat buddy
 *
 * @param name The name.
 * @param alias The alias.
 * @param flags The flags.
 *
 * @return The new chat buddy
 */
PurpleConvChatBuddy *purple_conv_chat_cb_new(const char *name, const char *alias,
										PurpleConvChatBuddyFlags flags);

/**
 * Find a chat buddy in a chat
 *
 * @param chat The chat.
 * @param name The name of the chat buddy to find.
 */
PurpleConvChatBuddy *purple_conv_chat_cb_find(PurpleConvChat *chat, const char *name);

/**
 * Get the name of a chat buddy
 *
 * @param cb    The chat buddy.
 *
 * @return The name of the chat buddy.
 */
const char *purple_conv_chat_cb_get_name(PurpleConvChatBuddy *cb);

/**
 * Destroys a chat buddy
 *
 * @param cb The chat buddy to destroy
 */
void purple_conv_chat_cb_destroy(PurpleConvChatBuddy *cb);

/**
 * Retrieves the extended menu items for the conversation.
 *
 * @param conv The conversation.
 *
 * @return  A list of PurpleMenuAction items, harvested by the
 *          chat-extended-menu signal. The list and the menuaction
 *          items should be freed by the caller.
 *
 * @since 2.1.0
 */
GList * purple_conversation_get_extended_menu(PurpleConversation *conv);

/**
 * Perform a command in a conversation. Similar to @see purple_cmd_do_command
 *
 * @param conv    The conversation.
 * @param cmdline The entire command including the arguments.
 * @param markup  @c NULL, or the formatted command line.
 * @param error   If the command failed errormsg is filled in with the appropriate error
 *                message, if not @c NULL. It must be freed by the caller with g_free().
 *
 * @return  @c TRUE if the command was executed successfully, @c FALSE otherwise.
 *
 * @since 2.1.0
 */
gboolean purple_conversation_do_command(PurpleConversation *conv, const gchar *cmdline, const gchar *markup, gchar **error);

/*@}*/

/**************************************************************************/
/** @name Conversations Subsystem                                         */
/**************************************************************************/
/*@{*/

/**
 * Returns the conversation subsystem handle.
 *
 * @return The conversation subsystem handle.
 */
void *purple_conversations_get_handle(void);

/**
 * Initializes the conversation subsystem.
 */
void purple_conversations_init(void);

/**
 * Uninitializes the conversation subsystem.
 */
void purple_conversations_uninit(void);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_CONVERSATION_H_ */
