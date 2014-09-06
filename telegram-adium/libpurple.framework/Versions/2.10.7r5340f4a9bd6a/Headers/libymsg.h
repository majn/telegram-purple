/**
 * @file libymsg.h The Yahoo! and Yahoo! JAPAN Protocol Plugins
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

#ifndef _LIBYMSG_H_
#define _LIBYMSG_H_

#include "circbuffer.h"
#include "cmds.h"
#include "prpl.h"
#include "network.h"

#define YAHOO_PAGER_HOST_REQ_URL "http://vcs1.msg.yahoo.com/capacity"
#define YAHOO_PAGER_HOST_FALLBACK "scsa.msg.yahoo.com"
#define YAHOO_PAGER_PORT 5050
#define YAHOO_PAGER_PORT_P2P 5101
#define YAHOO_LOGIN_URL "https://login.yahoo.com/config/pwtoken_login?src=ymsgr&ts=&token=%s"
#define YAHOO_TOKEN_URL "https://login.yahoo.com/config/pwtoken_get?src=ymsgr&ts=&login=%s&passwd=%s&chal=%s"
#define YAHOO_P2P_KEEPALIVE_SECS 300
#define YAHOO_P2P_SERVER_TIMEOUT 10
#define YAHOO_PROFILE_URL "http://profiles.yahoo.com/"
#define YAHOO_MAIL_URL "http://rd.yahoo.com/messenger/client/?http://mail.yahoo.com/"
#define YAHOO_XFER_HOST "filetransfer.msg.yahoo.com"
#define YAHOO_XFER_PORT 80
#define YAHOO_XFER_RELAY_HOST "relay.msg.yahoo.com"
#define YAHOO_XFER_RELAY_PORT 80
#define YAHOO_ROOMLIST_URL "http://insider.msg.yahoo.com/ycontent/"
#define YAHOO_ROOMLIST_LOCALE "us"

/* Yahoo! JAPAN stuff */
#define YAHOOJP_PAGER_HOST_REQ_URL "http://cs1.yahoo.co.jp/capacity"
#define YAHOOJP_TOKEN_URL "https://login.yahoo.co.jp/config/pwtoken_get?src=ymsgr&ts=&login=%s&passwd=%s&chal=%s"
#define YAHOOJP_LOGIN_URL "https://login.yahoo.co.jp/config/pwtoken_login?src=ymsgr&ts=&token=%s"
#define YAHOOJP_PROFILE_URL "http://profiles.yahoo.co.jp/"
#define YAHOOJP_MAIL_URL "http://mail.yahoo.co.jp/"
#define YAHOOJP_XFER_HOST "filetransfer.msg.yahoo.co.jp"
#define YAHOOJP_WEBCAM_HOST "wc.yahoo.co.jp"
/* not sure, must test: */
#define YAHOOJP_XFER_RELAY_HOST "relay.msg.yahoo.co.jp"
#define YAHOOJP_XFER_RELAY_PORT 80
#define YAHOOJP_ROOMLIST_URL "http://insider.msg.yahoo.co.jp/ycontent/"
#define YAHOOJP_ROOMLIST_LOCALE "ja"

#define YAHOO_AUDIBLE_URL "http://l.yimg.com/pu/dl/aud"

#define WEBMESSENGER_URL "http://login.yahoo.com/config/login?.src=pg"

#define YAHOO_SMS_CARRIER_URL "http://validate.msg.yahoo.com"

#define YAHOO_USERINFO_URL "http://address.yahoo.com/yab/us?v=XM&sync=1&tags=short&useutf8=1&noclear=1&legenc=codepage-1252"
#define YAHOOJP_USERINFO_URL "http://address.yahoo.co.jp/yab/jp?v=XM&sync=1&tags=short&useutf8=1&noclear=1&legenc=codepage-1252"

#define YAHOO_PICURL_SETTING "picture_url"
#define YAHOO_PICCKSUM_SETTING "picture_checksum"
#define YAHOO_PICEXPIRE_SETTING "picture_expire"

#define YAHOO_STATUS_TYPE_OFFLINE "offline"
#define YAHOO_STATUS_TYPE_AVAILABLE "available"
#define YAHOO_STATUS_TYPE_BRB "brb"
#define YAHOO_STATUS_TYPE_BUSY "busy"
#define YAHOO_STATUS_TYPE_NOTATHOME "notathome"
#define YAHOO_STATUS_TYPE_NOTATDESK "notatdesk"
#define YAHOO_STATUS_TYPE_NOTINOFFICE "notinoffice"
#define YAHOO_STATUS_TYPE_ONPHONE "onphone"
#define YAHOO_STATUS_TYPE_ONVACATION "onvacation"
#define YAHOO_STATUS_TYPE_OUTTOLUNCH "outtolunch"
#define YAHOO_STATUS_TYPE_STEPPEDOUT "steppedout"
#define YAHOO_STATUS_TYPE_AWAY "away"
#define YAHOO_STATUS_TYPE_INVISIBLE "invisible"
#define YAHOO_STATUS_TYPE_MOBILE "mobile"

