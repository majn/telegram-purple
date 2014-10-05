//
//  AdiumTelegramAccount.m
//  telegram-adium
//
//  Created by Matthias Jentsch on 06.09.14.
//  Copyright (c) 2014 Matthias Jentsch. All rights reserved.
//

#import "AdiumTelegramAccount.h"

@implementation AdiumTelegramAccount

- (const char*)protocolPlugin
{
	return "prpl-telegram";
}

- (NSString *)host
{
	return @"149.154.167.50";
}

- (int)port
{
	return 443;
}

- (BOOL)canSendOfflineMessageToContact:(AIListContact *)inContact
{
	return YES;
}

@end
