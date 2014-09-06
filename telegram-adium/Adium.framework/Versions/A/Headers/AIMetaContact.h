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

#import <Adium/AIListContact.h>

#define	KEY_PREFERRED_DESTINATION_CONTACT	@"Preferred Destination Contact"

@interface AIMetaContact : AIListContact <AIContainingObject> {
	NSNumber					*objectID;

	AIListContact		*_preferredContact;
	NSArray				*_listContacts;
	NSArray				*_listContactsIncludingOfflineAccounts;
	
	BOOL								containsOnlyOneUniqueContact;

	NSMutableArray	*_containedObjects;			//Manually ordered array of contents
	BOOL								containedObjectsNeedsSort;
	BOOL								saveGroupingChanges;
	
	BOOL								expanded;			//Exanded/Collapsed state of this object
}

//The objectID is unique to a meta contact and is used as the UID for purposes of AIListContact inheritance
- (id)initWithObjectID:(NSNumber *)objectID;
@property (readonly, nonatomic) NSNumber *objectID;
+ (NSString *)internalObjectIDFromObjectID:(NSNumber *)inObjectID;

@property (weak, readonly, nonatomic) AIListContact *preferredContact;
- (AIListContact *)preferredContactWithCompatibleService:(AIService *)inService;

- (void)updateRemoteGroupingOfContact:(AIListContact *)inListContact;

@property (readonly, nonatomic) BOOL containsOnlyOneService;
@property (readonly, nonatomic) NSUInteger uniqueContainedObjectsCount;
- (AIListObject *)uniqueObjectAtIndex:(int)inIndex;

@property (weak, readonly, nonatomic) NSDictionary *dictionaryOfServiceClassesAndListContacts;
@property (weak, readonly, nonatomic) NSArray *servicesOfContainedObjects;

/*
 * @brief A flat array of AIListContacts each with a different internalObjectID
 *
 * If multiple AIListContacts with the same UID/Service are within this metacontact (i.e. from multiple accounts),
 * only one will be included in this array, and that one will be the most available of them.
 * Contacts from all accounts, including offline ones, will be included.
 */
- (NSArray *)listContactsIncludingOfflineAccounts;
- (BOOL)removeObject:(AIListObject *)inObject;

@end
