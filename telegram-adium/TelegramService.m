//
//  TelegramService.m
//  telegram-adium
//
//  Created by Matthias Jentsch on 06.09.14.
//  Copyright (c) 2014 Matthias Jentsch. All rights reserved.
//

#import "TelegramService.h"
#import "AdiumTelegramAccount.h"

@implementation TelegramService

- (Class)accountClass{
	return [AdiumOkCupidAccount class];
}

/*
- (AIAccountViewController *)accountViewController {
	return [OkCupidAccountViewController accountViewController];
}
 */

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

- (NSString *)UIDPlaceholder
{
	return @"Telegram";
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
	return @"Support for the protocol of the Telegram messenger.";
}

- (BOOL)isSocialNetworkingService
{
	return NO;
}


- (NSCharacterSet *)allowedCharacters{
	return [[NSCharacterSet illegalCharacterSet] invertedSet];
}
- (NSCharacterSet *)ignoredCharacters{
	return [NSCharacterSet characterSetWithCharactersInString:@""];
}
- (BOOL)caseSensitive{
	return NO;
}
- (AIServiceImportance)serviceImportance{
	return AIServiceSecondary;
}
- (NSImage *)defaultServiceIconOfType:(AIServiceIconType)iconType
{
	NSImage *image;
	NSString *imagename;
	NSSize imagesize;
	
	if (iconType == AIServiceIconLarge)
	{
		imagename = @"telegram";
		imagesize = NSMakeSize(48,48);
	} else {
		imagename = @"telegram16";
		imagesize = NSMakeSize(16,16);
	}
	
	image = [NSImage imageNamed:(imagename)]; // TODO: forClass:[self class] loadLazily:YES]
	[image setSize:imagesize];
	return image;
}

@end
