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

#include <tgl.h>
#include "telegram-purple.h"
#include "tgp-structs.h"
#include "tgp-chat.h"
#include "telegram-base.h"

#include "TelegramJoinChatViewController.h"
#include "TelegramAutocompletionDelegate.h"

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
    
    // provide the current TLS instance for access by the autocompletion
    TelegramAutocompletionDelegate *D = [tokenField_createChatUsers delegate];
    if (D != nil) {
      [D setTLS:conn->TLS];
    }
  }
}

- (void)joinChatWithAccount:(AIAccount *)inAccount
{
  PurpleAccount *pa = [(CBPurpleAccount *)inAccount purpleAccount];
  PurpleConnection *gc = purple_account_get_connection(pa);
  if (gc && PURPLE_CONNECTION_IS_CONNECTED(gc)) {
    connection_data *conn = purple_connection_get_protocol_data (gc);
    NSString *createChatName = [textField_createChatName stringValue];
    NSArray *tokens = [tokenField_createChatUsers objectValue];
    if ([createChatName length] && [tokens count]) {
      int i, cnt = (int)[tokens count];
      const char **users = g_malloc(cnt * sizeof(char*));
      for (i = 0; i < cnt; i++) {
        users[i] = [tokens[i] UTF8String];
      }
      tgp_create_group_chat_by_usernames (conn->TLS, [createChatName UTF8String], users, i, TRUE);
      g_free (users);
      return;
    }
    /*
      int i = 0;
      tgl_peer_id_t ids[(int)[tokens count] + 1];
      ids[i++] = TGL_MK_USER(conn->TLS->our_id);
      for (NSString *user in tokens) {
        tgl_peer_t *P = NULL;
        P = tgl_peer_get_by_name (conn->TLS, [user UTF8String]);
        if (P) {
          ids[i++] = P->id;
        }
      }
      const char *subject = [createChatName UTF8String];
      tgl_do_create_group_chat(conn->TLS, i, ids, subject, (int) strlen(subject),
                               create_group_chat_done_cb, NULL);
      // TODO: display new chat
      return;
    }
     */
    
    NSString *room = [[popupButton_existingChat selectedItem] title];
    NSDictionary *chatCreationInfo = [NSDictionary
                                      dictionaryWithObjectsAndKeys:room, @"subject", nil];
    [self doJoinChatWithName:room
                   onAccount:inAccount
            chatCreationInfo:chatCreationInfo
            invitingContacts:@[]
       withInvitationMessage:@""];
  }
}

- (NSString *)nibName
{
  return @"TelegramJoinChatView";
}

@end
