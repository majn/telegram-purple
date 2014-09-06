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

#import <Adium/ESObjectWithProperties.h>
#import <Adium/AIContentTyping.h>
#import <Adium/AIListObject.h>
#import <Adium/AIInterfaceControllerProtocol.h>

@class AIAccount, AIListObject, AIListContact, AIContentObject, AIEmoticon;

#define Chat_OrderDidChange						@"Chat_OrderDidChange"
#define Chat_WillClose							@"Chat_WillClose"
#define Chat_DidOpen							@"Chat_DidOpen"
#define Chat_BecameActive						@"Chat_BecameActive"
#define Chat_AttributesChanged					@"Chat_AttributesChanged"
#define Chat_StatusChanged						@"Chat_StatusChagned"
#define Chat_ParticipatingListObjectsChanged	@"Chat_ParticipatingListObjectsChanged"
#define Chat_SourceChanged 						@"Chat_SourceChanged"
#define Chat_DestinationChanged 				@"Chat_DestinationChanged"

#define KEY_UNVIEWED_CONTENT	@"unviewedContent"
#define KEY_UNVIEWED_MENTION	@"unviewedMention"
#define KEY_TYPING				@"typing"

#define	KEY_CHAT_TIMED_OUT		@"Timed Out"
#define KEY_CHAT_CLOSED_WINDOW	@"Closed Window"

#define KEY_TOPIC @"topic"
#define KEY_TOPIC_SETTER @"topicSetter"

typedef enum {
	AIChatCanNotSendMessage = 0,
	AIChatMayNotBeAbleToSendMessage,
	AIChatCanSendMessageNow,
	AIChatCanSendViaServersideOfflineMessage
} AIChatSendingAbilityType;

#define KEY_ENCRYPTED_CHAT_PREFERENCE	@"Encrypted Chat Preference"
#define GROUP_ENCRYPTION				@"Encryption"

typedef enum {
	EncryptedChat_Default = -2, /* For use by a menu which wants to provide a 'no preference' option */
	EncryptedChat_Never = -1,
	EncryptedChat_Manually = 0, /* Manually is the default */
	EncryptedChat_Automatically = 1, 
	EncryptedChat_RejectUnencryptedMessages = 2
} AIEncryptedChatPreference;

typedef enum {
	EncryptionStatus_None = 0,
	EncryptionStatus_Unverified,
	EncryptionStatus_Verified,
	EncryptionStatus_Finished
} AIEncryptionStatus;

//Chat errors should be indicated by setting a property on this key 
//with an NSNumber of the appropriate error type as its object
#define	KEY_CHAT_ERROR			@"Chat Error"

//This key may be set before sending KEY_CHAT_ERROR to provide any data the
//the error message should make use of.  It may be of any type.
#define	KEY_CHAT_ERROR_DETAILS	@"Chat Error Details"

typedef enum {
	AIChatUnknownError = 0,
	AIChatMessageSendingUserIsBlocked,
	AIChatMessageSendingNotAllowedWhileInvisible,
	AIChatMessageSendingUserNotAvailable,
	AIChatMessageSendingTooLarge,
	AIChatMessageSendingTimeOutOccurred,
	AIChatMessageSendingConnectionError,
	AIChatMessageSendingMissedRateLimitExceeded,
	AIChatMessageReceivingMissedTooLarge,
	AIChatMessageReceivingMissedInvalid,
	AIChatMessageReceivingMissedRateLimitExceeded,
	AIChatMessageReceivingMissedRemoteIsTooEvil,
	AIChatMessageReceivingMissedLocalIsTooEvil,
	AIChatCommandFailed,
	AIChatInvalidNumberOfArguments
} AIChatErrorType;

@interface AIChat : ESObjectWithProperties {
	AIAccount			*account;
	NSDate				*dateOpened;
	BOOL				isOpen;
	BOOL				hasSentOrReceivedContent;

	NSMutableArray		*pendingOutgoingContentObjects;
	
	AIListContact		*__weak preferredContact;
	NSString			*name;
	NSString			*uniqueChatID;
	id					identifier;
	
	NSMutableSet		*ignoredListContacts;
	
	BOOL				hideUserIconAndStatus;
	
	NSMutableSet		*customEmoticons;
	
	// Former properties
	NSDictionary		*chatCreationInfo;

	NSDictionary		*securityDetails;
    
    AIListContact       *_listObject;
	
	NSNumber			*overrideLogging;
}

+ (id)chatForAccount:(AIAccount *)inAccount;
- (id)initForAccount:(AIAccount *)inAccount;

@property (readwrite, nonatomic, strong) AIAccount *account;

@property (readonly, nonatomic) NSDate *dateOpened;
@property (readwrite, nonatomic, strong) NSDictionary *chatCreationDictionary;

@property (readwrite, nonatomic) BOOL isOpen;

@property (readwrite, nonatomic) BOOL hasSentOrReceivedContent;

@property (readonly, nonatomic) NSUInteger unviewedContentCount;
@property (readonly, nonatomic) NSUInteger unviewedMentionCount;

- (void)incrementUnviewedContentCount;
- (void)incrementUnviewedMentionCount;

- (void)clearUnviewedContentCount;

- (void)setDisplayName:(NSString *)inDisplayName;

//
@property (readwrite, nonatomic, strong) AIListContact *listObject;
@property (readwrite, nonatomic, weak) AIListContact *preferredListObject;

- (BOOL)shouldBeginSendingContentObject:(AIContentObject *)inObject;
- (void)finishedSendingContentObject:(AIContentObject *)inObject;

@property (readwrite, nonatomic, strong) NSString *name;
@property (readwrite, nonatomic, strong) id identifier;

@property (weak, readonly, nonatomic) NSString *uniqueChatID;

@property (unsafe_unretained, readonly, nonatomic) NSImage *chatImage;
@property (unsafe_unretained, readonly, nonatomic) NSImage *chatMenuImage;

@property (readwrite, nonatomic, strong) NSDictionary *securityDetails;
@property (readonly, nonatomic) BOOL isSecure;
@property (readonly, nonatomic) AIEncryptionStatus encryptionStatus;
@property (readonly, nonatomic) BOOL supportsSecureMessagingToggling;

@property (readonly, nonatomic) AIChatSendingAbilityType messageSendingAbility;
@property (readonly, nonatomic) BOOL canSendImages;

- (BOOL)isListContactIgnored:(AIListObject *)inContact;
- (void)setListContact:(AIListContact *)inContact isIgnored:(BOOL)isIgnored;

- (void)addCustomEmoticon:(AIEmoticon *)inEmoticon;
@property (readonly, nonatomic) NSMutableSet *customEmoticons;

- (void)receivedError:(NSNumber *)type;

@property (readonly, nonatomic) id <AIChatContainer> chatContainer;

@property (readonly, nonatomic) NSMenu *actionMenu;

@property (readonly, nonatomic) BOOL shouldLog;

@property (readwrite, nonatomic) BOOL hideUserIconAndStatus;

// Compatibility. I don't like this here.
- (NSArray *)containedObjects;
- (BOOL)isGroupChat;

@end