#define YAHOO_CLIENT_VERSION_ID "4194239"
#define YAHOO_CLIENT_VERSION "9.0.0.2162"

#define YAHOOJP_CLIENT_VERSION_ID "4186047"
#define YAHOOJP_CLIENT_VERSION "9.0.0.1727"

#define YAHOO_CLIENT_USERAGENT "Mozilla/5.0"
#define YAHOO_CLIENT_USERAGENT_ALIAS "Mozilla/4.0 (compatible; MSIE 5.5)"

/* Index into attention types list. */
#define YAHOO_BUZZ 0

typedef enum {
	YAHOO_PKT_TYPE_SERVER = 0,
	YAHOO_PKT_TYPE_P2P
} yahoo_pkt_type;

typedef enum {
	YAHOO_P2P_WE_ARE_CLIENT =0,
	YAHOO_P2P_WE_ARE_SERVER
} yahoo_p2p_connection_type;

enum yahoo_status {
	YAHOO_STATUS_AVAILABLE = 0,
	YAHOO_STATUS_BRB,
	YAHOO_STATUS_BUSY,
	YAHOO_STATUS_NOTATHOME,
	YAHOO_STATUS_NOTATDESK,
	YAHOO_STATUS_NOTINOFFICE,
	YAHOO_STATUS_ONPHONE,
	YAHOO_STATUS_ONVACATION,
	YAHOO_STATUS_OUTTOLUNCH,
	YAHOO_STATUS_STEPPEDOUT,
	YAHOO_STATUS_P2P = 11,
	YAHOO_STATUS_INVISIBLE = 12,
	YAHOO_STATUS_CUSTOM = 99,
	YAHOO_STATUS_IDLE = 999,
	YAHOO_STATUS_WEBLOGIN = 0x5a55aa55,
	YAHOO_STATUS_OFFLINE = 0x5a55aa56, /* don't ask */
	YAHOO_STATUS_TYPING = 0x16,
	YAHOO_STATUS_DISCONNECTED = 0xffffffff /* in ymsg 15. doesnt mean the normal sense of 'disconnected' */
};

/*
 * Yahoo federated networks.  Key 241 in ymsg.
 * If it doesn't exist, it is on Yahoo's netowrk.
 * It if does exist, send to another IM network.
 */

typedef enum {
	YAHOO_FEDERATION_NONE = 0, /* No federation - Yahoo! network */
	YAHOO_FEDERATION_OCS = 1,  /* LCS or OCS private networks */
	YAHOO_FEDERATION_MSN = 2,  /* MSN or Windows Live network */
	YAHOO_FEDERATION_IBM = 9,  /* IBM/Sametime network */
	YAHOO_FEDERATION_PBX = 100 /* Yahoo! Pingbox service */
} YahooFederation;


struct yahoo_buddy_icon_upload_data {
	PurpleConnection *gc;
	GString *str;
	char *filename;
	int pos;
	int fd;
	guint watcher;
};

struct yahoo_p2p_data	{
	PurpleConnection *gc;
	char *host_ip;
	char *host_username;
	int val_13;
	guint input_event;
	gint source;
	int session_id;
	yahoo_p2p_connection_type connection_type;
};

struct _YchtConn;

typedef struct _YahooPersonalDetails {
	char *id;

	struct {
		char *first;
		char *last;
		char *middle;
		char *nick;
	} names;

	struct {
		char *work;
		char *home;
		char *mobile;
	} phone;
} YahooPersonalDetails;

