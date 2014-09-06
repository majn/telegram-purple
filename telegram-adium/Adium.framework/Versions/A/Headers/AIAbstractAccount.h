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
#import <Adium/AIPasswordPromptController.h>

typedef enum {
	AIReconnectNever = 0,
	AIReconnectNeverNoMessage,
	AIReconnectImmediately,
	AIReconnectNormally
} AIReconnectDelayType;

@interface AIAccount (Abstract)

- (id)initWithUID:(NSString *)inUID internalObjectID:(NSString *)inInternalObjectID service:(AIService *)inService;
@property (readwrite, retain, nonatomic) NSData *userIconData;
@property (readwrite, nonatomic) BOOL enabled;
@property (readonly, nonatomic) NSString *host;
@property (readonly, nonatomic) int port;
- (void)filterAndSetUID:(NSString *)inUID;

//Status
- (void)preferencesChangedForGroup:(NSString *)group key:(NSString *)key object:(AIListObject *)object
					preferenceDict:(NSDictionary *)prefDict firstTime:(BOOL)firstTime;
- (void)silenceAllContactUpdatesForInterval:(NSTimeInterval)interval;
- (void)updateContactStatus:(AIListContact *)inContact;
- (void)updateCommonStatusForKey:(NSString *)key;
@property (readwrite, retain, nonatomic) AIStatus *statusState;
@property (readonly, nonatomic) AIStatus *actualStatusState;
- (void)setStatusStateAndRemainOffline:(AIStatus *)statusState;

/*!
 * @brief Sent by an account to itself to update its user icon
 *
 * Both NSImage and NSData forms are passed to prevent duplication of data; either or both may be used.
 *
 * The image should be resized as needed for the protocol.
 *
 * Subclasses MUST call super's implementation.
 *
 * @param image An NSImage of the user icon, or nil if no image.
 * @param originalData The original data which made the image, which may be in any NSImage-compatible format, or nil if no image.
 */
- (void)setAccountUserImage:(NSImage *)image withData:(NSData *)originalData;

/*!
 * @brief Called when the account was edited.
 */
- (void)accountEdited;

//Auto-Refreshing Status String
- (NSAttributedString *)autoRefreshingOutgoingContentForStatusKey:(NSString *)key;
- (void)autoRefreshingOutgoingContentForStatusKey:(NSString *)key selector:(SEL)selector context:(id)originalContext;
- (NSAttributedString *)autoRefreshingOriginalAttributedStringForStatusKey:(NSString *)key;
- (void)setValue:(id)value forProperty:(NSString *)key notify:(NotifyTiming)notify;
- (void)startAutoRefreshingStatusKey:(NSString *)key forOriginalValueString:(NSString *)originalValueString;
- (void)stopAutoRefreshingStatusKey:(NSString *)key;
- (void)_startAttributedRefreshTimer;
- (void)_stopAttributedRefreshTimer;
- (void)gotFilteredStatusMessage:(NSAttributedString *)statusMessage forStatusState:(AIStatus *)statusState;
- (void)updateLocalDisplayNameTo:(NSAttributedString *)displayName;
@property (readonly, nonatomic) NSString *currentDisplayName;

//Contacts
@property (readonly, nonatomic) NSArray *contacts;
- (AIListContact *)contactWithUID:(NSString *)sourceUID;
- (void)removeAllContacts;
- (void)removePropertyValuesFromContact:(AIListContact *)listContact silently:(BOOL)silent;
- (NSString *)fallbackAliasForContact:(AIListContact *)contact inChat:(AIChat *)chat;
- (AIAccountGroupDeletionResponse)willDeleteGroup:(AIListGroup *)group;

//Connectivity
@property (readwrite, nonatomic) BOOL shouldBeOnline;
- (void)toggleOnline;
- (void)didConnect;
@property (readonly, nonatomic) NSSet *contactProperties;
- (void)didDisconnect;
- (void)connectScriptCommand:(NSScriptCommand *)command;
- (void)disconnectScriptCommand:(NSScriptCommand *)command;
- (void)serverReportedInvalidPassword;
- (void)getProxyConfigurationNotifyingTarget:(id)target selector:(SEL)selector context:(id)context;
- (NSString *)lastDisconnectionError;
- (void)setLastDisconnectionError:(NSString *)inError;
- (AIReconnectDelayType)shouldAttemptReconnectAfterDisconnectionError:(NSString * __strong *)disconnectionError;
@property (readonly, nonatomic) BOOL encrypted;

//FUS Disconnecting
- (void)autoReconnectAfterDelay:(NSTimeInterval)delay;
- (double)minimumReconnectTime;
- (void)cancelAutoReconnect;
- (void)initFUSDisconnecting;

- (void)setPasswordTemporarily:(NSString *)inPassword;
/*!
 * @brief While we are connected, return the password used to connect
 *
 * This will not look up the password in the keychain. Results are undefined if we are not connected.
 */
@property (readonly, nonatomic) NSString *passwordWhileConnected;

@end

@interface AIAccount (Abstract_ForSubclasses)
//Chats
- (void)displayYouHaveConnectedInChat:(AIChat *)chat;

- (void)passwordReturnedForConnect:(NSString *)inPassword 
						returnCode:(AIPasswordPromptReturn)returnCode 
						   context:(id)inContext;

/*!
 * @brief Does joining a group chat require more information than is contained in the name alone?
 *
 * Default returns NO. Subclasses should return YES if additional information (via a chatCreationDictionary) is needed.
 */
- (BOOL)joiningGroupChatRequiresCreationDictionary;

@end
