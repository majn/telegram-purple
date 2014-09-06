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

#import <AddressBook/AddressBook.h>
#import <Adium/AIContactObserverManager.h>
#import <Adium/AIContactControllerProtocol.h>
#import <Adium/AIUserIcons.h>

#define PREF_GROUP_ADDRESSBOOK					@"Address Book"
#define KEY_AB_ENABLE_IMPORT					@"AB Enable Import"
#define KEY_AB_DISPLAYFORMAT					@"AB Display Format String"
#define KEY_AB_NOTE_SYNC						@"AB Note Sync"
#define KEY_AB_USE_IMAGES						@"AB Use AB Images"
#define KEY_AB_IMAGE_SYNC						@"AB Image Sync"
#define KEY_AB_PREFER_ADDRESS_BOOK_IMAGES		@"AB Prefer AB Images"
#define KEY_AB_USE_FIRSTNAME					@"AB Use FirstName"
#define KEY_AB_USE_NICKNAME						@"AB Use NickName Only"
#define KEY_AB_CREATE_METACONTACTS				@"AB Create MetaContacts"
#define AB_DISPLAYFORMAT_DEFAULT_PREFS			@"AB Display Format Defaults"

#define FORMAT_FULL								@"FULL"
#define FORMAT_INITIAL							@"INITIAL"
#define FORMAT_FIRST_INITIAL					@"%[FIRSTINITIAL]"
#define FORMAT_FIRST_FULL						@"%[FIRSTFULL]"
#define FORMAT_MIDDLE_INITIAL					@"%[MIDDLEINITIAL]"
#define FORMAT_MIDDLE_FULL						@"%[MIDDLEFULL]"
#define FORMAT_LAST_INITIAL						@"%[LASTINITIAL]"
#define FORMAT_LAST_FULL						@"%[LASTFULL]"
#define FORMAT_NICK_INITIAL						@"%[NICKINITIAL]"
#define FORMAT_NICK_FULL						@"%[NICKFULL]"

@class AIService, AIAddressBookUserIconSource;

typedef enum {
	AIRequiresAddressBookEntry,
	AIRequiresNoAddressBookEntry
} AIAddressBookContextMenuTag;

@interface AIAddressBookController : NSObject <AIListObjectObserver, ABImageClient> {
@private
	NSMenuItem			*showInABContextualMenuItem;
	NSMenuItem			*editInABContextualMenuItem;
	NSMenuItem			*addToABContexualMenuItem;

	NSInteger			meTag;
    
	NSString				*displayFormat;
	BOOL					enableImport;
	BOOL					useFirstName;
	BOOL					useNickNameOnly;
	BOOL					automaticUserIconSync;
	BOOL					createMetaContacts;
	
	AIAddressBookUserIconSource *addressBookUserIconSource;
	
	NSMutableDictionary			*personUniqueIdToMetaContactDict;
}

+ (void) startAddressBookIntegration;
+ (void) stopAddressBookIntegration;

+ (AIService *)serviceFromProperty:(NSString *)property;
+ (NSString *)propertyFromService:(AIService *)service;
+ (ABPerson *)personForListObject:(AIListObject *)inObject;

@end
