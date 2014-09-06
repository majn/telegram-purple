/*
 * Purple's oscar protocol plugin
 * This file is the legal property of its developers.
 * Please see the AUTHORS file distributed alongside this file.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
*/

/*
 * OFT and ODC Services
 */

#ifndef _PEER_H_
#define _PEER_H_

#include "ft.h"
#include "network.h"
#include "proxy.h"

typedef struct _ChecksumData          ChecksumData;
typedef struct _OdcFrame              OdcFrame;
typedef struct _OftFrame              OftFrame;
typedef struct _ProxyFrame            ProxyFrame;
typedef struct _PeerConnection        PeerConnection;

#define PEER_CONNECTION_FLAG_INITIATED_BY_ME  0x0001
#define PEER_CONNECTION_FLAG_APPROVED         0x0002
#define PEER_CONNECTION_FLAG_TRIED_DIRECT     0x0004
#define PEER_CONNECTION_FLAG_TRIED_INCOMING   0x0008
#define PEER_CONNECTION_FLAG_TRIED_PROXY      0x0010
#define PEER_CONNECTION_FLAG_IS_INCOMING      0x0020

#define PEER_TYPE_PROMPT 0x0101 /* "I am going to send you this file, is that ok?" */
#define PEER_TYPE_RESUMEACCEPT 0x0106 /* We are accepting the resume */
#define PEER_TYPE_ACK 0x0202 /* "Yes, it is ok for you to send me that file" */
#define PEER_TYPE_DONE 0x0204 /* "I received that file with no problems" or "I already have that file, great!" */
#define PEER_TYPE_RESUME 0x0205 /* Resume transferring, sent by whoever receives */
#define PEER_TYPE_RESUMEACK 0x0207 /* Our resume accept was ACKed */

#define PEER_TYPE_GETFILE_REQUESTLISTING 0x1108 /* "I have a listing.txt file, do you want it?" */
#define PEER_TYPE_GETFILE_RECEIVELISTING 0x1209 /* "Yes, please send me your listing.txt file" */
#define PEER_TYPE_GETFILE_RECEIVEDLISTING 0x120a /* received corrupt listing.txt file? I'm just guessing about this one... */
#define PEER_TYPE_GETFILE_ACKLISTING 0x120b /* "I received the listing.txt file successfully" */
#define PEER_TYPE_GETFILE_REQUESTFILE 0x120c /* "Please send me this file" */

/*
 * For peer proxying
 */
#define AIM_PEER_PROXY_SERVER         "ars.oscar.aol.com"
#define ICQ_PEER_PROXY_SERVER         "ars.icq.com"
#define PEER_PROXY_PORT           5190   /* The port we should always connect to */
#define PEER_PROXY_PACKET_VERSION 0x044a

/* Thanks to Keith Lea and the Joust project for documenting these */
#define PEER_PROXY_TYPE_ERROR   0x0001
#define PEER_PROXY_TYPE_CREATE  0x0002
#define PEER_PROXY_TYPE_CREATED 0x0003
#define PEER_PROXY_TYPE_JOIN    0x0004
#define PEER_PROXY_TYPE_READY   0x0005

struct _OdcFrame
{
	/* guchar magic[4]; */        /* 0 */
	/* guint16 length; */         /* 4 */
	guint16 type;                 /* 6 */
	guint16 subtype;              /* 8 */
	/* Unknown */                 /* 10 */
	guchar cookie[8];		      /* 12 */
	/* Unknown */
	/* guint32 payloadlength; */  /* 28 */
	guint16 encoding;             /* 32 */
	/* Unknown */
	guint16 flags;                /* 38 */
	/* Unknown */
	guchar bn[32];                /* 44 */
	/* Unknown */
	ByteStream payload;           /* 76 */
};

struct _OftFrame
{
	/* guchar magic[4]; */   /* 0 */
	/* guint16 length; */    /* 4 */
	guint16 type;            /* 6 */
	guchar cookie[8];        /* 8 */
	guint16 encrypt;         /* 16 */
	guint16 compress;        /* 18 */
	guint16 totfiles;        /* 20 */
	guint16 filesleft;       /* 22 */
	guint16 totparts;        /* 24 */
	guint16 partsleft;       /* 26 */
	guint32 totsize;         /* 28 */
	guint32 size;            /* 32 */
	guint32 modtime;         /* 36 */
	guint32 checksum;        /* 40 */
	guint32 rfrcsum;         /* 44 */
	guint32 rfsize;          /* 48 */
	guint32 cretime;         /* 52 */
	guint32 rfcsum;          /* 56 */
	guint32 nrecvd;          /* 60 */
	guint32 recvcsum;        /* 64 */
	guchar idstring[32];     /* 68 */
	guint8 flags;            /* 100 */
	guint8 lnameoffset;      /* 101 */
	guint8 lsizeoffset;      /* 102 */
	guchar dummy[69];        /* 103 */
	guchar macfileinfo[16];  /* 172 */
	guint16 nencode;         /* 188 */
	guint16 nlanguage;       /* 190 */
	guchar *name;            /* 192 */
	size_t name_length;
	/* Payload? */           /* 256 */
};

