/**
 * @file directconn.h MSN direct connection functions
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
#ifndef MSN_DIRECTCONN_H
#define MSN_DIRECTCONN_H

typedef struct _MsnDirectConn MsnDirectConn;

#include "network.h"
#include "proxy.h"
#include "circbuffer.h"

#include "slp.h"
#include "slplink.h"
#include "slpmsg.h"
#include "slpmsg_part.h"
#include "p2p.h"

#define MSN_DCCONN_MAX_SIZE 1352

typedef enum
{
	DC_STATE_CLOSED,            /*< No socket opened yet */
	DC_STATE_FOO,               /*< Waiting for FOO message */
	DC_STATE_HANDSHAKE,         /*< Waiting for handshake message */
	DC_STATE_HANDSHAKE_REPLY,   /*< Waiting for handshake reply message */
	DC_STATE_ESTABLISHED        /*< Handshake complete */
} MsnDirectConnState;

typedef enum
{
	DC_PROCESS_OK = 0,
	DC_PROCESS_ERROR,
	DC_PROCESS_FALLBACK,
	DC_PROCESS_CLOSE

} MsnDirectConnProcessResult;

typedef enum
{
	DC_NONCE_UNKNOWN,	/**< Invalid scheme */
	DC_NONCE_PLAIN,     /**< No hashing */
	DC_NONCE_SHA1       /**< First 16 bytes of SHA1 of nonce */

} MsnDirectConnNonceType;

typedef struct _MsnDirectConnPacket MsnDirectConnPacket;

struct _MsnDirectConnPacket {
	guint32     length;
	guchar      *data;

	void        (*sent_cb)(struct _MsnDirectConnPacket*);
	MsnSlpMessagePart *part;
};

struct _MsnDirectConn
{
	MsnDirectConnState  state;      /**< Direct connection status */
	MsnSlpLink          *slplink;   /**< The slplink using this direct connection */
	MsnSlpCall          *slpcall;   /**< The slpcall which initiated the direct connection */
	char                *msg_body;  /**< The body of message sent by send_connection_info_msg_cb */
	MsnSlpMessage       *prev_ack;  /**< The saved SLP ACK message */

	MsnDirectConnNonceType nonce_type;         /**< The type of nonce hashing */
	guchar                 nonce[16];          /**< The nonce used for handshake */
	gchar                  nonce_hash[37];     /**< The hash of nonce */
	gchar                  remote_nonce[37];   /**< The remote side's nonce */

	PurpleNetworkListenData *listen_data;           /**< The pending socket creation request */
	PurpleProxyConnectData  *connect_data;          /**< The pending connection attempt */
	int                     listenfd;               /**< The socket we're listening for incoming connections */
	guint                   listenfd_handle;        /**< The timeout handle for incoming connection */
	guint                   connect_timeout_handle; /**< The timeout handle for outgoing connection */

	int     fd;             /**< The direct connection socket */
	guint   recv_handle;    /**< The incoming data callback handle */
	guint   send_handle;    /**< The outgoing data callback handle */

	gchar   *in_buffer; /**< The receive buffer */
	int     in_size;    /**< The receive buffer size */
	int     in_pos;     /**< The first free position in receive buffer */
	GQueue  *out_queue; /**< The outgoing packet queue */
	int     msg_pos;    /**< The position of next byte to be sent in the actual packet */

	/** The callback used for sending information to the peer about the opened socket */
	void (*send_connection_info_msg_cb)(MsnDirectConn *);

	gchar   *ext_ip;    /**< Our external IP address */
	int     ext_port;   /**< Our external port */

	guint       timeout_handle;
	gboolean    progress;

	/*int   num_calls;*/  /**< The number of slpcalls using this direct connection */
};

/* Outgoing attempt */
#define DC_OUTGOING_TIMEOUT (5)
/* Time for internal + external connection attempts */
#define DC_INCOMING_TIMEOUT (DC_OUTGOING_TIMEOUT * 3)
/* Timeout for lack of activity */
#define DC_TIMEOUT          (60)

/*
 * Queues an MSN message to be sent via direct connection.
 */
void
msn_dc_enqueue_part(MsnDirectConn *dc, MsnSlpMessagePart *part);

/*
 * Creates, initializes, and returns a new MsnDirectConn structure.
 */
MsnDirectConn *
msn_dc_new(MsnSlpCall *slpcall);

/*
 * Destroys an MsnDirectConn structure. Frees every buffer allocated earlier
 * restores saved callbacks, etc.
 */
void
msn_dc_destroy(MsnDirectConn *dc);

/*
 * Fallback to switchboard connection. Used when neither side is able to
 * create a listening socket.
 */
void
msn_dc_fallback_to_sb(MsnDirectConn *dc);

/*
 * Increases the slpcall counter in DC. The direct connection remains open
 * until all slpcalls using it are destroyed.
 */
void
msn_dc_ref(MsnDirectConn *dc);

/*
 * Decrease the slpcall counter in DC. The direct connection remains open
 * until all slpcalls using it are destroyed.
 */
void
msn_dc_unref(MsnDirectConn *dc);

/*
 * Sends a direct connect INVITE message on the associated slplink
 * with the corresponding connection type and information.
 */
void
msn_dc_send_invite(MsnDirectConn *dc);

/*
 * Sends a direct connect OK message as a response to an INVITE received earliaer
 * on the corresponding slplink.
 */
void
msn_dc_send_ok(MsnDirectConn *dc);

/*
 * This callback will be called when we're successfully connected to
 * the remote host.
 */
void
msn_dc_connected_to_peer_cb(gpointer data, gint fd, const gchar *error_msg);

/*
 * This callback will be called when we're unable to connect to
 * the remote host in DC_CONNECT_TIMEOUT seconds.
 */
gboolean
msn_dc_outgoing_connection_timeout_cb(gpointer data);

/*
 * This callback will be called when the listening socket is successfully
 * created and its parameters (IP/port) are available.
 */
void
msn_dc_listen_socket_created_cb(int listenfd, gpointer data);

#endif /* MSN_DIRECTCONN_H */
