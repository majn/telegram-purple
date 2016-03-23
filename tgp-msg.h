/*
 This file is part of telegram-purple
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 
 Copyright Matthias Jentsch 2014-2015
 */

#ifndef __telegram_adium__tgp_msg__
#define __telegram_adium__tgp_msg__

/**
 * Process a message and display it
 *
 * Loads embedded ressources like pictures or document thumbnails and ensures that 
 * that all messages are still displayed in the original incoming order.
 */
void tgp_msg_recv (struct tgl_state *TLS, struct tgl_message *M, GList *before);

/**
 * Process a message and send it the peer
 *
 * Removes all HTML escape chars and HTML markup, finds  embedded images and sends 
 * them as pictures and splits up messages that are too big for single Telegram
 * messages.
 */
int tgp_msg_send (struct tgl_state *TLS, const char *msg, tgl_peer_id_t to);

/**
 * Print a special message in the conversation with a peer assuring that special flags are displayed
 */
void tgp_msg_special_out (struct tgl_state *TLS, const char *msg, tgl_peer_id_t to_id, int flags);

#endif
