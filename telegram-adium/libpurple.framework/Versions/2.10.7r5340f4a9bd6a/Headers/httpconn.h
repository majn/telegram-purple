/**
 * @file httpconn.h HTTP connection
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
#ifndef MSN_HTTPCONN_H
#define MSN_HTTPCONN_H

typedef struct _MsnHttpConn MsnHttpConn;

#include "circbuffer.h"
#include "servconn.h"
#include "session.h"

/**
 * An HTTP Connection.
 */
struct _MsnHttpConn
{
	MsnSession *session; /**< The MSN Session. */
	MsnServConn *servconn; /**< The connection object. */

	PurpleProxyConnectData *connect_data;

	char *full_session_id; /**< The full session id. */
	char *session_id; /**< The trimmed session id. */

	int timer; /**< The timer for polling. */

	gboolean waiting_response; /**< The flag that states if we are waiting
								 a response from the server. */
	gboolean connected;        /**< The flag that states if the connection is on. */
	gboolean virgin;           /**< The flag that states if this connection
								 should specify the host (not gateway) to
								 connect to. */

	char *host; /**< The HTTP gateway host. */
	GList *queue; /**< The queue of data chunks to write. */

	int fd; /**< The connection's file descriptor. */
	guint inpa; /**< The connection's input handler. */

	char *rx_buf; /**< The receive buffer. */
	int rx_len; /**< The receive buffer length. */

	PurpleCircBuffer *tx_buf;
	guint tx_handler;
};

/**
 * Creates a new HTTP connection object.
 *
 * @param servconn The connection object.
 *
 * @return The new object.
 */
MsnHttpConn *msn_httpconn_new(MsnServConn *servconn);

/**
 * Destroys an HTTP connection object.
 *
 * @param httpconn The HTTP connection object.
 */
void msn_httpconn_destroy(MsnHttpConn *httpconn);

/**
 * Writes a chunk of data to the HTTP connection.
 *
 * @param servconn    The server connection.
 * @param data        The data to write.
 * @param data_len    The size of the data to write.
 *
 * @return The number of bytes written.
 */
gssize msn_httpconn_write(MsnHttpConn *httpconn, const char *data, size_t data_len);

/**
 * Connects the HTTP connection object to a host.
 *
 * @param httpconn The HTTP connection object.
 * @param host The host to connect to.
 * @param port The port to connect to.
 */
gboolean msn_httpconn_connect(MsnHttpConn *httpconn,
							  const char *host, int port);

/**
 * Disconnects the HTTP connection object.
 *
 * @param httpconn The HTTP connection object.
 */
void msn_httpconn_disconnect(MsnHttpConn *httpconn);

#endif /* MSN_HTTPCONN_H */
