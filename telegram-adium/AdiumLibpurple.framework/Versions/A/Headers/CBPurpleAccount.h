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

#import <Adium/AIAccount.h>
#import <Adium/AIGroupChat.h>
#import <AdiumLibpurple/PurpleCommon.h>

#define KEY_DISPLAY_CUSTOM_EMOTICONS	@"Display Custom Emoticons"
#define KEY_BROADCAST_MUSIC_INFO		@"Broadcast Music Information"

#define KEY_KEY		@"Key"
#define KEY_VALUE	@"Value"
#define KEY_TYPE	@"Type"

@class SLPurpleCocoaAdapter, ESFileTransfer, AIService, AIContentMessage, AIStatus, AIWindowController;
@protocol AIAccountControllerRemoveConfirmationDialog;

@interface CBPurpleAccount : AIAccount <AIAccount_Privacy>
{   	
    PurpleAccount         *account;

	NSMutableArray		*permittedContactsArray;
	NSMutableArray		*deniedContactsArray;	
	
	NSDictionary		*tuneinfo;
	
	BOOL				unregisterAfterConnecting;
	BOOL				deletePurpleAccountAfterDisconnecting;
	BOOL				finishedConnectProcess;
	BOOL				openPsychicChats;
	BOOL				shouldIncludeNowPlayingInformationInAllStatuses;

	PurpleConnectionError lastDisconnectionReason;
}

- (const char*)protocolPlugin;
@property (assign, readwrite) PurpleAccount *purpleAccount;
- (PurplePluginProtocolInfo *)protocolInfo;
- (const char *)purpleAccountName;

- (void)createNewPurpleAccount;

- (void)dealloc;
- (NSDictionary *)defaultProperties;
- (NSString *)unknownGroupName;
- (AIService *)_serviceForUID:(NSString *)contactUID;
- (void)unregisteredAccount:(BOOL)success;

#pragma mark Subclass-reuse but not declared
	//Instructs the account to accept a file transfer request
- (void)acceptFileTransferRequest:(ESFileTransfer *)fileTransfer;
	//Instructs the account to reject a file receive request
- (void)rejectFileReceiveRequest:(ESFileTransfer *)fileTransfer;
	//Instructs the account to cancel a file transfer in progress
- (void)cancelFileTransfer:(ESFileTransfer *)fileTransfer;

#pragma mark -

	//Private (for subclasses only) file transfer methods
- (PurpleXfer *)newOutgoingXferForFileTransfer:(ESFileTransfer *)fileTransfer;
- (void)_beginSendOfFileTransfer:(ESFileTransfer *)fileTransfer;

	//AIAccount_Privacy
-(BOOL)addListObject:(AIListObject *)inObject toPrivacyList:(AIPrivacyType)type;
-(BOOL)removeListObject:(AIListObject *)inObject fromPrivacyList:(AIPrivacyType)type;
-(NSArray *)listObjectsOnPrivacyList:(AIPrivacyType)type;

	//Connectivity
- (void)connect;
- (void)configureAccountProxyNotifyingTarget:(id)target selector:(SEL)selector;
- (void)disconnect;
- (PurpleConnectionError)lastDisconnectionReason;
- (NSString *)connectionStringForStep:(NSInteger)step;
- (void)configurePurpleAccount;
- (PurpleSslConnection *)secureConnection;

//Account status
- (NSSet *)supportedPropertyKeys;
- (void)updateStatusForKey:(NSString *)key;
- (void)setAccountIdleSinceTo:(NSDate *)idleSince;

- (void)setStatusState:(AIStatus *)statusState statusID:(const char *)statusID isActive:(NSNumber *)isActive arguments:(NSMutableDictionary *)arguments;
- (const char *)purpleStatusIDForStatus:(AIStatus *)statusState
							arguments:(NSMutableDictionary *)arguments;

- (void)setAccountProfileTo:(NSAttributedString *)profile;

