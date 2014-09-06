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

#import <Adium/AIControllerProtocol.h>

@class AIService, AIAccount, AIListContact, AIStatus;

#define Account_ListChanged 					@"Account_ListChanged"
#define Adium_RequestSetManualIdleTime			@"Adium_RequestSetManualIdleTime"

@protocol AIAccountControllerRemoveConfirmationDialog <NSObject>
- (void)runModal;
- (void)beginSheetModalForWindow:(NSWindow*)window;
@end

@interface NSObject (AIEditAccountWindowControllerTarget)
//Optional
- (void)editAccountWindow:(NSWindow*)window didOpenForAccount:(AIAccount *)inAccount;

//Required
- (void)editAccountSheetDidEndForAccount:(AIAccount *)inAccount withSuccess:(BOOL)successful;
@end

typedef enum {
	AIPromptAsNeeded = 0,
	AIPromptAlways,
	AIPromptNever
} AIPromptOption;

typedef enum {
	AINickServPassword,
	AIQPassword,
	AIXPassword,
	AIAuthServPassword
} AISpecialPasswordType;

@protocol AIAccountController <AIController>

#pragma mark Services
/*!
 * @brief Register an AIService instance
 *
 * All services should be registered before they are used. A service provides access to an instant messaging protocol.
 */
- (void)registerService:(AIService *)inService;

/*!
 * @brief Returns an array of all available services
 *
 * @return NSArray of AIService instances
 */
@property (nonatomic, readonly) NSArray *services;

/*!
 * @brief Returns an array of all active services
 *
 * "Active" services are those for which the user has an enabled account.
 * @param includeCompatible Include services which are compatible with an enabled account but not specifically active.
 *        For example, if an AIM account is enabled, the ICQ service will be included if this is YES.
 * @return NSArray of AIService instances
 */
- (NSSet *)activeServicesIncludingCompatibleServices:(BOOL)includeCompatible;

/*!
 * @brief Retrieves a service by its unique ID
 *
 * @param uniqueID The serviceCodeUniqueID of the desired service
 * @return AIService if found, nil if not found
 */
- (AIService *)serviceWithUniqueID:(NSString *)uniqueID;

/*!
 * @brief Retrieves a service by service ID.
 *
 * Service IDs may be shared by multiple services if the same service is provided by two different plugins.
 * -[AIService serviceID] returns serviceIDs. An example is @"AIM".
 * @return The first service with the matching service ID, or nil if none is found.
 */
- (AIService *)firstServiceWithServiceID:(NSString *)serviceID;

#pragma mark Passwords
/*!
 * @brief Set the password of an account
 *
 * @param inPassword password to store
 * @param inAccount account the password belongs to
 */
- (void)setPassword:(NSString *)inPassword forAccount:(AIAccount *)inAccount;

/*!
 * @brief Forget the password of an account
 *
 * @param inAccount account whose password should be forgotten. Any stored keychain item will be removed.
 */
- (void)forgetPasswordForAccount:(AIAccount *)inAccount;

/*!
 * @brief Retrieve the stored password of an account
 * 
 * @param inAccount account whose password is desired
 * @return account password, or nil if the password is not available without prompting
 */
- (NSString *)passwordForAccount:(AIAccount *)inAccount;

/*!
 * @brief Retrieve the password of an account, prompting the user if necessary
 *
 * @param inAccount account whose password is desired
 * @param promptOption An AIPromptOption determining whether and how a prompt for the password should be displayed if it is needed. This allows forcing or suppressing of the prompt dialogue. If AIPromptOptionNever is used, the returnCode sent to the target will always be AIPasswordPromptOKReturn.
 * @param inTarget target to notify when password is available
 * @param inSelector selector to notify when password is available. Selector is of the form - (void)returnedPassword:(NSString *)p returnCode:(AIPasswordPromptReturn)returnCode context:(id)context
 * @param inContext context passed to target
 */
- (void)passwordForAccount:(AIAccount *)inAccount promptOption:(AIPromptOption)promptOption notifyingTarget:(id)inTarget selector:(SEL)inSelector context:(id)inContext;

