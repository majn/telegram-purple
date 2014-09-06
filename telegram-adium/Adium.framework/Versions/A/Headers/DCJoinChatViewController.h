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


@class AIChat, AIAccount, AIMetaContact, AIListContact, AIService;

@interface DCJoinChatViewController : NSObject {
	IBOutlet		NSView			*__weak view;			// Custom view
	AIChat							*chat;			// The newly created chat
	AIAccount						*account;		// The account we're being configured for

	id								__unsafe_unretained delegate;		// Our delegate
	id								__unsafe_unretained sharedChatInstance;
}

+ (DCJoinChatViewController *)joinChatView;

- (id)init;
@property (weak, readonly, nonatomic) NSView *view;
@property (weak, readonly, nonatomic) NSString *nibName;

- (void)configureForAccount:(AIAccount *)inAccount;
- (void)joinChatWithAccount:(AIAccount *)inAccount;

- (NSString *)impliedCompletion:(NSString *)aString;
- (AIListContact *)validContact:(NSString *)uniqueID withService:(AIService *)service;
- (NSDragOperation)doDraggingEntered:(id <NSDraggingInfo>)sender;
- (BOOL)doPerformDragOperation:(id <NSDraggingInfo>)sender toField:(NSTextField *)theField;

- (void)doJoinChatWithName:(NSString *)inName
				 onAccount:(AIAccount *)inAccount
		  chatCreationInfo:(NSDictionary *)inInfo 
		  invitingContacts:(NSArray *)contactsToInvite
	 withInvitationMessage:(NSString *)invitationMessage;
- (NSArray *)contactsFromNamesSeparatedByCommas:(NSString *)namesSeparatedByCommas onAccount:(AIAccount *)inAccount;

@property (readwrite, nonatomic, unsafe_unretained) id delegate;

//roomlistWindowController delegate
@property (readwrite, nonatomic, unsafe_unretained) id sharedChatInstance;

@end


