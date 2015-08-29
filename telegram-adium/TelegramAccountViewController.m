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

#import "TelegramAccountViewController.h"
#import "TelegramAccount.h"

#import <Adium/AIService.h>
#import <AIUtilities/AIStringFormatter.h>
#import <AIUtilities/AIAttributedStringAdditions.h>
#import <AIUtilities/AIPopUpButtonAdditions.h>

#include "telegram-purple.h"

@implementation TelegramAccountViewController

- (NSString *)nibName{
    return @"TelegramAccountView";
}

- (void)configureForAccount:(AIAccount *)inAccount
{
  [super configureForAccount:inAccount];
  
  NSString *acceptSecretChats = [account
                                 preferenceForKey:@"Telegram:"TGP_KEY_ACCEPT_SECRET_CHATS
                                 group:GROUP_ACCOUNT_STATUS] ?: @TGP_DEFAULT_ACCEPT_SECRET_CHATS;
  
  NSInteger row = 0;
  if ([acceptSecretChats isEqual:@"always"]) {
    row = 1;
  } else if ([acceptSecretChats isEqual:@"never"]) {
    row = 2;
  }
  [radio_Encryption selectCellAtRow:row column:0];
  
  NSString *passwordTwoFactor = [account preferenceForKey:@"Telegram:"TGP_KEY_PASSWORD_TWO_FACTOR
                                 group:GROUP_ACCOUNT_STATUS] ?: nil;
  [textField_password setStringValue:passwordTwoFactor];
  
  id s = [account preferenceForKey:@"Telegram:"TGP_KEY_HISTORY_SYNC_ALL group:GROUP_ACCOUNT_STATUS];
  [checkbox_historySyncAll setState:[s boolValue]];
  
  id read = [account preferenceForKey:@"Telegram:"TGP_KEY_DISPLAY_READ_NOTIFICATIONS group:GROUP_ACCOUNT_STATUS];
  [checkbox_displayReadNotifications setState:[read boolValue]];
  
  id send = [account preferenceForKey:@"Telegram:"TGP_KEY_SEND_READ_NOTIFICATIONS group:GROUP_ACCOUNT_STATUS];
  [checkbox_sendReadNotifications setState:[send boolValue]];

  NSString *inactiveDaysOffline = [account
                                   preferenceForKey:@"Telegram:"TGP_KEY_INACTIVE_DAYS_OFFLINE
                                   group:GROUP_ACCOUNT_STATUS] ?: @"";
  [textField_inactiveDaysOffline setStringValue:inactiveDaysOffline];
  
  NSString *historyRetrievalThreshold = [account
                                         preferenceForKey:@"Telegram:"TGP_KEY_HISTORY_RETRIEVAL_THRESHOLD
                                         group:GROUP_ACCOUNT_STATUS] ?: @"";
  [textField_historyRetrieveDays setStringValue:historyRetrievalThreshold];
}

- (void)saveConfiguration
{
	[super saveConfiguration];
	
  NSArray *selections = @[@"ask", @"always", @"never"];
  
  [account setPreference:[textField_password stringValue]
                  forKey:@"Telegram:"TGP_KEY_PASSWORD_TWO_FACTOR
                   group:GROUP_ACCOUNT_STATUS];
  
  [account setPreference:selections[[radio_Encryption selectedRow]]
                  forKey:@"Telegram:"TGP_KEY_ACCEPT_SECRET_CHATS
                   group:GROUP_ACCOUNT_STATUS];

  [account setPreference:[NSNumber numberWithBool:[checkbox_historySyncAll state]]
                  forKey:@"Telegram:"TGP_KEY_HISTORY_SYNC_ALL
                   group:GROUP_ACCOUNT_STATUS];
  
  [account setPreference:[NSNumber numberWithBool:[checkbox_displayReadNotifications state]]
                  forKey:@"Telegram:"TGP_KEY_DISPLAY_READ_NOTIFICATIONS
                   group:GROUP_ACCOUNT_STATUS];
  
  [account setPreference:[NSNumber numberWithBool: [checkbox_sendReadNotifications state]]
                  forKey:@"Telegram:"TGP_KEY_SEND_READ_NOTIFICATIONS
                   group:GROUP_ACCOUNT_STATUS];
  
  [account setPreference:[textField_historyRetrieveDays stringValue]
                  forKey:@"Telegram:"TGP_KEY_HISTORY_RETRIEVAL_THRESHOLD
                   group:GROUP_ACCOUNT_STATUS];
  
  [account setPreference:[textField_inactiveDaysOffline stringValue]
                  forKey:@"Telegram:"TGP_KEY_INACTIVE_DAYS_OFFLINE
                   group:GROUP_ACCOUNT_STATUS];
}	

@end
