/**
 * @file error.h Error functions
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
#ifndef MSN_ERROR_H
#define MSN_ERROR_H

#include "session.h"

/**
 * Returns the string representation of an error type.
 *
 * @param type The error type.
 * @param debug Whether this should be treated as a debug log message or a user-visible error
 *
 * @return The string representation of the error type.
 */
const char *msn_error_get_text(unsigned int type, gboolean *debug);

/**
 * Handles an error.
 *
 * @param session The current session.
 * @param type    The error type.
 */
void msn_error_handle(MsnSession *session, unsigned int type);

/**
 * Show the sync issue in a dialog using request api
 *
 * @param sesion 		MsnSession associated to this error.
 * @param passport 		The passport associated with the error.
 * @param group_name 	The group in the buddy is suppoused to be
 */
void msn_error_sync_issue(MsnSession *session, const char *passport,
						 const char *group_name);

#endif /* MSN_ERROR_H */
