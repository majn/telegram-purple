//
//  TelegramPlugin.m
//  telegram-adium
//
//  Created by Matthias Jentsch on 06.09.14.
//  Copyright (c) 2014 Matthias Jentsch. All rights reserved.
//

#import "telegram-purple.h"
#import "TelegramPlugin.h"
#import "TelegramService.h"

@implementation TelegramPlugin

- (void) installPlugin
{
    purple_init_telegram();
    service = [[TelegramService alloc] init];
}

- (void) uninstallPlugin
{
    service = nil;
}

@end
