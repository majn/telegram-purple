//
//  TelegramPlugin.h
//  telegram-adium
//
//  Created by Matthias Jentsch on 06.09.14.
//  Copyright (c) 2014 Matthias Jentsch. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Adium/AIPlugin.h>

@interface TelegramPlugin : NSObject <AIPlugin> {
	id service;
}

@end
