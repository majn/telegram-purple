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

#include <tgl.h>
#include "TelegramJoinChatViewController.h"
#include "telegram-purple.h"
#include "tgp-structs.h"
#include "tgp-chat.h"

#import <Adium/AIContactControllerProtocol.h>
#import <AIUtilities/AICompletingTextField.h>
#import <Adium/AIListContact.h>
#import <Adium/AIWindowController.h>
#import <AdiumLibpurple/CBPurpleAccount.h>

@interface NSObject (JointChatViewDelegate)
- (void)setJoinChatEnabled:(BOOL)enabled;
@end

@implementation TelegramJoinChatViewController

static void tgl_peer_iterator_cb (tgl_peer_t *peer, void *extra) {
  NSMutableArray *A = (__bridge NSMutableArray *)(extra);
  if (tgl_get_peer_type (peer->id) == TGL_PEER_CHAT) {
    [A addObject: [NSValue valueWithPointer: peer]];
  }
}

- (void)configureForAccount:(AIAccount *)inAccount
{
  [super configureForAccount:inAccount];
  
  PurpleAccount *pa = [(CBPurpleAccount *)inAccount purpleAccount];
  PurpleConnection *gc = purple_account_get_connection(pa);
  if (gc && PURPLE_CONNECTION_IS_CONNECTED(gc)) {
    connection_data *conn = purple_connection_get_protocol_data (gc);
    
    [popupButton_existingChat removeAllItems];
    
    NSMutableArray *a = [NSMutableArray new];
    tgl_peer_iterator_ex (conn->TLS, tgl_peer_iterator_cb, (__bridge void *)(a));
    
    NSMutableArray *chats = [NSMutableArray new];
    NSMutableArray *leftChats = [NSMutableArray new];
    for (NSValue *V in a) {
      tgl_peer_t *P = [V pointerValue];
      NSString *name = [[NSString alloc] initWithUTF8String:P->print_name];
      if (chat_is_member (conn->TLS->our_id, &P->chat)) {
        [chats addObject: name];
      }
      else {
        [leftChats addObject: name];
      }
    }
    
    NSArray *sortedChats = [chats sortedArrayUsingComparator:^NSComparisonResult(id a, id b) {
      return [(NSString*)a
              compare:b
              options:NSDiacriticInsensitiveSearch | NSCaseInsensitiveSearch];
    }];
    [popupButton_existingChat addItemsWithTitles: sortedChats];
    
    // TODO: Display left chats with a grey font to indicate that those are no
    // longer, but still allow the user to view the history
    // [popupButton_existingChat addItemsWithTitles: leftChats];
  }
}

- (void)joinChatWithAccount:(AIAccount *)inAccount
{
	NSString *room = [[popupButton_existingChat selectedItem] title];

  NSDictionary *chatCreationInfo = [NSDictionary
                                    dictionaryWithObjectsAndKeys:room, @"subject", nil];
  
  [self doJoinChatWithName:room
                 onAccount:inAccount
          chatCreationInfo:chatCreationInfo
          invitingContacts:@[]
     withInvitationMessage:@""];
}

- (NSString *)nibName
{
  return @"TelegramJoinChatView";
}


@end
