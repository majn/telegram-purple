/**
 * @file server.h Server API
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
#ifndef _PURPLE_SERVER_H_
#define _PURPLE_SERVER_H_

#include "account.h"
#include "conversation.h"
#include "prpl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Send a typing message to a given user over a given connection.
 *
 * TODO: Could probably move this into the conversation API.
 *
 * @param gc    The connection over which to send the typing notification.
 * @param name  The user to send the typing notification to.
 * @param state One of PURPLE_TYPING, PURPLE_TYPED, or PURPLE_NOT_TYPING.
 * @return A quiet-period, specified in seconds, where Purple will not
 *         send any additional typing notification messages.  Most
 *         protocols should return 0, which means that no additional
 *         PURPLE_TYPING messages need to be sent.  If this is 5, for
 *         example, then Purple will wait five seconds, and if the Purple
 *         user is still typing then Purple will send another PURPLE_TYPING
 *         message.
 */
unsigned int serv_send_typing(PurpleConnection *gc, const char *name, PurpleTypingState state);

void serv_move_buddy(PurpleBuddy *, PurpleGroup *, PurpleGroup *);
int  serv_send_im(PurpleConnection *, const char *, const char *, PurpleMessageFlags flags);

/** Get information about an account's attention commands, from the prpl.
 *
 * @return The attention command numbered 'code' from the prpl's attention_types, or NULL.
 */
PurpleAttentionType *purple_get_attention_type_from_code(PurpleAccount *account, guint type_code);

/** Send an attention request message.
 *
 * @deprecated Use purple_prpl_send_attention() instead.
 *
 * @param gc The connection to send the message on.
 * @param who Whose attention to request.
 * @param type_code An index into the prpl's attention_types list determining the type
 * 	of the attention request command to send. 0 if prpl only defines one
 * 	(for example, Yahoo and MSN), but some protocols define more (MySpaceIM).
 *
 * Note that you can't send arbitrary PurpleAttentionType's, because there is
 * only a fixed set of attention commands.
 */
void serv_send_attention(PurpleConnection *gc, const char *who, guint type_code);

/** Process an incoming attention message.
 *
 * @deprecated Use purple_prpl_got_attention() instead.
 *
 * @param gc The connection that received the attention message.
 * @param who Who requested your attention.
 * @param type_code An index into the prpl's attention_types list determining the type
 * 	of the attention request command to send.
 */
void serv_got_attention(PurpleConnection *gc, const char *who, guint type_code);

void serv_get_info(PurpleConnection *, const char *);
void serv_set_info(PurpleConnection *, const char *);

void serv_add_permit(PurpleConnection *, const char *);
void serv_add_deny(PurpleConnection *, const char *);
void serv_rem_permit(PurpleConnection *, const char *);
void serv_rem_deny(PurpleConnection *, const char *);
void serv_set_permit_deny(PurpleConnection *);
void serv_chat_invite(PurpleConnection *, int, const char *, const char *);
void serv_chat_leave(PurpleConnection *, int);
void serv_chat_whisper(PurpleConnection *, int, const char *, const char *);
int  serv_chat_send(PurpleConnection *, int, const char *, PurpleMessageFlags flags);
void serv_alias_buddy(PurpleBuddy *);
void serv_got_alias(PurpleConnection *gc, const char *who, const char *alias);

/**
 * A protocol plugin should call this when it retrieves a private alias from
 * the server.  Private aliases are the aliases the user sets, while public
 * aliases are the aliases or display names that buddies set for themselves.
 *
 * @param gc The connection on which the alias was received.
 * @param who The name of the buddy whose alias was received.
 * @param alias The alias that was received.
 */
void purple_serv_got_private_alias(PurpleConnection *gc, const char *who, const char *alias);


/**
 * Receive a typing message from a remote user.  Either PURPLE_TYPING
 * or PURPLE_TYPED.  If the user has stopped typing then use
 * serv_got_typing_stopped instead.
 *
 * TODO: Could probably move this into the conversation API.
 *
 * @param gc      The connection on which the typing message was received.
 * @param name    The name of the remote user.
 * @param timeout If this is a number greater than 0, then
 *        Purple will wait this number of seconds and then
 *        set this buddy to the PURPLE_NOT_TYPING state.  This
 *        is used by protocols that send repeated typing messages
 *        while the user is composing the message.
 * @param state   The typing state received
 */
void serv_got_typing(PurpleConnection *gc, const char *name, int timeout,
					 PurpleTypingState state);

/**
 * TODO: Could probably move this into the conversation API.
 */
void serv_got_typing_stopped(PurpleConnection *gc, const char *name);

void serv_got_im(PurpleConnection *gc, const char *who, const char *msg,
				 PurpleMessageFlags flags, time_t mtime);

/**
 * @param data The hash function should be g_str_hash() and the equal
 *             function should be g_str_equal().
 */
void serv_join_chat(PurpleConnection *, GHashTable *data);

/**
 * @param data The hash function should be g_str_hash() and the equal
 *             function should be g_str_equal().
 */
void serv_reject_chat(PurpleConnection *, GHashTable *data);

/**
 * Called by a prpl when an account is invited into a chat.
 *
 * @param gc      The connection on which the invite arrived.
 * @param name    The name of the chat you're being invited to.
 * @param who     The username of the person inviting the account.
 * @param message The optional invite message.
 * @param data    The components necessary if you want to call serv_join_chat().
 *                The hash function should be g_str_hash() and the equal
 *                function should be g_str_equal().
 */
void serv_got_chat_invite(PurpleConnection *gc, const char *name,
						  const char *who, const char *message,
						  GHashTable *data);

/**
 * Called by a prpl when an account has joined a chat.
 *
 * @param gc   The connection on which the chat was joined.
 * @param id   The id of the chat, assigned by the prpl.
 * @param name The name of the chat.
 * @return     The resulting conversation
 */
PurpleConversation *serv_got_joined_chat(PurpleConnection *gc,
									   int id, const char *name);
/**
 * Called by a prpl when an attempt to join a chat via serv_join_chat()
 * fails.
 *
 * @param gc      The connection on which chat joining failed
 * @param data    The components passed to serv_join_chat() originally.
 *                The hash function should be g_str_hash() and the equal
 *                function should be g_str_equal().
 */
void purple_serv_got_join_chat_failed(PurpleConnection *gc, GHashTable *data);

/**
 * Called by a prpl when an account has left a chat.
 *
 * @param g  The connection on which the chat was left.
 * @param id The id of the chat, as assigned by the prpl.
 */
void serv_got_chat_left(PurpleConnection *g, int id);

/**
 * Called by a prpl when a message has been received in a chat.
 *
 * @param g       The connection on which the message was received.
 * @param id      The id of the chat, as assigned by the prpl.
 * @param who     The name of the user who sent the message.
 * @param flags   The flags of the message.
 * @param message The message received in the chat.
 * @param mtime   The time when the message was received.
 */
void serv_got_chat_in(PurpleConnection *g, int id, const char *who,
					  PurpleMessageFlags flags, const char *message, time_t mtime);
void serv_send_file(PurpleConnection *gc, const char *who, const char *file);

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_SERVER_H_ */
