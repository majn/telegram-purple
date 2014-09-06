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

#import <Adium/AIControllerProtocol.h>
#import <Adium/AIGroupChat.h>

@protocol AIChatController_Private;
@class AIChat, AIAccount, AIListContact;

//Observer which receives notifications of changes in chat status
@protocol AIChatObserver
- (NSSet *)updateChat:(AIChat *)inChat keys:(NSSet *)inModifiedKeys silent:(BOOL)silent;
@end

@protocol AIChatController <AIController, AIChatController_Private>
//Chats
- (NSSet *)allChatsWithContact:(AIListContact *)inContact;
- (NSSet *)allGroupChatsContainingContact:(AIListContact *)inContact;
- (AIChat *)openChatWithContact:(AIListContact *)inContact onPreferredAccount:(BOOL)onPreferredAccount;
- (AIChat *)chatWithContact:(AIListContact *)inContact;
- (AIChat *)existingChatWithContact:(AIListContact *)inContact;
- (AIChat *)existingChatWithUniqueChatID:(NSString *)uniqueChatID;
- (AIGroupChat *)chatWithName:(NSString *)inName 
			  identifier:(id)identifier
			   onAccount:(AIAccount *)account
		chatCreationInfo:(NSDictionary *)chatCreationInfo;

- (AIGroupChat *)existingChatWithIdentifier:(id)identifier onAccount:(AIAccount *)account;
- (AIGroupChat *)existingChatWithName:(NSString *)name onAccount:(AIAccount *)account;

- (BOOL)closeChat:(AIChat *)inChat;
- (void)accountDidCloseChat:(AIChat *)inChat;
- (void)restoreChat:(AIChat *)inChat;

@property (nonatomic, readonly) NSSet *openChats;
@property (nonatomic, readonly) AIChat *mostRecentUnviewedChat;
@property (nonatomic, readonly) NSUInteger unviewedContentCount;
@property (nonatomic, readonly) NSUInteger unviewedConversationCount;

- (void)switchChat:(AIChat *)chat toAccount:(AIAccount *)newAccount;
- (void)switchChat:(AIChat *)chat toListContact:(AIListContact *)inContact usingContactAccount:(BOOL)useContactAccount;
- (BOOL)contactIsInGroupChat:(AIListContact *)listContact;

	//Status
- (void)registerChatObserver:(id <AIChatObserver>)inObserver;
- (void)unregisterChatObserver:(id <AIChatObserver>)inObserver;
- (void)updateAllChatsForObserver:(id <AIChatObserver>)observer;

	//Addition/removal of contacts to group chats
- (void)chat:(AIChat *)chat addedListContacts:(NSArray *)contacts notify:(BOOL)notify;
- (void)chat:(AIChat *)chat removedListContact:(AIListContact *)inContact;

- (NSString *)defaultInvitationMessageForRoom:(NSString *)room account:(AIAccount *)inAccount;
@end

@protocol AIChatController_Private
- (void)chatStatusChanged:(AIChat *)inChat modifiedStatusKeys:(NSSet *)inModifiedKeys silent:(BOOL)silent;
@end