typedef struct {
	PurpleConnection *gc;
	int fd;
	guchar *rxqueue;
	int rxlen;
	PurpleCircBuffer *txbuf;
	guint txhandler;
	GHashTable *friends;

	char **profiles;  /* Multiple profiles can be associated with an account */
	YahooPersonalDetails ypd;

	/**
	 * This is used to keep track of the IMVironment chosen
	 * by people you talk to.  We don't do very much with
	 * this right now... but at least now if the remote user
	 * selects an IMVironment we won't reset it back to the
	 * default of nothing.
	 */
	GHashTable *imvironments;

	int current_status;
	gboolean logged_in;
	GString *tmp_serv_blist, *tmp_serv_ilist, *tmp_serv_plist;
	GSList *confs;
	unsigned int conf_id; /* just a counter */
	gboolean chat_online;
	gboolean in_chat;
	char *chat_name;
	char *pending_chat_room;
	char *pending_chat_id;
	char *pending_chat_topic;
	char *pending_chat_goto;
	char *auth;
	gsize auth_written;
	char *cookie_y;
	char *cookie_t;
	char *cookie_b;
	int session_id;
	gboolean jp;
	gboolean wm; /* connected w/ web messenger method */
	/* picture aka buddy icon stuff */
	char *picture_url;
	int picture_checksum;

	/* ew. we have to check the icon before we connect,
	 * but can't upload it til we're connected. */
	struct yahoo_buddy_icon_upload_data *picture_upload_todo;
	PurpleProxyConnectData *buddy_icon_connect_data;

	struct _YchtConn *ycht;

	/**
	 * This linked list contains PurpleUtilFetchUrlData structs
	 * for when we lookup people profile or photo information.
	 */
	GSList *url_datas;
	GHashTable *xfer_peer_idstring_map;/* Hey, i dont know, but putting this HashTable next to friends gives a run time fault... */
	GSList *cookies;/* contains all cookies, including _y and _t */
	PurpleNetworkListenData *listen_data;

	/**
	 * We may receive a list15 in multiple packets with no prior warning as to how many we'll be getting;
	 * the server expects us to keep track of the group for which it is sending us contact names.
	 */
	char *current_list15_grp;
	time_t last_ping;
	time_t last_keepalive;
	GHashTable *peers;	/* information about p2p data */
	int yahoo_p2p_timer;
	int yahoo_local_p2p_server_fd;
	int yahoo_p2p_server_watcher;
	GHashTable *sms_carrier;	/* sms carrier data */
	guint yahoo_p2p_server_timeout_handle;
} YahooData;

#define YAHOO_MAX_STATUS_MESSAGE_LENGTH (255)

/*
 * Current Maximum Length for Instant Messages
 *
 * This was found by experiment.
 *
 * The YMSG protocol allows a message of up to 948 bytes, but the official client
 * limits to 800 characters.  According to experiments I conducted, it seems that
 * the discrepancy is to allow some leeway for messages with mixed single- and
 * multi-byte characters, as I was able to send messages of 840 and 932 bytes
 * by using some multibyte characters (some random Chinese or Japanese characters,
 * to be precise). - rekkanoryo
 */
#define YAHOO_MAX_MESSAGE_LENGTH_BYTES 948
#define YAHOO_MAX_MESSAGE_LENGTH_CHARS 800

/* sometimes i wish prpls could #include things from other prpls. then i could just
 * use the routines from libfaim and not have to admit to knowing how they work. */
#define yahoo_put16(buf, data) ( \
		(*(buf) = (unsigned char)((data)>>8)&0xff), \
		(*((buf)+1) = (unsigned char)(data)&0xff),  \
		2)
#define yahoo_get16(buf) ((((*(buf))<<8)&0xff00) + ((*((buf)+1)) & 0xff))
#define yahoo_put32(buf, data) ( \
		(*((buf)) = (unsigned char)((data)>>24)&0xff), \
		(*((buf)+1) = (unsigned char)((data)>>16)&0xff), \
		(*((buf)+2) = (unsigned char)((data)>>8)&0xff), \
		(*((buf)+3) = (unsigned char)(data)&0xff), \
		4)
#define yahoo_get32(buf) ((((*(buf))<<24)&0xff000000) + \
		(((*((buf)+1))<<16)&0x00ff0000) + \
		(((*((buf)+2))<< 8)&0x0000ff00) + \
		(((*((buf)+3)    )&0x000000ff)))

/* util.c */
void yahoo_init_colorht(void);
void yahoo_dest_colorht(void);
char *yahoo_codes_to_html(const char *x);

/**
 * This function takes a normal HTML message and converts it to the message
 * format used by Yahoo, which uses a frankensteinish combination of ANSI
 * escape codes and broken HTML.
 *
 * It results in slightly different output than would be sent by official
 * Yahoo clients.  The two main differences are:
 *
 * 1. We always close all tags, whereas official Yahoo clients leave tags
 *    dangling open at the end of each message (and the client treats them
 *    as closed).
 * 2. We always close inner tags first before closing outter tags.
 *
 * For example, if you want to send this message:
 *   <b> bold <i> bolditalic </i></b><i> italic </i>
 * Official Yahoo clients would send:
 *   ESC[1m bold ESC[2m bolditalic ESC[x1m italic
 * But we will send:
 *   ESC[1m bold ESC[2m bolditalic ESC[x2mESC[x1mESC[2m italic ESC[x2m
 */
