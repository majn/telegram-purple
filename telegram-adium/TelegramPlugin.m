//
//  TelegramPlugin.m
//  telegram-adium
//
//  Created by Matthias Jentsch on 06.09.14.
//  Copyright (c) 2014 Matthias Jentsch. All rights reserved.
//

#import "TelegramPlugin.h"
#import "TelegramService.h"
#import "telegram-purple.h"

extern void purple_init_telegram_plugin();

@implementation TelegramPlugin

- (void) installPlugin
{
    purple_init_telegram_plugin();
    [TelegramService registerService];
}

- (void) installLibpurplePlugin
{
}

- (void) loadLibpurplePlugin
{
}

- (void) uninstallPlugin
{
}

- (NSString *)pluginAuthor
{
    return @TG_AUTHOR;
}

-(NSString *)pluginVersion
{
    return @TG_VERSION;
}

-(NSString *)pluginDescription
{
    return @"Telegram";
}

@end
