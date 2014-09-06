/**
 * @file cmds.h Commands API
 * @ingroup core
 * @see @ref cmd-signals
 */

/* Copyright (C) 2003 Timothy Ringenbach <omarvo@hotmail.com>
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
 *
 */
#ifndef _PURPLE_CMDS_H_
#define _PURPLE_CMDS_H_

#include "conversation.h"

/**************************************************************************/
/** @name Structures                                                      */
/**************************************************************************/
/*@{*/

/** The possible results of running a command with purple_cmd_do_command(). */
typedef enum _PurpleCmdStatus {
	PURPLE_CMD_STATUS_OK,
	PURPLE_CMD_STATUS_FAILED,
	PURPLE_CMD_STATUS_NOT_FOUND,
	PURPLE_CMD_STATUS_WRONG_ARGS,
	PURPLE_CMD_STATUS_WRONG_PRPL,
	PURPLE_CMD_STATUS_WRONG_TYPE
} PurpleCmdStatus;

/** Commands registered with the core return one of these values when run.
 *  Normally, a command will want to return one of the first two; in some
 *  unusual cases, you might want to have several functions called for a
 *  particular command; in this case, they should return
 *  #PURPLE_CMD_RET_CONTINUE to cause the core to fall through to other
 *  commands with the same name.
 */
typedef enum _PurpleCmdRet {
	PURPLE_CMD_RET_OK,       /**< Everything's okay; Don't look for another command to call. */
	PURPLE_CMD_RET_FAILED,   /**< The command failed, but stop looking.*/
	PURPLE_CMD_RET_CONTINUE /**< Continue, looking for other commands with the same name to call. */
} PurpleCmdRet;

#define PURPLE_CMD_FUNC(func) ((PurpleCmdFunc)func)

/** A function implementing a command, as passed to purple_cmd_register().
 *
 *  @todo document the arguments to these functions.
 * */
typedef PurpleCmdRet (*PurpleCmdFunc)(PurpleConversation *, const gchar *cmd,
                                  gchar **args, gchar **error, void *data);
/** A unique integer representing a command registered with
 *  purple_cmd_register(), which can subsequently be passed to
 *  purple_cmd_unregister() to unregister that command.
 */
typedef guint PurpleCmdId;

typedef enum _PurpleCmdPriority {
	PURPLE_CMD_P_VERY_LOW  = -1000,
	PURPLE_CMD_P_LOW       =     0,
	PURPLE_CMD_P_DEFAULT   =  1000,
	PURPLE_CMD_P_PRPL      =  2000,
	PURPLE_CMD_P_PLUGIN    =  3000,
	PURPLE_CMD_P_ALIAS     =  4000,
	PURPLE_CMD_P_HIGH      =  5000,
	PURPLE_CMD_P_VERY_HIGH =  6000
} PurpleCmdPriority;

/** Flags used to set various properties of commands.  Every command should
 *  have at least one of #PURPLE_CMD_FLAG_IM and #PURPLE_CMD_FLAG_CHAT set in
 *  order to be even slighly useful.
 *
 *  @see purple_cmd_register
 */
typedef enum _PurpleCmdFlag {
	/** Command is usable in IMs. */
	PURPLE_CMD_FLAG_IM               = 0x01,
	/** Command is usable in multi-user chats. */
	PURPLE_CMD_FLAG_CHAT             = 0x02,
	/** Command is usable only for a particular prpl. */
	PURPLE_CMD_FLAG_PRPL_ONLY        = 0x04,
	/** Incorrect arguments to this command should be accepted anyway. */
	PURPLE_CMD_FLAG_ALLOW_WRONG_ARGS = 0x08
} PurpleCmdFlag;


/*@}*/

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Commands API                                                    */
/**************************************************************************/
/*@{*/

