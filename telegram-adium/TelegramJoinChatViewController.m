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

static void tgl_chat_iterator_cb (tgl_peer_t *peer, void *extra) {
  NSMutableArray *A = (__bridge NSMutableArray *)(extra);
  
  // chats with 0 participants were deleted or left by the user
  if (tgl_get_peer_type (peer->id) == TGL_PEER_CHAT && peer->chat.users_num > 0) {
    [A addObject: [NSString stringWithUTF8String: peer->print_name]];
  }
}

- (void)configureForAccount:(AIAccount *)inAccount
{
  [super configureForAccount:inAccount];
  
  PurpleAccount *pa = [(CBPurpleAccount *)inAccount purpleAccount];
  PurpleConnection *gc = purple_account_get_connection(pa);
  if (gc && PURPLE_CONNECTION_IS_CONNECTED(gc)) {
    connection_data *conn = purple_connection_get_protocol_data (gc);
    
    // fetch all active chats and put them into the select box
    [popupButton_existingChat removeAllItems];
    NSMutableArray *chats = [NSMutableArray new];
    tgl_peer_iterator_ex (conn->TLS, tgl_chat_iterator_cb, (__bridge void *)(chats));
    [popupButton_existingChat
        addItemsWithTitles: [chats sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)]];
    
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
    
    // Join by link
    NSString *link = [textField_joinByLink stringValue];
    if ([link length]) {
      import_chat_link_checked (conn->TLS, [link UTF8String]);
      return;
    }
    
    // Create a new chat with provided participants
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
    
    // Pass the joining to the prpl
    const char *name = [[[popupButton_existingChat selectedItem] title] UTF8String];
    PurpleChat *purpleChat = purple_blist_find_chat (conn->pa, name);
    if (purpleChat) {
      serv_join_chat (conn->gc, purple_chat_get_components (purpleChat));
    } else {
      GHashTable *chatCreationInfo = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_free);
      g_hash_table_insert (chatCreationInfo, "subject", g_strdup (name));
      serv_join_chat (conn->gc, chatCreationInfo);
      g_hash_table_destroy (chatCreationInfo);
    }
  }
}

- (NSString *)nibName
{
  return @"TelegramJoinChatView";
}

@end
