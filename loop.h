/*
    This file is part of telegram-client.

    Telegram-client is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Telegram-client is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this telegram-client.  If not, see <http://www.gnu.org/licenses/>.

    Copyright Vitaly Valtman 2013
*/

#include "telegram.h"

#ifndef __LOOP_H__
#define __LOOP_H__
// forward declarations

struct dc;
struct telegram;


/**
 * The authorization state, storing all known data centers of the telegram
 * network and associated shared keys for transport encryption
 */
struct authorization_state {
  int dc_working_num;
  int auth_state;
  struct dc* DC_list[11];
  int our_id;
};

/**
 * Stores the current update state of a client
 */
struct protocol_state {

    /**
     * Number of events occured in a text box
     */
    int pts;

    /**
     * Position within a siccession of unconfirmed updates, used for encrypted
     * messages
     */
    int qts;

    /**
     * Number of sent updates
     */
    int seq;

    /**
     * Last update
     */
    int last_date;
};

void write_secret_chat_file (struct telegram *instance, const char *filename);

void write_auth_file (struct authorization_state *state, const char *filename);
struct authorization_state read_auth_file (const char *filename);

void write_state_file (struct protocol_state *state, const char *filename);
struct protocol_state read_state_file (const char *filename);

void write_secret_chat_file (struct telegram *instance, const char *filename);
void read_secret_chat_file (struct telegram *instance, const char *filename);

#endif
