//
//  TelegramService.m
//  telegram-adium
//
//  Created by Matthias Jentsch on 06.09.14.
//  Copyright (c) 2014 Matthias Jentsch. All rights reserved.
//

#import "TelegramService.h"
#import "AdiumTelegramAccount.h"

#import <Adium/AIStatusControllerProtocol.h>
#import <AIUtilities/AIImageAdditions.h>

@implementation TelegramService

- (Class)accountClass{
	return [AdiumTelegramAccount class];
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
    return @"Phone Number";
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
	return YES;
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

- (NSCharacterSet *)ignoredCharacters{
	return [NSCharacterSet characterSetWithCharactersInString:@"/-"];
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