- (BOOL)shouldSetAliasesServerside;

- (SLPurpleCocoaAdapter *)purpleAdapter;

#pragma mark Purple callback handling methods
- (void)accountConnectionConnected;
- (void)accountConnectionReportDisconnect:(NSString *)text withReason:(PurpleConnectionError)reason;
- (void)accountConnectionNotice:(NSString *)text;
- (void)accountConnectionDisconnected;
- (void)accountConnectionProgressStep:(NSNumber *)step percentDone:(NSNumber *)connectionProgressPrecent;

- (void)newContact:(AIListContact *)theContact withName:(NSString *)inName;
- (void)addContact:(AIListContact *)theContact
				 toGroupName:(NSString *)groupName
				 contactName:(NSString *)contactName;
- (void)removeContact:(AIListContact *)theContact fromGroupName:(NSString *)groupName;
- (void)updateContact:(AIListContact *)theContact toAlias:(NSString *)purpleAlias;
- (void)updateContact:(AIListContact *)theContact forEvent:(NSNumber *)event;
- (void)updateSignon:(AIListContact *)theContact withData:(void *)data;
- (void)updateSignoff:(AIListContact *)theContact withData:(void *)data;
- (void)updateSignonTime:(AIListContact *)theContact withData:(NSDate *)signonDate;
- (void)updateStatusForContact:(AIListContact *)theContact
				  toStatusType:(NSNumber *)statusTypeNumber
					statusName:(NSString *)statusName
				 statusMessage:(NSAttributedString *)statusMessage
					  isMobile:(BOOL)isMobile;
- (NSString *)statusNameForPurpleBuddy:(PurpleBuddy *)b;
- (NSAttributedString *)statusMessageForPurpleBuddy:(PurpleBuddy *)b;
- (void)updateEvil:(AIListContact *)theContact withData:(NSNumber *)evilNumber;
- (void)updateIcon:(AIListContact *)theContact withData:(NSData *)userIconData;

- (void)removeContact:(AIListContact *)theContact;

- (NSWindowController *)authorizationRequestWithDict:(NSDictionary*)dict __attribute__((ns_returns_retained));

- (void)addChat:(AIChat *)chat;
- (void)typingUpdateForIMChat:(AIChat *)chat typing:(NSNumber *)typing;
- (AIChat *)chatWithContact:(AIListContact *)contact identifier:(id)identifier;
- (void)chatWasDestroyed:(AIChat *)chat;
- (void)chatJoinDidFail:(AIChat *)chat;

- (void)receivedEventForChat:(AIChat *)chat
					 message:(NSString *)message
						date:(NSDate *)date
					   flags:(NSNumber *)flagsNumber;
- (void)receivedIMChatMessage:(NSDictionary *)messageDict inChat:(AIChat *)chat;
- (void)receivedMultiChatMessage:(NSDictionary *)messageDict inChat:(AIChat *)chat;
- (void)leftChat:(AIChat *)chat;

- (void)requestReceiveOfFileTransfer:(ESFileTransfer *)fileTransfer;
- (void)updateProgressForFileTransfer:(ESFileTransfer *)fileTransfer 
									 percent:(NSNumber *)percent
								   bytesSent:(NSNumber *)bytesSent;
- (void)fileTransferCancelledRemotely:(ESFileTransfer *)fileTransfer;
- (void)fileTransferCancelledLocally:(ESFileTransfer *)fileTransfer;
- (void)destroyFileTransfer:(ESFileTransfer *)fileTransfer;
- (ESFileTransfer *)newFileTransferObjectWith:(NSString *)destinationUID
										 size:(unsigned long long)inSize
							   remoteFilename:(NSString *)remoteFilename __attribute__((objc_method_family(none)));

- (BOOL)allowFileTransferWithListObject:(AIListObject *)inListObject;
- (BOOL)canSendFolders;

- (AIGroupChat *)chatWithName:(NSString *)name identifier:(id)identifier;
- (void)requestAddContactWithUID:(NSString *)contactUID;

