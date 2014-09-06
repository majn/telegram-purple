/**
 * @file notification.h Notification server functions
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
#ifndef MSN_NOTIFICATION_H
#define MSN_NOTIFICATION_H

typedef struct _MsnNotification MsnNotification;

/* MSN protocol challenge info */

/* MSNP18 challenge: WLM Version 2009 (Build 14.0.8089.726) */
#define MSNP18_WLM_PRODUCT_KEY "C1BX{V4W}Q3*10SM"
#define MSNP18_WLM_PRODUCT_ID "PROD0120PW!CCV9@"

/* MSNP15 challenge: WLM 8.5.1288.816 */
#define MSNP15_WLM_PRODUCT_KEY "ILTXC!4IXB5FB*PX"
#define MSNP15_WLM_PRODUCT_ID "PROD0119GSJUC$18"

/* MSNP13 challenge */
#define MSNP13_WLM_PRODUCT_KEY	"O4BG@C7BWLYQX?5G"
#define MSNP13_WLM_PRODUCT_ID	"PROD01065C%ZFN6F"

#define MSNP10_PRODUCT_KEY		"VT6PX?UQTM4WM%YR"
#define MSNP10_PRODUCT_ID		"PROD0038W!61ZTF9"

#include "cmdproc.h"
#include "msg.h"
#include "session.h"
#include "servconn.h"
#include "state.h"
#include "user.h"
#include "userlist.h"

struct _MsnNotification
{
	MsnSession *session;

	/**
	 * This is a convenience pointer that always points to
	 * servconn->cmdproc
	 */
	MsnCmdProc *cmdproc;
	MsnServConn *servconn;

	gboolean in_use;
};

typedef void (*MsnFqyCb)(MsnSession *session, const char *passport, MsnNetwork network, gpointer data);

/* Type used for msn_notification_send_uun */
typedef enum {
	MSN_UNIFIED_NOTIFICATION_SHARED_FOLDERS = 1,
	MSN_UNIFIED_NOTIFICATION_UNKNOWN1 = 2,
	MSN_UNIFIED_NOTIFICATION_P2P = 3,
	MSN_UNIFIED_NOTIFICATION_MPOP = 4

} MsnUnifiedNotificationType;

void msn_notification_end(void);
void msn_notification_init(void);

void msn_notification_add_buddy_to_list(MsnNotification *notification,
					MsnListId list_id, MsnUser *user);
void msn_notification_rem_buddy_from_list(MsnNotification *notification,
					  MsnListId list_id, MsnUser *user);

void msn_notification_send_fqy(MsnSession *session,
                               const char *payload, int payload_len,
                               MsnFqyCb cb, gpointer cb_data);

MsnNotification *msn_notification_new(MsnSession *session);
void msn_notification_destroy(MsnNotification *notification);
gboolean msn_notification_connect(MsnNotification *notification,
				  const char *host, int port);
void msn_notification_disconnect(MsnNotification *notification);
void msn_notification_dump_contact(MsnSession *session);

void msn_notification_send_uum(MsnSession *session, MsnMessage *msg);

void msn_notification_send_uux(MsnSession *session, const char *payload);

void msn_notification_send_uux_endpointdata(MsnSession *session);

void msn_notification_send_uux_private_endpointdata(MsnSession *session);

void msn_notification_send_uun(MsnSession *session,
                               const char *user,
                               MsnUnifiedNotificationType type,
                               const char *payload);

void msn_notification_send_circle_auth(MsnSession *session, const char *ticket);

/**
 * Closes a notification.
 *
 * It's first closed, and then disconnected.
 *
 * @param notification The notification object to close.
 */
void msn_notification_close(MsnNotification *notification);

void msn_got_login_params(MsnSession *session, const char *ticket, const char *response);

#endif /* MSN_NOTIFICATION_H */
