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

#import <libpurple/libpurple.h>
#import <Adium/AIGroupChat.h>
#import <Adium/AIContentTyping.h>
#import <Adium/AIContentNotification.h>
#import <AdiumLibpurple/PurpleCommon.h>
#import <AdiumLibpurple/CBPurpleAccount.h>

@class AIChat, AIListContact, CBPurpleAccount;

/*!
 * @class SLPurpleCocoaAdapter
 * Singleton to run libpurple from a Cocoa event loop.
 **/
@interface SLPurpleCocoaAdapter : NSObject {

}

+ (SLPurpleCocoaAdapter *)sharedInstance;
+ (void)pluginDidLoad;
+ (NSArray *)libpurplePluginArray;

- (void)addAdiumAccount:(CBPurpleAccount *)adiumAccount;
- (void)removeAdiumAccount:(CBPurpleAccount *)adiumAccount;

- (void)sendNotificationOfType:(AINotificationType)type
				   fromAccount:(id)sourceAccount
						inChat:(AIChat *)chat;
- (void)sendEncodedMessage:(NSString *)encodedMessage
			   fromAccount:(id)sourceAccount
					inChat:(AIChat *)chat
				 withFlags:(PurpleMessageFlags)flags;
- (BOOL)attemptPurpleCommandOnMessage:(NSString *)originalMessage
						fromAccount:(AIAccount *)sourceAccount 
							 inChat:(AIChat *)chat;

- (void)sendTyping:(AITypingState)typingState inChat:(AIChat *)chat;

- (void)addUID:(NSString *)objectUID onAccount:(id)adiumAccount toGroup:(NSString *)groupName withAlias:(NSString *)alias;
- (void)removeUID:(NSString *)objectUID onAccount:(id)adiumAccount fromGroup:(NSString *)groupName;
- (void)moveUID:(NSString *)objectUID onAccount:(id)adiumAccount fromGroups:(NSSet *)groupNames toGroups:(NSSet *)groupNames withAlias:(NSString *)alias;
- (void)renameGroup:(NSString *)oldGroupName onAccount:(id)adiumAccount to:(NSString *)newGroupName;
- (void)deleteGroup:(NSString *)groupName onAccount:(id)adiumAccount;

- (void)setAlias:(NSString *)alias forUID:(NSString *)UID onAccount:(id)adiumAccount;

- (void)openChat:(AIChat *)chat onAccount:(id)adiumAccount;
- (void)inviteContact:(AIListContact *)contact toChat:(AIChat *)chat withMessage:(NSString *)inviteMessage;
- (BOOL)contact:(AIListContact *)inContact isIgnoredInChat:(AIChat *)inChat;
- (void)setContact:(AIListContact *)inContact ignored:(BOOL)inIgnored inChat:(AIChat *)inChat;

- (void)closeChat:(AIChat *)chat;
- (void)disconnectAccount:(id)adiumAccount;
- (void)registerAccount:(id)adiumAccount;
- (void)unregisterAccount:(id)adiumAccount;
- (void)xferRequest:(PurpleXfer *)xfer;
- (void)xferRequestAccepted:(PurpleXfer *)xfer withFileName:(NSString *)xferFileName;
- (void)xferRequestRejected:(PurpleXfer *)xfer;
- (void)xferCancel:(PurpleXfer *)xfer;
- (void)getInfoFor:(NSString *)inUID onAccount:(id)adiumAccount;

- (void)setStatusID:(const char *)statusID isActive:(NSNumber *)isActive arguments:(NSMutableDictionary *)arguments onAccount:(id)adiumAccount;
- (void)setSongInformation:(NSDictionary *)arguments onAccount:(id)adiumAccount;
- (void)setInfo:(NSString *)profileHTML onAccount:(id)adiumAccount;
- (void)setBuddyIcon:(NSData *)buddyImageData onAccount:(id)adiumAccount;
- (void)setIdleSinceTo:(NSDate *)idleSince onAccount:(id)adiumAccount;

- (void)setCheckMail:(NSNumber *)checkMail forAccount:(id)adiumAccount;
- (void)setDefaultPermitDenyForAccount:(id)adiumAccount;

- (void)OSCAREditComment:(NSString *)comment forUID:(NSString *)inUID onAccount:(id)adiumAccount;
- (void)OSCARSetFormatTo:(NSString *)inFormattedUID onAccount:(id)adiumAccount;

- (void)displayFileSendError;
- (void *)handleNotifyMessageOfType:(PurpleNotifyMsgType)type withTitle:(const char *)title primary:(const char *)primary secondary:(const char *)secondary;
- (void *)handleNotifyFormattedWithTitle:(const char *)title primary:(const char *)primary secondary:(const char *)secondary text:(const char *)text;

- (void)performContactMenuActionFromDict:(NSDictionary *)dict forAccount:(id)adiumAccount;
- (void)performAccountMenuActionFromDict:(NSDictionary *)dict forAccount:(id)adiumAccount;

- (void)doAuthRequestCbValue:(NSValue *)inCallBackValue withUserDataValue:(NSValue *)inUserDataValue;
- (void)closeAuthRequestWithHandle:(id)authRequestHandle;
- (BOOL)doCommand:(NSString *)originalMessage fromAccount:(id)sourceAccount inChat:(AIChat *)chat;

#ifdef HAVE_CDSA
- (CFArrayRef)copyServerCertificates:(PurpleSslConnection*)gsc;
- (NSDictionary *)getCipherDetails:(PurpleSslConnection*)gsc;
#endif
@end

//Lookup functions
void *adium_purple_get_handle(void);
PurpleConversation *existingConvLookupFromChat(AIChat *chat);
PurpleConversation *convLookupFromChat(AIChat *chat, id adiumAccount);
AIChat *imChatLookupFromConv(PurpleConversation *conv);
AIGroupChat *groupChatLookupFromConv(PurpleConversation *conv);
AIChat *existingChatLookupFromConv(PurpleConversation *conv);
AIChat *chatLookupFromConv(PurpleConversation *conv);
AIListContact *contactLookupFromIMConv(PurpleConversation *conv);
AIListContact *contactLookupFromBuddy(PurpleBuddy *buddy);
PurpleAccount *accountLookupFromAdiumAccount(CBPurpleAccount *adiumAccount);
CBPurpleAccount *accountLookup(PurpleAccount *acct);

//Images
NSString* processPurpleImages(NSString* inString, AIAccount* adiumAccount);

@interface NSObject (SLPurpleCocoaAdapterOptionalAccountMethods)

- (void)purpleAccountRegistered:(BOOL)success;

@end
