/**
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
 *
 * Copyright Matthias Jentsch 2014-2015
 */

#import "TelegramService.h"
#import "AdiumTelegramAccount.h"
#import "TelegramAccountViewController.h"
#import "TelegramJoinChatViewController.h"

#import <Adium/AIStatusControllerProtocol.h>
#import <AIUtilities/AIImageAdditions.h>

@implementation TelegramService

- (Class)accountClass{
	return [AdiumTelegramAccount class];
}

- (AIAccountViewController *)accountViewController{
  return [TelegramAccountViewController accountViewController];
}

- (DCJoinChatViewController *)joinChatView{
  return [TelegramJoinChatViewController joinChatView];
}

//Service Description
- (NSString *)serviceCodeUniqueID{
    return @"prpl-telegram";
}

- (NSString *)serviceID{
    return @"Telegram";
}

- (NSString *)serviceClass{
    return @"Telegram";
}

- (NSString *)shortDescription{
    return @"Telegram";
}

- (NSString *)longDescription{
    return @"Telegram";
}

- (NSString *)userNameLabel
{
    return @"Phone number\n(with country prefix)";
}

- (BOOL)supportsProxySettings{
	return YES;
}

- (BOOL)supportsPassword
{
	return NO;
}

- (BOOL)requiresPassword
{
    return NO;
}

- (BOOL)canCreateGroupChats
{
    return YES;
}

- (NSString *)UIDPlaceholder
{
	return @"e.g. +49157123456";
}

- (BOOL)isSocialNetworkingService
{
	return NO;
}

- (AIServiceImportance)serviceImportance{
    return AIServiceSecondary;
}

- (NSCharacterSet *)allowedCharacters{
	return [[NSCharacterSet illegalCharacterSet] invertedSet];
}
- (NSCharacterSet *)allowedCharactersForAccountName
{
    return ([NSCharacterSet characterSetWithCharactersInString: @"+1234567890"]);
}

- (NSUInteger)allowedLengthForAccountName
{
    return 16;
}

- (BOOL)caseSensitive{
	return NO;
}

- (NSImage *)defaultServiceIconOfType:(AIServiceIconType)iconType
{
    if ((iconType == AIServiceIconSmall) || (iconType == AIServiceIconList)) {
        return [NSImage imageNamed:@"telegram16" forClass:[self class] loadLazily:YES];
    } else {
        return [NSImage imageNamed:@"telegram" forClass:[self class] loadLazily:YES];
    }
}

- (NSString *)pathForDefaultServiceIconOfType:(AIServiceIconType)iconType
{
    if ((iconType == AIServiceIconSmall) || (iconType == AIServiceIconList)) {
        return [[NSBundle bundleForClass:[self class]] pathForImageResource:@"telegram16"];
    }
    return [[NSBundle bundleForClass:[self class]] pathForImageResource:@"telegram"];
}

- (void)registerStatuses {
#define ADDSTATUS(name, type) \
[adium.statusController registerStatus:name \
withDescription:[adium.statusController localizedDescriptionForCoreStatusName:name] \
ofType:type forService:self]
    
    [adium.statusController registerStatus:STATUS_NAME_AVAILABLE withDescription:[adium.statusController localizedDescriptionForCoreStatusName:STATUS_NAME_AVAILABLE] ofType:AIAvailableStatusType forService:self];
    
    ADDSTATUS(STATUS_NAME_AVAILABLE, AIAvailableStatusType);
    ADDSTATUS(STATUS_NAME_NOT_AVAILABLE, AIAvailableStatusType);
    ADDSTATUS(STATUS_NAME_OFFLINE, AIOfflineStatusType);
}

@end