- (void)gotGroupForContact:(AIListContact *)contact;

- (NSString *)encodedAttributedString:(NSAttributedString *)inAttributedString 
					   forStatusState:(AIStatus *)statusState;
- (BOOL)inviteContact:(AIListContact *)contact toChat:(AIChat *)chat withMessage:(NSString *)inviteMessage;

- (NSString *)titleForContactMenuLabel:(const char *)label forContact:(AIListContact *)inContact;
- (NSString *)titleForAccountActionMenuLabel:(const char *)label;

- (NSString *)_UIDForAddingObject:(AIListContact *)object;

#pragma mark Contacts
- (void)setAttribute:(NSString *)name value:(NSString *)value forContact:(AIListContact *)contact;
- (void)renameContact:(AIListContact *)theContact toUID:(NSString *)newUID;
- (void)updateWentIdle:(AIListContact *)theContact withData:(NSDate *)idleSinceDate;
- (void)updateIdleReturn:(AIListContact *)theContact withData:(void *)data;
- (NSMutableArray *)arrayOfDictionariesFromPurpleNotifyUserInfo:(PurpleNotifyUserInfo *)user_info forContact:(AIListContact *)contact;
- (void)updateUserInfo:(AIListContact *)theContact withData:(PurpleNotifyUserInfo *)user_info;
- (void)openInspectorForContactInfo:(AIListContact *)theContact;

#pragma mark Chats
- (void)errorForChat:(AIChat *)chat type:(NSNumber *)type;
- (void)removeUsersArray:(NSArray *)usersArray fromChat:(AIGroupChat *)chat;
- (void)updateTopic:(NSString *)inTopic forChat:(AIGroupChat *)chat withSource:(NSString *)source;
- (void)updateTitle:(NSString *)inTitle forChat:(AIGroupChat *)chat;
- (void)convUpdateForChat:(AIGroupChat *)chat type:(NSNumber *)type;
- (void)renameParticipant:(NSString *)oldName newNick:(NSString *)newName newUID:(NSString *)newUID flags:(PurpleConvChatBuddyFlags)flags inChat:(AIGroupChat *)chat;
- (void)removeUser:(NSString *)contactName fromChat:(AIGroupChat *)chat;
- (void)updateUserListForChat:(AIGroupChat *)chat users:(NSArray *)users newlyAdded:(BOOL)newlyAdded;
- (void)updateUser:(NSString *)user forChat:(AIGroupChat *)chat flags:(PurpleConvChatBuddyFlags)flags newAlias:(NSString *)alias attributes:(NSDictionary *)attributes;
- (NSDictionary *)willJoinChatUsingDictionary:(NSDictionary *)chatCreationDictionary;
- (BOOL)chatCreationDictionary:(NSDictionary *)chatCreationDict isEqualToDictionary:(NSDictionary *)baseDict;
- (NSDictionary *)extractChatCreationDictionaryFromConversation:(PurpleConversation *)conv;

#pragma mark Privacy
- (void)privacyPermitListAdded:(NSString *)sourceUID;
- (void)privacyDenyListAdded:(NSString *)sourceUID;
- (void)privacyPermitListRemoved:(NSString *)sourceUID;
- (void)privacyDenyListRemoved:(NSString *)sourceUID;

#pragma mark Emoticons
- (void)chat:(AIChat *)inChat isWaitingOnCustomEmoticon:(NSString *)isWaiting;
- (void)chat:(AIChat *)inChat setCustomEmoticon:(NSString *)emoticonEquivalent withImageData:(NSData *)inImageData;
- (void)chat:(AIChat *)inChat closedCustomEmoticon:(NSString *)inEmoticon;

@end

@interface CBPurpleAccount (ForSubclasses)
- (void)continueConnectWithConfiguredPurpleAccount;
- (void)continueConnectWithConfiguredProxy;
@end