char *yahoo_html_to_codes(const char *src);

gboolean
yahoo_account_use_http_proxy(PurpleConnection *conn);

/**
 * Encode some text to send to the yahoo server.
 *
 * @param gc The connection handle.
 * @param str The null terminated utf8 string to encode.
 * @param utf8 If not @c NULL, whether utf8 is okay or not.
 *             Even if it is okay, we may not use it. If we
 *             used it, we set this to @c TRUE, else to
 *             @c FALSE. If @c NULL, false is assumed, and
 *             it is not dereferenced.
 * @return The g_malloced string in the appropriate encoding.
 */
char *yahoo_string_encode(PurpleConnection *gc, const char *str, gboolean *utf8);

/**
 * Decode some text received from the server.
 *
 * @param gc The gc handle.
 * @param str The null terminated string to decode.
 * @param utf8 Did the server tell us it was supposed to be utf8?
 * @return The decoded, utf-8 string, which must be g_free()'d.
 */
char *yahoo_string_decode(PurpleConnection *gc, const char *str, gboolean utf8);

char *yahoo_convert_to_numeric(const char *str);

YahooFederation yahoo_get_federation_from_name(const char *who);

/* yahoo_profile.c */
void yahoo_get_info(PurpleConnection *gc, const char *name);

/* libymsg.h  - these functions were formerly static but need not to be for the
 * new two-prpl model. */
const char *yahoo_list_icon(PurpleAccount *a, PurpleBuddy *b);
const char *yahoo_list_emblem(PurpleBuddy *b);
char *yahoo_status_text(PurpleBuddy *b);
void yahoo_tooltip_text(PurpleBuddy *b, PurpleNotifyUserInfo *user_info, gboolean full);
GList *yahoo_status_types(PurpleAccount *account);
GList *yahoo_blist_node_menu(PurpleBlistNode *node);
void yahoo_login(PurpleAccount *account);
void yahoo_close(PurpleConnection *gc);
int yahoo_send_im(PurpleConnection *gc, const char *who, const char *what, PurpleMessageFlags flags);
unsigned int yahoo_send_typing(PurpleConnection *gc, const char *who, PurpleTypingState state);
void yahoo_set_status(PurpleAccount *account, PurpleStatus *status);
void yahoo_set_idle(PurpleConnection *gc, int idle);
void yahoo_add_buddy(PurpleConnection *gc, PurpleBuddy *buddy, PurpleGroup *g);
void yahoo_remove_buddy(PurpleConnection *gc, PurpleBuddy *buddy, PurpleGroup *group);
void yahoo_add_deny(PurpleConnection *gc, const char *who);
void yahoo_rem_deny(PurpleConnection *gc, const char *who);
void yahoo_set_permit_deny(PurpleConnection *gc);
void yahoo_keepalive(PurpleConnection *gc);
void yahoo_change_buddys_group(PurpleConnection *gc, const char *who, const char *old_group, const char *new_group);
void yahoo_rename_group(PurpleConnection *gc, const char *old_name, PurpleGroup *group, GList *moved_buddies);
gboolean yahoo_offline_message(const PurpleBuddy *buddy);
gboolean yahoo_send_attention(PurpleConnection *gc, const char *username, guint type);
GList *yahoo_attention_types(PurpleAccount *account);

GList *yahoo_actions(PurplePlugin *plugin, gpointer context);
void yahoopurple_register_commands(void);

PurpleCmdRet yahoopurple_cmd_buzz(PurpleConversation *c, const gchar *cmd, gchar **args, gchar **error, void *data);
PurpleCmdRet yahoopurple_cmd_chat_join(PurpleConversation *conv, const char *cmd, char **args, char **error, void *data);
PurpleCmdRet yahoopurple_cmd_chat_list(PurpleConversation *conv, const char *cmd, char **args, char **error, void *data);
/* needed for xfer, thought theyd be useful for other enhancements later on
   Returns list of cookies stored in yahoo_data formatted as a single null terminated string
   returned value must be g_freed
*/
gchar* yahoo_get_cookies(PurpleConnection *gc);

/* send p2p pkt containing our encoded ip, asking peer to connect to us */
void yahoo_send_p2p_pkt(PurpleConnection *gc, const char *who, int val_13);

#endif /* _LIBYMSG_H_ */
