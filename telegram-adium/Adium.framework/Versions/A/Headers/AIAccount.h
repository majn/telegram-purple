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

#import <Adium/AIListObject.h>

@class AIListContact, AIChat, AIContentObject, ESFileTransfer, AIStatus, AIContentMessage, AIContentNotification, AIContentTyping;

#define GROUP_ACCOUNT_STATUS   @"Account Status"

//All keys below are for NSString objects
#define KEY_CONNECT_HOST 			@"Connect Host"
#define KEY_DEFAULT_USER_ICON		@"Default User Icon"
#define KEY_USE_USER_ICON			@"Use User Icon"

//NSNumber objects
#define KEY_CONNECT_PORT 			@"Connect Port"
#define KEY_ACCOUNT_CHECK_MAIL		@"Check Mail"
#define KEY_ENABLED					@"Enabled"
#define KEY_AUTOCONNECT				@"AutoConnect"
#define KEY_ACCOUNT_REGISTER_ON_CONNECT @"Register on Connect"

//NSData archive of an NSAttributedString
#define KEY_ACCOUNT_DISPLAY_NAME	@"FullNameAttr"

#define	Adium_RequestImmediateDynamicContentUpdate			@"Adium_RequestImmediateDynamicContentUpdate"

//Proxy
#define KEY_ACCOUNT_PROXY_ENABLED		@"Proxy Enabled"
#define KEY_ACCOUNT_PROXY_TYPE			@"Proxy Type"
#define KEY_ACCOUNT_PROXY_HOST			@"Proxy Host"
#define KEY_ACCOUNT_PROXY_PORT			@"Proxy Port"
#define KEY_ACCOUNT_PROXY_USERNAME		@"Proxy Username"
#define KEY_ACCOUNT_PROXY_PASSWORD		@"Proxy Password"

//Proxy types
typedef enum
{
	Adium_Proxy_HTTP = 0,
	Adium_Proxy_SOCKS4,
	Adium_Proxy_SOCKS5,
	Adium_Proxy_Default_HTTP,
	Adium_Proxy_Default_SOCKS4,
	Adium_Proxy_Default_SOCKS5,
	Adium_Proxy_None,
    Adium_Proxy_Tor
} AdiumProxyType;

//Privacy
typedef enum {
    AIPrivacyTypePermit = 0,
    AIPrivacyTypeDeny
}  AIPrivacyType;

typedef enum {
    AIPrivacyOptionAllowAll = 1,		//Anyone can conctact you
	AIPrivacyOptionDenyAll,				//Nobody can contact you
	AIPrivacyOptionAllowUsers,			//Only those on your allow list can contact you
	AIPrivacyOptionDenyUsers,			//Those on your deny list can't contact you
	AIPrivacyOptionAllowContactList,	//Only those on your contact list can contact you
	AIPrivacyOptionUnknown,			//used by the privacy settings window, but could probably also be used by accounts
	AIPrivacyOptionCustom				//used by the privacy settings window
} AIPrivacyOption;

typedef enum {
	AIAuthorizationNoResponse = 0,
	AIAuthorizationDenied,
	AIAuthorizationAllowed
} AIAuthorizationResponse;

typedef enum {
	AIAccountGroupDeletionShouldRemoveContacts = 0,
	AIAccountGroupDeletionShouldIgnoreContacts
} AIAccountGroupDeletionResponse;

//Support for file transfer
@protocol AIAccount_Files
	//can the account send entire folders on its own?
	- (BOOL)canSendFolders;

    //Instructs the account to accept a file transfer request
    - (void)acceptFileTransferRequest:(ESFileTransfer *)fileTransfer;

    //Instructs the account to reject a file receive request
    - (void)rejectFileReceiveRequest:(ESFileTransfer *)fileTransfer;

    //Instructs the account to initiate sending of a file
	- (void)beginSendOfFileTransfer:(ESFileTransfer *)fileTransfer;

	//Instructs the account to cancel a filet ransfer in progress
	- (void)cancelFileTransfer:(ESFileTransfer *)fileTransfer;
@end

/*!
 * @protocol AIAccount_Privacy
 * @brief Support for privacy settings
 *
 * An AIAccount subclass may implement this protocol to support privacy settings
 */
@protocol AIAccount_Privacy
    /*!
	 * @brief Add a list object to a privacy list
	 *
	 * @param inObject The object
	 * @param type Either AIPrivacyTypePermit or AIPrivacyTypeDeny
	 *
	 * @result Boolean success
	 */
    -(BOOL)addListObject:(AIListObject *)inObject toPrivacyList:(AIPrivacyType)type;

	/*!
	 * @brief Remove a list object from a privacy list
	 *
	 * @param inObject The object
	 * @param type Either AIPrivacyTypePermit or AIPrivacyTypeDeny
	 *
	 * @result Boolean success
	 */
	-(BOOL)removeListObject:(AIListObject *)inObject fromPrivacyList:(AIPrivacyType)type;

	/*!
	 * @brief Get the contacts on the specified privacy list
	 *
	 * @result An NSArray of AIListContacts, or an empty array if no contacts are on the requested list
	 */
	-(NSArray *)listObjectsOnPrivacyList:(AIPrivacyType)type;
    
	/*!
	 * @brief Set the privacy options
	 *
	 * @param option An AIPrivacyOption value
	 */
    -(void)setPrivacyOptions:(AIPrivacyOption)option;

	/*!
	 * @brief Get the privacy options
	 *
	 * @result An AIPrivacyOption value
	 */
	-(AIPrivacyOption)privacyOptions;
