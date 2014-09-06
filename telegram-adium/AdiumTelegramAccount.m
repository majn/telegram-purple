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

- (BOOL)connectivityBasedOnNetworkReachability
{
	return YES;
}

- (NSString *)host
{
	return @"173.240.5.1";
}

- (int)port
{
	return 443;
}

/*
- (NSString *)encodedAttributedString:(NSAttributedString *)inAttributedString forListObject:(AIListObject *)inListObject
{
	NSString *temp = [AIHTMLDecoder encodeHTML:inAttributedString
									   headers:YES
									  fontTags:NO
							includingColorTags:NO
								 closeFontTags:NO
									 styleTags:NO
					closeStyleTagsOnFontChange:NO
								encodeNonASCII:NO
								  encodeSpaces:NO
									imagesPath:nil
							 attachmentsAsText:YES
					 onlyIncludeOutgoingImages:NO
								simpleTagsOnly:YES
								bodyBackground:NO
						   allowJavascriptURLs:NO];
	return temp;
}
*/
 
- (BOOL)canSendOfflineMessageToContact:(AIListContact *)inContact
{
	return YES;
}


@end
