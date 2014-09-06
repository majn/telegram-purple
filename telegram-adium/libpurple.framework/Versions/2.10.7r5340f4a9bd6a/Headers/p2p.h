/**
 * @file p2p.h MSN P2P functions
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

#ifndef MSN_P2P_H
#define MSN_P2P_H

typedef struct {
	guint32 session_id;
	guint32 id;
	/**
	 * In a MsnSlpMessage:
	 * For outgoing messages this is the number of bytes from buffer that
	 * have already been sent out.  For incoming messages this is the
	 * number of bytes that have been written to buffer.
	 */
	guint64 offset;
	guint64 total_size;
	guint32 length;
	guint32 flags;
	guint32 ack_id;
	guint32 ack_sub_id;
	guint64 ack_size;
/*	guint8  body[1]; */
} MsnP2PHeader;
#define P2P_PACKET_HEADER_SIZE (6 * 4 + 3 * 8)

typedef struct {
	guint8  header_len;
	guint8  opcode;
	guint16 message_len;
	guint32 base_id;
	GSList *header_tlv;
	guint8  data_header_len;
	guint8  data_tf;
	guint16 package_number;
	guint32 session_id;
	GSList *data_tlv;
/*	guint8  body[1]; */
} MsnP2Pv2Header;

typedef struct {
	guint16 protocol_version;
	guint16 implementation_id;
	guint16 version;
	guint16 reserved;
	guint32 caps;
} P2PPeerInfo;

typedef enum
{
	TF_FIRST    = 0x01,     /**< The first package. */
	TF_MSNOBJ   = 0x04,     /**< Payload contains binary data for MsnObject. */
	TF_FILE     = 0x06      /**< Payload contains binary data. */
} TF;

typedef enum
{
	P2P_HEADER_TLV_TYPE_PEER_INFO  = 0x01, /**< Client peer info */
	P2P_HEADER_TLV_TYPE_ACK        = 0x02, /**< ACK */
	P2P_HEADER_TLV_TYPE_NAK        = 0x03  /**< NAK */
} P2PHeaderTLVType;

typedef enum
{
	P2P_DATA_TLV_REMAINING  = 0x01, /**< Indicates the remaining data to transfer.*/
} P2PDataTLVType;

typedef enum
{
	P2P_PI_PVER     = 0x0200,
	P2P_PI_IMP_ID   = 0,
	P2P_PI_VER      = 0x0e00,
	P2P_PI_RES      = 0,
	P2P_PI_CAPS     = 0x0000010f
} P2PPeerInfoVal;

typedef struct
{
	guint32 value;
} MsnP2PFooter;
#define P2P_PACKET_FOOTER_SIZE (1 * 4)

typedef enum
{
	MSN_P2P_VERSION_ONE = 0,
	MSN_P2P_VERSION_TWO = 1,
} MsnP2PVersion;

typedef struct {
	MsnP2PVersion version;
	union {
		MsnP2PHeader v1;
		MsnP2Pv2Header v2;
	} header;
	MsnP2PFooter footer;
} MsnP2PInfo;

typedef enum
{
	P2P_NO_FLAG         = 0x0,        /**< No flags specified */
	P2P_OUT_OF_ORDER    = 0x1,        /**< Chunk out-of-order */
	P2P_ACK             = 0x2,        /**< Acknowledgement */
	P2P_PENDING_INVITE  = 0x4,        /**< There is a pending invite */
	P2P_BINARY_ERROR    = 0x8,        /**< Error on the binary level */
	P2P_FILE            = 0x10,       /**< File */
	P2P_MSN_OBJ_DATA    = 0x20,       /**< MsnObject data */
	P2P_CLOSE           = 0x40,       /**< Close session */
	P2P_TLP_ERROR       = 0x80,       /**< Error at transport layer protocol */
	P2P_DC_HANDSHAKE    = 0x100,      /**< Direct Handshake */
	P2P_WLM2009_COMP    = 0x1000000,  /**< Compatibility with WLM 2009 */
	P2P_FILE_DATA       = 0x1000030   /**< File transfer data */
} MsnP2PHeaderFlag;
/* Info From:
 * http://msnpiki.msnfanatic.com/index.php/MSNC:P2Pv1_Headers#Flags
 * http://trac.kmess.org/changeset/ba04d0c825769d23370511031c47f6be75fe9b86
 * #7180
 */