@end

@class AdiumAccounts;
@protocol AIAccountControllerRemoveConfirmationDialog;

/*!
 * @class AIAccount
 * @brief An account of ours (one we connect to and use to talk to handles)
 * 
 * AIAccount is effectively an abstract superclass, as it can do nothing useful on its own.
 * Subclasses of AIAccount, however, inherit much power. With great power comes great responsibility.
 */
@interface AIAccount : AIListObject {
	NSString							*password;
	BOOL										silentAndDelayed;				//We are waiting for and processing our sign on updates
	BOOL										disconnectedByFastUserSwitch;	//We are offline because of a fast user switch
	BOOL										namesAreCaseSensitive;
	BOOL										isTemporary;
	BOOL										enabled;
	
	int										reconnectAttemptsPerformed;
	NSString							*lastDisconnectionError;

	//Attributed string refreshing
	NSTimer								*attributedRefreshTimer;
	NSMutableSet				*autoRefreshingKeys;
	NSMutableSet				*dynamicKeys;
	
	//Contact update guarding
	NSTimer								*delayedUpdateStatusTimer;
	NSMutableArray			*delayedUpdateStatusTargets;
	NSTimer								*silenceAllContactUpdatesTimer;
	
	// Former properties	
	NSString				*formattedUID;
	
	AIStatus				*accountStatus;
	NSDate					*waitingToReconnect;

	NSString				*connectionProgressString;
	
	NSString				*currentDisplayName;
	
	id<AIAccountControllerRemoveConfirmationDialog> confirmationDialogForAccountDeletion;
}

- (void)initAccount;
- (void)connect;
- (void)disconnect;
- (void)disconnectFromDroppedNetworkConnection;
- (void)performRegisterWithPassword:(NSString *)inPassword;
- (NSString *)accountWillSetUID:(NSString *)proposedUID;
- (void)didChangeUID;
- (void)willBeDeleted;
- (id<AIAccountControllerRemoveConfirmationDialog>)confirmationDialogForAccountDeletion __attribute__((objc_method_family(new)));
@property (unsafe_unretained, readonly, nonatomic) id<AIAccountControllerRemoveConfirmationDialog> confirmationDialogForAccountDeletion;
@property (weak, readonly, nonatomic) NSAlert *alertForAccountDeletion;
- (void)alertForAccountDeletion:(id<AIAccountControllerRemoveConfirmationDialog>)dialog didReturn:(NSInteger)returnCode;
@property (weak, readonly, nonatomic) NSString *explicitFormattedUID;
@property (readonly, nonatomic) BOOL useHostForPasswordServerName;
@property (readonly, nonatomic) BOOL useInternalObjectIDForPasswordName;

//Properties
@property (readonly, nonatomic) BOOL supportsAutoReplies;
@property (readonly, nonatomic) BOOL disconnectOnFastUserSwitch;
@property (readonly, nonatomic) BOOL connectivityBasedOnNetworkReachability;
@property (readonly, nonatomic) BOOL suppressTypingNotificationChangesAfterSend;
- (BOOL) canSendOfflineMessageToContact:(AIListContact *)inContact;
- (BOOL) maySendMessageToInvisibleContact:(AIListContact *)inContact;
@property (readonly, nonatomic) BOOL sendOfflineMessagesWithoutPrompting;
@property (readonly, nonatomic) BOOL accountDisplaysFileTransferMessages;
@property (readonly, nonatomic) BOOL managesOwnContactIconCache;
//Temporary Accounts
@property (readwrite, nonatomic) BOOL isTemporary;

//Status
@property (weak, readonly, nonatomic) NSSet *supportedPropertyKeys;
- (id)statusForKey:(NSString *)key;
- (void)updateStatusForKey:(NSString *)key;
- (void)delayedUpdateContactStatus:(AIListContact *)inContact;
- (float)delayedUpdateStatusInterval;
- (void)setStatusState:(AIStatus *)statusState usingStatusMessage:(NSAttributedString *)statusMessage;
- (void)setSocialNetworkingStatusMessage:(NSAttributedString *)statusMessage;
- (BOOL)shouldUpdateAutorefreshingAttributedStringForKey:(NSString *)inKey;

