/**
 * @file command.h MSN command functions
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
#ifndef MSN_COMMAND_H
#define MSN_COMMAND_H

typedef struct _MsnCommand MsnCommand;

#include "cmdproc.h"
#include "transaction.h"

typedef void (*MsnPayloadCb)(MsnCmdProc *cmdproc, MsnCommand *cmd,
							 char *payload, size_t len);

/**
 * A received command.
 */
struct _MsnCommand
{
	unsigned int trId;

	char *command;
	char **params;
	int param_count;

	guint ref_count;

	MsnTransaction *trans;

	char *payload;
	size_t payload_len;

	MsnPayloadCb payload_cb;
	void *payload_cbdata;
};

/**
 * Create a command object from the incoming string and ref it.
 *
 * @param string 	The incoming string.
 *
 * @return 			A MsnCommand object.
 */
MsnCommand *msn_command_from_string(const char *string);

/**
 * Increment the ref count.
 *
 * @param cmd 	The MsnCommand to be ref.
 *
 * @return 		The ref command.
 */
MsnCommand *msn_command_ref(MsnCommand *cmd);

/**
 * Decrement the ref count. If the count goes to 0, destroy it.
 *
 * @param cmd	The MsnCommand to be unref.
 *
 */
void msn_command_unref(MsnCommand *cmd);

#endif /* MSN_COMMAND_H */

