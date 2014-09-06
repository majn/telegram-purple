/**
 * @file cmdproc.h MSN command processor functions
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
#ifndef MSN_CMDPROC_H
#define MSN_CMDPROC_H

typedef struct _MsnCmdProc MsnCmdProc;

#include "command.h"
#include "history.h"
#include "servconn.h"
#include "session.h"
#include "table.h"

struct _MsnCmdProc
{
	MsnSession *session;
	MsnServConn *servconn;

	GQueue *txqueue;

	MsnCommand *last_cmd;

	MsnTable *cbs_table;

	MsnHistory *history;

	GHashTable *multiparts; /**< Multi-part message ID's */

	void *data; /**< Extra data, like the switchboard. */
};

/**
 * Creates a MsnCmdProc structure.
 *
 * @param session 	The session to associate with.
 *
 * @return A new MsnCmdProc structure.
 */
MsnCmdProc *msn_cmdproc_new(MsnSession *session);

/**
 * Destroys an MsnCmdProc.
 *
 * @param cmdproc 	The object structure.
 */
void msn_cmdproc_destroy(MsnCmdProc *cmdproc);

/**
 * Process the queued transactions.
 *
 * @param cmdproc 	The MsnCmdProc.
 */
void msn_cmdproc_process_queue(MsnCmdProc *cmdproc);

/**
 * Sends transaction using this servconn.
 *
 * @param cmdproc 	The MsnCmdProc to be used.
 * @param trans 	The MsnTransaction to be sent.
 */
gboolean msn_cmdproc_send_trans(MsnCmdProc *cmdproc, MsnTransaction *trans);

/**
 * Add a transaction to the queue to be processed latter.
 *
 * @param cmdproc 	The MsnCmdProc in which the transaction will be queued.
 * @param trans 	The MsnTransaction to be queued.
 */
void msn_cmdproc_queue_trans(MsnCmdProc *cmdproc,
							 MsnTransaction *trans);

void msn_cmdproc_process_msg(MsnCmdProc *cmdproc,
							 MsnMessage *msg);
void msn_cmdproc_process_cmd(MsnCmdProc *cmdproc, MsnCommand *cmd);
void msn_cmdproc_process_cmd_text(MsnCmdProc *cmdproc, const char *command);
void msn_cmdproc_process_payload(MsnCmdProc *cmdproc,
								 char *payload, int payload_len);

#endif /* MSN_CMDPROC_H */