/**
 * Register a new command with the core.
 *
 * The command will only happen if commands are enabled,
 * which is a UI pref. UIs don't have to support commands at all.
 *
 * @param cmd The command. This should be a UTF-8 (or ASCII) string, with no spaces
 *            or other white space.
 * @param args A string of characters describing to libpurple how to parse this
 *             command's arguments.  If what the user types doesn't match this
 *             pattern, libpurple will keep looking for another command, unless
 *             the flag #PURPLE_CMD_FLAG_ALLOW_WRONG_ARGS is passed in @a f.
 *             This string should contain no whitespace, and use a single
 *             character for each argument.  The recognized characters are:
 *             <ul>
 *               <li><tt>'w'</tt>: Matches a single word.</li>
 *               <li><tt>'W'</tt>: Matches a single word, with formatting.</li>
 *               <li><tt>'s'</tt>: Matches the rest of the arguments after this
 *                                 point, as a single string.</li>
 *               <li><tt>'S'</tt>: Same as <tt>'s'</tt> but with formatting.</li>
 *             </ul>
 *             If args is the empty string, then the command accepts no arguments.
 *             The args passed to the callback @a func will be a @c NULL
 *             terminated array of @c NULL terminated strings, and will always
 *             match the number of arguments asked for, unless
 *             #PURPLE_CMD_FLAG_ALLOW_WRONG_ARGS is passed.
 * @param p This is the priority. Higher priority commands will be run first,
 *          and usually the first command will stop any others from being
 *          called.
 * @param f Flags specifying various options about this command, combined with
 *          <tt>|</tt> (bitwise OR). You need to at least pass one of
 *          #PURPLE_CMD_FLAG_IM or #PURPLE_CMD_FLAG_CHAT (you may pass both) in
 *          order for the command to ever actually be called.
 * @param prpl_id If the #PURPLE_CMD_FLAG_PRPL_ONLY flag is set, this is the id
 *                of the prpl to which the command applies (such as
 *                <tt>"prpl-msn"</tt>). If the flag is not set, this parameter
 *                is ignored; pass @c NULL (or a humourous string of your
 *                choice!).
 * @param func This is the function to call when someone enters this command.
 * @param helpstr a whitespace sensitive, UTF-8, HTML string describing how to
 *                use the command.  The preferred format of this string is the
 *                command's name, followed by a space and any arguments it
 *                accepts (if it takes any arguments, otherwise no space),
 *                followed by a colon, two spaces, and a description of the
 *                command in sentence form.  Do not include a slash before the
 *                command name.
 * @param data User defined data to pass to the #PurpleCmdFunc @a f.
 * @return A #PurpleCmdId, which is only used for calling
 *         #purple_cmd_unregister, or @a 0 on failure.
 */
PurpleCmdId purple_cmd_register(const gchar *cmd, const gchar *args, PurpleCmdPriority p, PurpleCmdFlag f,
                             const gchar *prpl_id, PurpleCmdFunc func, const gchar *helpstr, void *data);

/**
 * Unregister a command with the core.
 *
 * All registered commands must be unregistered, if they're registered by a plugin
 * or something else that might go away. Normally this is called when the plugin
 * unloads itself.
 *
 * @param id The #PurpleCmdId to unregister, as returned by #purple_cmd_register.
 */
void purple_cmd_unregister(PurpleCmdId id);

/**
 * Do a command.
 *
 * Normally the UI calls this to perform a command. This might also be useful
 * if aliases are ever implemented.
 *
 * @param conv The conversation the command was typed in.
 * @param cmdline The command the user typed (including all arguments) as a single string.
 *            The caller doesn't have to do any parsing, except removing the command
 *            prefix, which the core has no knowledge of. cmd should not contain any
 *            formatting, and should be in plain text (no html entities).
 * @param markup This is the same as cmd, but is the formatted version. It should be in
 *               HTML, with < > and &, at least, escaped to html entities, and should
 *               include both the default formatting and any extra manual formatting.
 * @param errormsg If the command failed errormsg is filled in with the appropriate error
 *                 message. It must be freed by the caller with g_free().
 * @return A #PurpleCmdStatus indicating if the command succeeded or failed.
 */
PurpleCmdStatus purple_cmd_do_command(PurpleConversation *conv, const gchar *cmdline,
                                  const gchar *markup, gchar **errormsg);

/**
 * List registered commands.
 *
 * Returns a <tt>GList</tt> (which must be freed by the caller) of all commands
 * that are valid in the context of @a conv, or all commands, if @a conv is @c
 * NULL.  Don't keep this list around past the main loop, or anything else that
 * might unregister a command, as the <tt>const char *</tt>'s used get freed
 * then.
 *
 * @param conv The conversation, or @c NULL.
 * @return A @c GList of <tt>const char *</tt>, which must be freed with
 *         <tt>g_list_free()</tt>.
 */
GList *purple_cmd_list(PurpleConversation *conv);

/**
 * Get the help string for a command.
 *
 * Returns the help strings for a given command in the form of a GList,
 * one node for each matching command.
 *
 * @param conv The conversation, or @c NULL for no context.
 * @param cmd The command. No wildcards accepted, but returns help for all
 *            commands if @c NULL.
 * @return A <tt>GList</tt> of <tt>const char *</tt>s, which is the help string
 *         for that command.
 */
GList *purple_cmd_help(PurpleConversation *conv, const gchar *cmd);

/**
 * Get the handle for the commands API
 * @return The handle
 * @since 2.5.0
 */
gpointer purple_cmds_get_handle(void);

/**
 * Initialize the commands subsystem.
 * @since 2.5.0
 */
void purple_cmds_init(void);

/**
 * Uninitialize the commands subsystem.
 * @since 2.5.0
 */
void purple_cmds_uninit(void);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_CMDS_H_ */
