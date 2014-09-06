/**
 * @file table.h MSN helper structure
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
#ifndef MSN_TABLE_H
#define MSN_TABLE_H

typedef struct _MsnTable MsnTable;

#include "cmdproc.h"
#include "transaction.h"
#include "msg.h"

typedef void (*MsnMsgTypeCb)(MsnCmdProc *cmdproc, MsnMessage *msg);

struct _MsnTable
{
	GHashTable *cmds; 		/**< Callbacks that manage command response. */
	GHashTable *msgs; 		/**< Callbacks that manage incoming messages. */
	GHashTable *errors; 	/**< Callbacks that manage command errors. */

	GHashTable *async; 		/**< Callbacks that manage incoming asyncronous messages. */
	/* TODO: Does this one is really needed? */
	GHashTable *fallback; 	/**< Fallback callback. */
};

/**
 * Create a new instance of a MsnTable which map commands, errors and messages
 * with callbacks that will handle it.
 *
 * @return A new MsnTable.
 */
MsnTable *msn_table_new(void);

/**
 * Destroy a MsnTable.
 *
 * @param table The MsnTable to be destroyed.
 */
void msn_table_destroy(MsnTable *table);

/**
 * Relate an incomming command from server with a callback able to handle
 * the event.
 *
 * @param table 	The MsnTable.
 * @param command 	If NULL this add an incoming asyncronous command set in answer.
 * 					Else, the command sent.
 * @param answer 	The server answer to 'command'. If 'command' is NULL,
 * 					the asyncronous command sent by the server.
 * @param cb 		Callback to handle this event.
 */
void msn_table_add_cmd(MsnTable *table, char *command, char *answer,
					   MsnTransCb cb);

/**
 * Set a callback to handle incoming command errors.
 *
 * @param table 	The MsnTable.
 * @param answer 	Incoming command with error.
 * @param cb 		Callback to handle this error.
 */
void msn_table_add_error(MsnTable *table, char *answer, MsnErrorCb cb);

/**
 * Relate a message Content-type with a callback able to handle it.
 *
 * @param table 	The MsnTable.
 * @param type 		The Message Content-Type.
 * @param cb 		Callback to handle this Content-type.
 */
void msn_table_add_msg_type(MsnTable *table, char *type, MsnMsgTypeCb cb);

#endif /* MSN_TABLE_H */