struct _ProxyFrame
{
	/* guint16 length; */    /* 0 */
	guint16 version;         /* 2 */
	guint16 type;            /* 4 */
	guint32 unknown;         /* 6 */
	guint16 flags;           /* 10 */
	ByteStream payload;      /* 12 */
};

struct _PeerConnection
{
	OscarData *od;
	guint64 type;
	char *bn;
	guchar magic[4];
	guchar cookie[8];
	guint16 lastrequestnumber;

	gboolean ready;
	int flags;                       /**< Bitmask of PEER_CONNECTION_FLAG_ */
	time_t lastactivity;             /**< Time of last transmit. */
	guint destroy_timeout;
	OscarDisconnectReason disconnect_reason;
	char *error_message;

	/**
	 * A pointer to either an OdcFrame or an OftFrame.
	 */
	gpointer frame;

	/**
	 * This is only used when the peer connection is being established.
	 */
	PurpleProxyConnectData *client_connect_data;
	PurpleProxyConnectData *verified_connect_data;

	/**
	 * This is only used when the peer connection is being established.
	 */
	PurpleNetworkListenData *listen_data;


	/**
	 * This is only used when the peer connection is being established.
	 */
	guint connect_timeout_timer;

	/**
	 * This is only used while the remote user is attempting to
	 * connect to us.
	 */
	int listenerfd;

	int fd;
	guint8 header[6];
	gssize header_received;
	guint8 proxy_header[12];
	gssize proxy_header_received;
	ByteStream buffer_incoming;
	PurpleCircBuffer *buffer_outgoing;
	guint watcher_incoming;
	guint watcher_outgoing;

	/**
	 * IP address of the proxy server, if applicable.
	 */
	gchar *proxyip;

	/**
	 * IP address of the remote user from THEIR point of view.
	 */
	gchar *clientip;

	/**
	 * IP address of the remote user from the oscar server's
	 * point of view.
	 */
	gchar *verifiedip;

	guint16 port;
	gboolean use_proxy;

	/**
	 * Checksumming
	 */
	ChecksumData *checksum_data;

	/* TODOFT */
	PurpleXfer *xfer;
	OftFrame xferdata;
	guint sending_data_timer;
};

/*
 * For all peer connections
 */

/**
 * Create a new PeerConnection structure and initialize it with some
 * sane defaults.
 *
 * @param type The type of the peer connection.  One of
 *        OSCAR_CAPABILITY_DIRECTIM or OSCAR_CAPABILITY_SENDFILE.
 */
PeerConnection *peer_connection_new(OscarData *od, guint64 type, const char *bn);

void peer_connection_destroy(PeerConnection *conn, OscarDisconnectReason reason, const gchar *error_message);
void peer_connection_schedule_destroy(PeerConnection *conn, OscarDisconnectReason reason, const gchar *error_message);
PeerConnection *peer_connection_find_by_type(OscarData *od, const char *bn, guint64 type);
PeerConnection *peer_connection_find_by_cookie(OscarData *od, const char *bn, const guchar *cookie);

void peer_connection_listen_cb(gpointer data, gint source, PurpleInputCondition cond);
void peer_connection_recv_cb(gpointer data, gint source, PurpleInputCondition cond);
void peer_connection_send(PeerConnection *conn, ByteStream *bs);

void peer_connection_trynext(PeerConnection *conn);
void peer_connection_finalize_connection(PeerConnection *conn);
void peer_connection_propose(OscarData *od, guint64 type, const char *bn);
void peer_connection_got_proposition(OscarData *od, const gchar *bn, const gchar *message, IcbmArgsCh2 *args);

/*
 * For ODC
 */
void peer_odc_close(PeerConnection *conn);
void peer_odc_recv_frame(PeerConnection *conn, ByteStream *bs);
void peer_odc_send_cookie(PeerConnection *conn);
void peer_odc_send_typing(PeerConnection *conn, PurpleTypingState typing);
void peer_odc_send_im(PeerConnection *conn, const char *msg, int len, int encoding, gboolean autoreply);

/*
 * For OFT
 */
void peer_oft_close(PeerConnection *conn);
void peer_oft_recv_frame(PeerConnection *conn, ByteStream *bs);
void peer_oft_send_prompt(PeerConnection *conn);
void peer_oft_checksum_destroy(ChecksumData *checksum_data);

/* Xfer callbacks for receiving a file */
void peer_oft_recvcb_init(PurpleXfer *xfer);
void peer_oft_recvcb_end(PurpleXfer *xfer);
void peer_oft_recvcb_ack_recv(PurpleXfer *xfer, const guchar *buffer, size_t size);

/* Xfer callbacks for sending a file */
void peer_oft_sendcb_init(PurpleXfer *xfer);
void peer_oft_sendcb_ack(PurpleXfer *xfer, const guchar *buffer, size_t size);

/* Xfer callbacks for both sending and receiving */
void peer_oft_cb_generic_cancel(PurpleXfer *xfer);

/*
 * For peer proxying
 */
void peer_proxy_connection_established_cb(gpointer data, gint source, const gchar *error_message);

#endif /* _PEER_H_ */