//Messaging, Chatting, Strings
- (BOOL)availableForSendingContentType:(NSString *)inType toContact:(AIListContact *)inContact;
- (BOOL)openChat:(AIChat *)chat;
- (BOOL)closeChat:(AIChat *)chat;
- (BOOL)inviteContact:(AIListObject *)contact toChat:(AIChat *)chat withMessage:(NSString *)inviteMessage;
- (void)sendTypingObject:(AIContentTyping *)inTypingObject;
- (BOOL)sendMessageObject:(AIContentMessage *)inMessageObject;
- (NSString *)encodedAttributedString:(NSAttributedString *)inAttributedString forListObject:(AIListObject *)inListObject;
- (NSString *)encodedAttributedStringForSendingContentMessage:(AIContentMessage *)inContentMessage;
- (BOOL)rejoinChat:(AIChat*)chat;
- (BOOL)groupChatsSupportTopic;
- (void)setTopic:(NSString *)topic forChat:(AIChat *)chat;
- (BOOL)shouldDisplayOutgoingMUCMessages;
/*!
 * @brief Should an autoreply be sent to this message?
 *
 * This will only be called if the generic algorithm determines that an autoreply is appropriate. The account
 * gets an opportunity to suppress sending the autoreply, e.g. on the basis of the message's content or source.
 */
- (BOOL)shouldSendAutoreplyToMessage:(AIContentMessage *)message;

//Presence Tracking
@property (readonly, nonatomic) BOOL contactListEditable;
- (void)addContact:(AIListContact *)contact toGroup:(AIListGroup *)group;
- (void)removeContacts:(NSArray *)objects fromGroups:(NSArray *)groups;
- (void)deleteGroup:(AIListGroup *)group;
- (void)moveListObjects:(NSArray *)objects fromGroups:(NSSet *)oldGroups toGroups:(NSSet *)groups;
- (void)renameGroup:(AIListGroup *)group to:(NSString *)newName;
- (BOOL)isContactIntentionallyListed:(AIListContact *)contact;

- (NSData *)serversideIconDataForContact:(AIListContact *)contact;

//Contact-specific menu items
- (NSArray *)menuItemsForContact:(AIListContact *)inContact;

//Chat-specific menu items
- (NSArray *)menuItemsForChat:(AIChat *)inChat;

//Account-specific menu items
@property (weak, readonly, nonatomic) NSArray *accountActionMenuItems;
- (void)accountMenuDidUpdate:(NSMenuItem*)menuItem;

//Secure messaging
- (BOOL)allowSecureMessagingTogglingForChat:(AIChat *)inChat;
@property (weak, readonly, nonatomic) NSString *aboutEncryption;
- (void)requestSecureMessaging:(BOOL)inSecureMessaging
						inChat:(AIChat *)inChat;

/*!
 * @brief Can the account send images inline within a chat?
 */
- (BOOL)canSendImagesForChat:(AIChat *)inChat;

/*!
 * @brief Should the chat autocomplete the UID instead of the Display Name?
 */
- (BOOL)chatShouldAutocompleteUID:(AIChat *)inChat;

/*!
 * @brief Suffix for autocompleted contacts
 */
- (NSString *)suffixForAutocomplete:(AIChat *)inChat forPartialWordRange:(NSRange)charRange;

/*!
 * @brief Prefix for autocompleted contacts
 */
- (NSString *)prefixForAutocomplete:(AIChat *)inChat forPartialWordRange:(NSRange)charRange;

/*!
 * @brief Does the account support sending notifications?
 */
- (BOOL)supportsSendingNotifications;
- (BOOL)sendNotificationObject:(AIContentNotification *)inContentNotification;

/*!
 * @brief An authorization prompt closed, granting or denying a contact's request for authorization
 *
 * @param inWindowController The window controller which closed; an account may have kept track of what windows were showing its authorization prompts
 * @param infoDict A dictionary of authorization information created by the account originally and possibly modified
 * @param authorizationResponse An AIAuthorizationResponse indicating if authorization was granted or denied or if there was no response
 */
- (void)authorizationWithDict:(NSDictionary *)__attribute__((ns_consumed))infoDict response:(AIAuthorizationResponse)authorizationResponse;

-(NSMenu*)actionMenuForChat:(AIChat*)chat;

- (BOOL)accountManagesGroupChatIgnore;
- (BOOL)contact:(AIListContact *)inContact isIgnoredInChat:(AIChat *)chat;
- (void)setContact:(AIListContact *)inContact ignored:(BOOL)inIgnored inChat:(AIChat *)chat;

/*!
 * @brief Should transcripts be stored for a given chat?
 *
 * Subclasses which intend to return YES should return [super shouldLogChat:chat].
 */
- (BOOL)shouldLogChat:(AIChat *)chat;

/*!
 * @brief The proxy's type used for this account.
 */
- (AdiumProxyType)proxyType;

/*!
 * @brief The proxy's hostname the user entered for this account.
 */
- (NSString *)proxyHost;

@end

@interface AIAccount (Private_ForSubclasses)
- (void)gotFilteredDisplayName:(NSAttributedString *)attributedDisplayName;
- (void)performDelete;
@end

#import <Adium/AIAbstractAccount.h>
