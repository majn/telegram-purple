/**
 * @file msn.h The MSN protocol plugin
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
#ifndef MSN_H
#define MSN_H

typedef enum
{
	MSN_CAP_VIA_MOBILE    = 0x0000001,
	MSN_CAP_VIA_TEXAS     = 0x0000002,
	MSN_CAP_INK_GIF       = 0x0000004,
	MSN_CAP_INK_ISF       = 0x0000008,
	MSN_CAP_VIDEO_CHAT    = 0x0000010,
	MSN_CAP_PACKET        = 0x0000020,
	MSN_CAP_MOBILE_ON     = 0x0000040,
	MSN_CAP_WEB_WATCH     = 0x0000080,
	MSN_CAP_ACTIVITIES    = 0x0000100,
	MSN_CAP_VIA_WEBIM     = 0x0000200,
	MSN_CAP_MOBILE_DEV    = 0x0000400,
	MSN_CAP_VIA_FEDERATED = 0x0000800,
	MSN_CAP_SPACE         = 0x0001000,
	MSN_CAP_MCE           = 0x0002000,
	MSN_CAP_DIRECTIM      = 0x0004000,
	MSN_CAP_WINKS         = 0x0008000,
	MSN_CAP_SEARCH        = 0x0010000,
	MSN_CAP_BOT           = 0x0020000,
	MSN_CAP_VOICEIM       = 0x0040000,
	MSN_CAP_SCHANNEL      = 0x0080000,
	MSN_CAP_SIP_INVITE    = 0x0100000,
	MSN_CAP_MULTI_VV      = 0x0200000,
	MSN_CAP_SDRIVE        = 0x0400000,
	MSN_CAP_PAGEMODE_MSG  = 0x080000,
	MSN_CAP_ONECARE       = 0x1000000,
	MSN_CAP_P2P_TURN      = 0x2000000,
	MSN_CAP_P2P_BOOTSTRAP_VIA_UUN = 0x4000000,
	MSN_CAP_ALIASED       = 0x8000000
} MsnClientCaps;

typedef enum
{
	MSN_EXT_CAP_SMS_ONLY            = 0x1,
	MSN_EXT_CAP_VOICE_OVER_MSNP     = 0x2,
	MSN_EXT_CAP_UUCP_SIP            = 0x4,
	MSN_EXT_CAP_APP_MSGS            = 0x8,
	MSN_EXT_CAP_RTC_VIDEO           = 0x10,
	MSN_EXT_CAP_P2PV2               = 0x20,
	MSN_EXT_CAP_AUTH_WEBIM          = 0x40,
	MSN_EXT_CAP_1ON1_VIA_GROUP      = 0x80,
	MSN_EXT_CAP_OFFLINEIM           = 0x100,
	MSN_EXT_CAP_SHARING_VIDEO       = 0x200,
	MSN_EXT_CAP_NUDGE               = 0x400,
	MSN_EXT_CAP_CIRCLE_VOICEIM      = 0x800,
	MSN_EXT_CAP_SHARING             = 0x1000,
	MSN_EXT_CAP_P2P_MIXER_RELAY     = 0x8000,
	MSN_EXT_CAP_CONV_WINDOW_FT      = 0x20000,
	MSN_EXT_CAP_VIDEO_16x9          = 0x40000,
	MSN_EXT_CAP_P2P_ENVELOPE        = 0x80000,
	MSN_EXT_CAP_YAHOOIM_DISABLE     = 0x400000,
	MSN_EXT_CAP_SIP_TUNNELv2        = 0x800000,
	MSN_EXT_CAP_VOICE_CLIP_WMA      = 0x1000000,
	MSN_EXT_CAP_VOICE_CLIP_CIRCLEIM = 0x2000000,
	MSN_EXT_CAP_SOCIAL_NEWS         = 0x4000000,
	MSN_EXT_CAP_CUSTOM_SMILEY       = 0x8000000,
	MSN_EXT_CAP_UTF8_MOODS          = 0x10000000,
	MSN_EXT_CAP_FTURN               = 0x20000000,
	MSN_EXT_CAP_P4_ACTIVITY         = 0x40000000,
	MSN_EXT_CAP_MUC                 = 0x80000000
} MsnClientExtCaps;

typedef enum
{
	MSN_CLIENT_VER_5_0  = 0x00,
	MSN_CLIENT_VER_6_0  = 0x10,	/* MSNC1 */
	MSN_CLIENT_VER_6_1  = 0x20,	/* MSNC2 */
	MSN_CLIENT_VER_6_2  = 0x30,	/* MSNC3 */
	MSN_CLIENT_VER_7_0  = 0x40,	/* MSNC4 */
	MSN_CLIENT_VER_7_5  = 0x50,	/* MSNC5 */
	MSN_CLIENT_VER_8_0  = 0x60,	/* MSNC6 */
	MSN_CLIENT_VER_8_1  = 0x70,	/* MSNC7 */
	MSN_CLIENT_VER_8_5  = 0x80,	/* MSNC8 */
	MSN_CLIENT_VER_9_0  = 0x90,	/* MSNC9 */
	MSN_CLIENT_VER_14_0 = 0xA0,	/* MSNC10 */
	MSN_CLIENT_VER_15_0 = 0xB0	/* MSNC11 */
} MsnClientVerId;

#include "internal.h"

#include "session.h"

#include "msg.h"

#define MSN_BUF_LEN 8192

/* Windows Live Messenger Server*/
#define MSN_SERVER "messenger.hotmail.com"
#define MSN_HTTPCONN_SERVER "gateway.messenger.hotmail.com"
#define MSN_PORT 1863
#define WLM_PROT_VER		18

#define WLM_MAX_PROTOCOL	18
#define WLM_MIN_PROTOCOL	18

#define MSN_TYPING_RECV_TIMEOUT 6
#define MSN_TYPING_SEND_TIMEOUT	4

#define PROFILE_URL "http://spaces.live.com/profile.aspx?mem="
#define PHOTO_URL	" contactparams:photopreauthurl=\""

#define BUDDY_ALIAS_MAXLEN 387

#define MSN_CAM_GUID "4BD96FC0-AB17-4425-A14A-439185962DC8"
#define MSN_CAM_REQUEST_GUID "1C9AA97E-9C05-4583-A3BD-908A196F1E92"
#define MSN_FT_GUID "5D3E02AB-6190-11D3-BBBB-00C04F795683"
#define MSN_OBJ_GUID "A4268EEC-FEC5-49E5-95C3-F126696BDBF6"

#define MSN_CLIENTINFO \
	"Client-Name: Purple/" VERSION "\r\n" \
	"Chat-Logging: Y\r\n"

/* Index into attention_types */
#define MSN_NUDGE 0

#define MSN_CLIENT_ID_VERSION      MSN_CLIENT_VER_9_0
#define MSN_CLIENT_ID_CAPABILITIES (MSN_CAP_PACKET|MSN_CAP_INK_GIF|MSN_CAP_VOICEIM)
#define MSN_CLIENT_ID_EXT_CAPS     (0)

#define MSN_CLIENT_ID \
	((MSN_CLIENT_ID_VERSION    << 24) | \
	 (MSN_CLIENT_ID_CAPABILITIES))

void
msn_set_public_alias(PurpleConnection *gc, const char *alias,
                     PurpleSetPublicAliasSuccessCallback success_cb,
                     PurpleSetPublicAliasFailureCallback failure_cb);
void msn_send_privacy(PurpleConnection *gc);
void msn_send_im_message(MsnSession *session, MsnMessage *msg);

#endif /* MSN_H */
