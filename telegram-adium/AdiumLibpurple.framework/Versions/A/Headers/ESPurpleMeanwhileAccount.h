/* 
 * Adium is the legal property of its developers, whose names are listed in the copyright file included
 * with this source distribution.
 * 
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#import "CBPurpleAccount.h"

#define	KEY_MEANWHILE_CONTACTLIST		@"Meanwhile:ContactList"
#define KEY_MEANWHILE_FORCE_LOGIN		@"Meanwhile:ForceLogin"
#define KEY_MEANWHILE_FAKE_CLIENT_ID	@"Meanwhile:FakeClientID"

#define MW_PRPL_OPT_BLIST_ACTION		"/plugins/prpl/meanwhile/blist_action"

enum Meanwhile_CL_Choice {
	Meanwhile_CL_None = 1,
	Meanwhile_CL_Load = 2,
	Meanwhile_CL_Load_And_Save = 3
};

/** the purple plugin data.
available as gc->proto_data and mwSession_getClientData */
struct mwPurplePluginData {
	struct mwSession *session;
	
	struct mwServiceAware *srvc_aware;
	struct mwServiceConference *srvc_conf;
	struct mwServiceDirectory *srvc_dir;
	struct mwServiceFileTransfer *srvc_ft;
	struct mwServiceIm *srvc_im;
	struct mwServiceResolve *srvc_resolve;
	struct mwServiceStorage *srvc_store;
	
	/** map of PurpleGroup:mwAwareList and mwAwareList:PurpleGroup */
	GHashTable *group_list_map;
	
	/** event id for the buddy list save callback */
	guint save_event;
	
	/** socket fd */
	NSInteger socket;
	
	PurpleConnection *gc;
};

/* 8.3.8.2 Awareness Presence Types */

/* @todo move mwAwareType, mwAwareIdBlock and mwAwareSnapshot into the
aware service and out of common */

/** type codes for mwAwareIdBlock */
enum mwAwareType {
	mwAware_USER    = 0x0002,  /**< a single user */
	mwAware_GROUP   = 0x0003,  /**< a group */
	mwAware_SERVER  = 0x0008,  /**< a server */
};

/* 8.4.2 Awareness Messages */
/* 8.4.2.1 Awareness ID Block */

struct mwAwareIdBlock {
	guint16 type;     /**< @see mwAwareType */
	char *user;       /**< user id */
	char *community;  /**< community id (NULL for same community) */
};

@interface ESPurpleMeanwhileAccount : CBPurpleAccount <AIAccount_Files> {

}

@end
