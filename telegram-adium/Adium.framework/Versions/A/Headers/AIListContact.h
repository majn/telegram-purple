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

#import <Adium/AIListObject.h>
#import <Adium/AIAccount.h>
#import <Adium/AIChat.h>

#define KEY_AB_UNIQUE_ID		@"ABUniqueID"

@class ABPerson, AIListGroup, AIMetaContact;

@interface AIListContact : AIListObject {
	AIAccount	*account;
	AIMetaContact *metaContact;
	NSMutableSet *m_remoteGroupNames;
	NSString		*internalUniqueObjectID;
	
	// Former properties
	NSColor			*textColor;
	NSInteger		idle;

	NSString		*formattedUID;
}

- (id)initWithUID:(NSString *)inUID account:(AIAccount *)inAccount service:(AIService *)inService;
- (id)initWithUID:(NSString *)inUID service:(AIService *)inService;
@property (unsafe_unretained, readonly, nonatomic) id<AIContainingObject>containingObject __attribute__((deprecated));
@property (readwrite, copy, nonatomic) NSSet *remoteGroupNames;
@property (readonly, nonatomic) NSUInteger countOfRemoteGroupNames;
@property (weak, readonly, nonatomic) NSSet *remoteGroups;
- (void) addRemoteGroupName:(NSString *)name;
- (void) removeRemoteGroupName:(NSString *)name;

- (void)setUID:(NSString *)inUID;
@property (readonly, nonatomic) AIAccount *account;
- (NSString *)internalUniqueObjectID;
+ (NSString *)internalUniqueObjectIDForService:(AIService *)inService account:(AIAccount *)inAccount UID:(NSString *)inUID;
- (void)restoreGrouping;

@property (weak, readonly, nonatomic) AIListContact *parentContact; //This needs renaming. It's the 'topmost' contact, either self or meta
@property (readonly, nonatomic, weak) AIMetaContact *metaContact;

@property (weak, readonly, nonatomic) NSString *ownDisplayName;
@property (weak, readonly, nonatomic) NSString *ownPhoneticName;
@property (weak, readonly, nonatomic) NSString *serversideDisplayName;

@property (readonly, nonatomic) BOOL canJoinMetaContacts;

@property (readonly, nonatomic) BOOL existsServerside; //Metas and Bookmarks (among others?) have no serverside representation

@property (readonly, nonatomic) BOOL isIntentionallyNotAStranger;

@property (readonly, nonatomic) AIEncryptedChatPreference encryptedChatPreferences;

- (void)setIsMobile:(BOOL)isMobile notify:(NotifyTiming)notify;
- (void)setOnline:(BOOL)online notify:(NotifyTiming)notify silently:(BOOL)silent;
- (void)setSignonDate:(NSDate *)signonDate notify:(NotifyTiming)notify;
- (NSDate *)signonDate;
- (void)setIsBlocked:(BOOL)yesOrNo updateList:(BOOL)addToPrivacyLists;
- (void)setIsAllowed:(BOOL)yesOrNo updateList:(BOOL)addToPrivacyLists;
- (void)setIsOnPrivacyList:(BOOL)yesOrNo updateList:(BOOL)addToPrivacyLists privacyType:(AIPrivacyType)privType;

- (void)setIdle:(BOOL)isIdle sinceDate:(NSDate *)idleSinceDate notify:(NotifyTiming)notify;
- (void)setServersideIconData:(NSData *)iconData notify:(NotifyTiming)notify;

- (void)setWarningLevel:(NSInteger)warningLevel notify:(NotifyTiming)notify;
- (NSInteger)warningLevel;

- (void)setProfileArray:(NSArray *)array notify:(NotifyTiming)notify;
- (void)setProfile:(NSAttributedString *)profile notify:(NotifyTiming)notify;
- (NSArray *)profileArray;
- (NSAttributedString *)profile;

- (void)setServersideAlias:(NSString *)alias 
				  silently:(BOOL)silent;

- (NSAttributedString *)contactListStatusMessage;

- (NSWritingDirection)baseWritingDirection;
- (void)setBaseWritingDirection:(NSWritingDirection)direction;

- (ABPerson *)addressBookPerson;
- (void)setAddressBookPerson:(ABPerson *)inPerson;

@end