/*!
 * @brief Set the password for a proxy server
 *
 * @param inPassword password to store. Nil to forget the password for this server/username pair.
 * @param server proxy server name
 * @param userName proxy server user name
 *
 * XXX - This is inconsistent.  Above we have a separate forget method, here we forget when nil is passed...
 */
- (void)setPassword:(NSString *)inPassword forProxyServer:(NSString *)server userName:(NSString *)userName;

/*!
 * @brief Retrieve the stored password for a proxy server
 * 
 * @param server proxy server name
 * @param userName proxy server user name
 * @return proxy server password, or nil if the password is not available without prompting
 */
- (NSString *)passwordForProxyServer:(NSString *)server userName:(NSString *)userName;

/*!
 * @brief Retrieve the password for a proxy server, prompting the user if necessary
 *
 * @param server proxy server name
 * @param userName proxy server user name
 * @param inTarget target to notify when password is available
 * @param inSelector selector to notify when password is available. Selector is of the form - (void)returnedPassword:(NSString *)p returnCode:(AIPasswordPromptReturn)returnCode context:(id)context
 * @param inContext context passed to target
 */
- (void)passwordForProxyServer:(NSString *)server userName:(NSString *)userName notifyingTarget:(id)inTarget selector:(SEL)inSelector context:(id)inContext;

// document
- (void)passwordForType:(AISpecialPasswordType)inType forAccount:(AIAccount *)inAccount promptOption:(AIPromptOption)inOption name:(NSString *)inName notifyingTarget:(id)inTarget selector:(SEL)inSelector context:(id)inContext;
- (NSString *)passwordForType:(AISpecialPasswordType)inType forAccount:(AIAccount *)inAccount name:(NSString *)inName;
- (void)setPassword:(NSString *)inPassword forType:(AISpecialPasswordType)inType forAccount:(AIAccount *)inAccount name:(NSString *)inName;

#pragma mark Accounts
@property (nonatomic, readonly) NSArray *accounts;
- (NSArray *)accountsCompatibleWithService:(AIService *)service;
- (NSArray *)accountsWithCurrentStatus:(AIStatus *)status;
- (AIAccount *)accountWithInternalObjectID:(NSString *)objectID;
- (AIAccount *)createAccountWithService:(AIService *)service UID:(NSString *)inUID;
- (void)addAccount:(AIAccount *)inAccount;
- (void)deleteAccount:(AIAccount *)inAccount;
- (NSUInteger)moveAccount:(AIAccount *)account toIndex:(NSUInteger)destIndex;
- (void)accountDidChangeUID:(AIAccount *)inAccount;

/*!
 * @brief Change an account's current and future service
 *
 * This should only be used by an AIAccount to upgrade itself to a new service, as may happen when 
 * transitioning from one implementation to another.
 *
 * Generally, this is not necessary, as migration can be performed by simply substituting the new service ID
 * for the old one in -[AdiumAccounts _upgradeServiceID:forAccountDict:]. However, if that is insufficient,
 * because for example some processing must be done with information from both old and new accounts,
 * this may be useful for performing a move at runtime.
 *
 * It will save the change; account immediately becomes on service, and it will be on service when it is
 * next loaded.
 */
- (void)moveAccount:(AIAccount *)account toService:(AIService *)service;

//Preferred Accounts
- (AIAccount *)preferredAccountForSendingContentType:(NSString *)inType toContact:(AIListContact *)inContact;

//Connection convenience methods
- (void)disconnectAllAccounts;
@property (nonatomic, readonly) BOOL oneOrMoreConnectedAccounts;
@property (nonatomic, readonly) BOOL oneOrMoreConnectedOrConnectingAccounts;

/*!
 * @brief Display account configuration for an account
 *
 * @param account The account to edit. Must not be nil.
 * @param window The window on which to attach the configuration as a sheet. If nil, the editor is shown as a free-standing window.
 * @param target The target to notify when editing is complete. See the AIEditAccountWindowControllerTarget informal protocol.
 */
- (void)editAccount:(AIAccount *)account onWindow:(NSWindow *)window notifyingTarget:(id)target;

@end
