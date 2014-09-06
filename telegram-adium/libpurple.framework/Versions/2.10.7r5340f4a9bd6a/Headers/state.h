/**
 * @file state.h State functions and definitions
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
#ifndef MSN_STATE_H
#define MSN_STATE_H

/**
 * Away types.
 */
typedef enum
{
	MSN_ONLINE  = 1,
	MSN_BUSY    = 2,
	MSN_IDLE    = 3,
	MSN_BRB     = 4,
	MSN_AWAY    = 5,
	MSN_PHONE   = 6,
	MSN_LUNCH   = 7,
	MSN_OFFLINE = 8,
	MSN_HIDDEN  = 9
} MsnAwayType;

/**
 * Changes the status of the user.
 *
 * @param session The MSN session.
 */
void msn_change_status(MsnSession *session);

/**
 * Returns the string representation of an away type.
 *
 * @param type The away type.
 *
 * @return The string representation of the away type.
 */
const char *msn_away_get_text(MsnAwayType type);

const char *msn_state_get_text(MsnAwayType state);

/* Get the CurrentMedia info from the XML node */
char *msn_get_currentmedia(xmlnode *payloadNode);

/* Get the PSM info from the XML node */
char *msn_get_psm(xmlnode *payloadNode);

MsnAwayType msn_state_from_account(PurpleAccount *account);

#endif /* MSN_STATE_H */
