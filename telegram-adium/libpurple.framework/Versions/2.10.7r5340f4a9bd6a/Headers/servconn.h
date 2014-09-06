/**
 * @file servconn.h Server connection functions
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
#ifndef MSN_SERVCONN_H
#define MSN_SERVCONN_H

typedef struct _MsnServConn MsnServConn;

/**
 * Connection error types.
 */
typedef enum
{
	MSN_SERVCONN_ERROR_NONE,
	MSN_SERVCONN_ERROR_CONNECT,
	MSN_SERVCONN_ERROR_WRITE,
	MSN_SERVCONN_ERROR_READ
} MsnServConnError;

/**
 * Connection types.
 */
typedef enum
{
	MSN_SERVCONN_NS,
	MSN_SERVCONN_SB
} MsnServConnType;

#include "internal.h"
#include "proxy.h"

#include "cmdproc.h"
#include "httpconn.h"
#include "session.h"

/**
 * A Connection.
 */
struct _MsnServConn
{
	MsnServConnType type; /**< The type of this connection. */
	MsnSession *session;  /**< The MSN session of this connection. */
	MsnCmdProc *cmdproc;  /**< The command processor of this connection. */

	PurpleProxyConnectData *connect_data;

	gboolean connected;   /**< A flag that states if it's connected. */
	gboolean processing;  /**< A flag that states if something is working
							with this connection. */
	gboolean wasted;      /**< A flag that states if it should be destroyed. */

	char *host; /**< The host this connection is connected or should be
				  connected to. */
	int num; /**< A number id of this connection. */

	MsnHttpConn *httpconn; /**< The HTTP connection this connection should use. */

	int fd; /**< The connection's file descriptor. */
	int inpa; /**< The connection's input handler. */

	char *rx_buf; /**< The receive buffer. */
	int rx_len; /**< The receive buffer lenght. */

	size_t payload_len; /**< The length of the payload.
						  It's only set when we've received a command that
						  has a payload. */

	PurpleCircBuffer *tx_buf;
	guint tx_handler;
	guint timeout_sec;
	guint timeout_handle;

	void (*connect_cb)(MsnServConn *); /**< The callback to call when connecting. */
	void (*disconnect_cb)(MsnServConn *); /**< The callback to call when disconnecting. */
	void (*destroy_cb)(MsnServConn *); /**< The callback to call when destroying. */
};

/**
 * Creates a new connection object.
 *
 * @param session The session.
 * @param type The type of the connection.
 */
MsnServConn *msn_servconn_new(MsnSession *session, MsnServConnType type);

/**
 * Destroys a connection object.
 *
 * @param servconn The connection.
 */
void msn_servconn_destroy(MsnServConn *servconn);

/**
 * Connects to a host.
 *
 * @param servconn The connection.
 * @param host The host.
 * @param port The port.
 * @param force Force this servconn to connect to a new server.
 */
gboolean msn_servconn_connect(MsnServConn *servconn, const char *host, int port,
                              gboolean force);

/**
 * Disconnects.
 *
 * @param servconn The connection.
 */
void msn_servconn_disconnect(MsnServConn *servconn);

/**
 * Sets the connect callback.
 *
 * @param servconn The servconn.
 * @param connect_cb The connect callback.
 */
void msn_servconn_set_connect_cb(MsnServConn *servconn,
								 void (*connect_cb)(MsnServConn *));
/**
 * Sets the disconnect callback.
 *
 * @param servconn The servconn.
 * @param disconnect_cb The disconnect callback.
 */
void msn_servconn_set_disconnect_cb(MsnServConn *servconn,
									void (*disconnect_cb)(MsnServConn *));
/**
 * Sets the destroy callback.
 *
 * @param servconn The servconn that's being destroyed.
 * @param destroy_cb The destroy callback.
 */
void msn_servconn_set_destroy_cb(MsnServConn *servconn,
								 void (*destroy_cb)(MsnServConn *));

/**
 * Writes a chunck of data to the servconn.
 *
 * @param servconn The servconn.
 * @param buf The data to write.
 * @param size The size of the data.
 */
gssize msn_servconn_write(MsnServConn *servconn, const char *buf,
						  size_t size);

/**
 * Function to call whenever an error related to a switchboard occurs.
 *
 * @param servconn The servconn.
 * @param error The error that happened.
 */
void msn_servconn_got_error(MsnServConn *servconn, MsnServConnError error,
                            const char *reason);

/**
 * Process the data in servconn->rx_buf.  This is called after reading
 * data from the socket.
 *
 * @param servconn The servconn.
 *
 * @return @c NULL if servconn was destroyed, 'servconn' otherwise.
 */
MsnServConn *msn_servconn_process_data(MsnServConn *servconn);

/**
 * Set a idle timeout fot this servconn
 *
 * @param servconn The servconn
 * @param seconds The idle timeout in seconds
 */
void msn_servconn_set_idle_timeout(MsnServConn *servconn, guint seconds);

#endif /* MSN_SERVCONN_H */