typedef enum
{
	P2P_APPID_SESSION   = 0x0,        /**< Negotiating session */
	P2P_APPID_OBJ       = 0x1,        /**< MsnObject (Display or Emoticon) */
	P2P_APPID_FILE      = 0x2,        /**< File transfer */
	P2P_APPID_EMOTE     = 0xB,        /**< CustomEmoticon */
	P2P_APPID_DISPLAY   = 0xC         /**< Display Image */
} MsnP2PAppId;

typedef enum
{
	P2P_OPCODE_NONE = 0x00,
	P2P_OPCODE_SYN  = 0x01,
	P2P_OPCODE_RAK  = 0x02
} MsnP2Pv2OpCode;

MsnP2PInfo *
msn_p2p_info_new(MsnP2PVersion version);

MsnP2PInfo *
msn_p2p_info_dup(MsnP2PInfo *info);

void
msn_p2p_info_free(MsnP2PInfo *info);

size_t
msn_p2p_header_from_wire(MsnP2PInfo *info, const char *wire, size_t max_len);

char *
msn_p2p_header_to_wire(MsnP2PInfo *info, size_t *len);

size_t
msn_p2p_footer_from_wire(MsnP2PInfo *info, const char *wire);

char *
msn_p2p_footer_to_wire(MsnP2PInfo *info, size_t *len);

void
msn_p2p_info_to_string(MsnP2PInfo *info, GString *str);

gboolean
msn_p2p_msg_is_data(const MsnP2PInfo *info);

gboolean
msn_p2p_info_is_valid(MsnP2PInfo *info);

gboolean
msn_p2p_info_is_first(MsnP2PInfo *info);

gboolean
msn_p2p_info_is_final(MsnP2PInfo *info);

void
msn_p2p_info_create_ack(MsnP2PInfo *old_info, MsnP2PInfo *new_info);

gboolean
msn_p2p_info_require_ack(MsnP2PInfo *info);

gboolean
msn_p2p_info_is_ack(MsnP2PInfo *info);

void
msn_p2p_info_init_first(MsnP2PInfo *new_info, MsnP2PInfo *old_info);

guint32
msn_p2p_info_get_session_id(MsnP2PInfo *info);

guint32
msn_p2p_info_get_id(MsnP2PInfo *info);

guint64
msn_p2p_info_get_offset(MsnP2PInfo *info);

guint64
msn_p2p_info_get_total_size(MsnP2PInfo *info);

guint32
msn_p2p_info_get_length(MsnP2PInfo *info);

guint32
msn_p2p_info_get_flags(MsnP2PInfo *info);

guint32
msn_p2p_info_get_ack_id(MsnP2PInfo *info);

guint32
msn_p2p_info_get_ack_sub_id(MsnP2PInfo *info);

guint64
msn_p2p_info_get_ack_size(MsnP2PInfo *info);

guint32
msn_p2p_info_get_app_id(MsnP2PInfo *info);

void
msn_p2p_info_set_session_id(MsnP2PInfo *info, guint32 session_id);

void
msn_p2p_info_set_id(MsnP2PInfo *info, guint32 id);

void
msn_p2p_info_set_offset(MsnP2PInfo *info, guint64 offset);

void
msn_p2p_info_set_total_size(MsnP2PInfo *info, guint64 total_size);

void
msn_p2p_info_set_length(MsnP2PInfo *info, guint32 length);

void
msn_p2p_info_set_flags(MsnP2PInfo *info, guint32 flags);

void
msn_p2p_info_set_ack_id(MsnP2PInfo *info, guint32 ack_id);

void
msn_p2p_info_set_ack_sub_id(MsnP2PInfo *info, guint32 ack_sub_id);

void
msn_p2p_info_set_ack_size(MsnP2PInfo *info, guint64 ack_size);

void
msn_p2p_info_set_app_id(MsnP2PInfo *info, guint32 app_id);

#endif /* MSN_P2P_H */
