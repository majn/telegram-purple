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

#import <Adium/ESObjectWithProperties.h>
#import <Adium/AIStatusDefines.h>

@class AIListObject, AIStatus, AIService, AIMutableOwnerArray, AIListGroup;

#define	KEY_ORDER_INDEX		@"Order Index"
#define KEY_IS_BLOCKED		@"isBlocked"

#define KEY_FORMATTED_UID	@"FormattedUID"

typedef enum {
	AIAvailableStatus = 'avaL',
	AIAwayStatus = 'awaY',
	AIIdleStatus = 'idlE',
	AIAwayAndIdleStatus = 'aYiE',
	AIOfflineStatus = 'offL',
	AIUnknownStatus = 'unkN'
} AIStatusSummary;

typedef enum {
		AIGroupChatNone						= 0x0000, /**< No flags                     */
		AIGroupChatVoice					= 0x0001, /**< Voiced user or "Participant" */
		AIGroupChatHalfOp					= 0x0002, /**< Half-op                      */
		AIGroupChatOp						= 0x0004, /**< Channel Op or Moderator      */
		AIGroupChatFounder					= 0x0008, /**< Channel Founder              */
		AIGroupChatTyping					= 0x0010, /**< Currently typing             */
		AIGroupChatAway						= 0x0020 /**< Currently away				*/
} AIGroupChatFlags;

@protocol AIContainingObject <NSObject, NSFastEnumeration>

@property (readonly, copy, nonatomic) NSArray *visibleContainedObjects;
@property (readonly, copy, nonatomic) NSArray *containedObjects;
- (NSUInteger) countOfContainedObjects;
@property (readonly, nonatomic) NSArray *uniqueContainedObjects;

@property (readonly, nonatomic)  NSString *contentsBasedIdentifier;

- (BOOL)containsObject:(AIListObject *)inObject;

/*!
 * As NSArray's -objectAtIndex:, except that it only looks at visible objects contained in this object
 */
- (AIListObject *)visibleObjectAtIndex:(NSUInteger)index;

/*!
 * As NSArray's -indexOfObject:, except that it looks at visible objects contained in this object
 */
- (NSUInteger)visibleIndexOfObject:(AIListObject *)object;


- (AIListObject *)objectWithService:(AIService *)inService UID:(NSString *)inUID;

@property (readonly, nonatomic) float smallestOrder;
@property (readonly, nonatomic) float largestOrder;
- (void)listObject:(AIListObject *)listObject didSetOrderIndex:(float)inOrderIndex;
- (float)orderIndexForObject:(AIListObject *)listObject;

- (void)removeObjectAfterAccountStopsTracking:(AIListObject *)inObject;

@property (readwrite, nonatomic, getter=isExpanded) BOOL expanded;
@property (readonly, nonatomic, getter=isExpandable) BOOL expandable;
@property (readonly, nonatomic) NSUInteger visibleCount;
- (BOOL)canContainObject:(id)obj;

- (BOOL)addObject:(AIListObject *)inObject;
- (void)removeObject:(AIListObject *)inObject;

@end

@interface AIListObject : ESObjectWithProperties {
	AIService			*__weak service;
	
	NSString			*UID;
	NSString			*internalObjectID;

	//Grouping, Manual ordering
	NSMutableSet *m_groups; //The AIContainingObjects that this object is in
	
	float				cachedSmallestOrder;
	float				cachedLargestOrder;
	
	BOOL				isOnline;
	
	// Former properties
	NSAttributedString	*listObjectStatusMessage;
	
	NSString			*webKitUserIconPath;
}

- (id)initWithUID:(NSString *)inUID service:(AIService *)inService;

- (void)object:(id)inObject didChangeValueForProperty:(NSString *)key notify:(NotifyTiming)notify;
- (void)notifyOfChangedPropertiesSilently:(BOOL)silent;

//Identifying information
@property (readonly, nonatomic) NSString *UID;
@property (readonly, weak, nonatomic) AIService *service;
@property (weak, readonly, nonatomic) NSString *internalObjectID;
+ (NSString *)internalObjectIDForServiceID:(NSString *)inServiceID UID:(NSString *)inUID;

//Visibility
@property (readwrite, nonatomic) BOOL alwaysVisible;

//Grouping
//Not recommended for most uses. Use -groups and -metaContact instead unless you really need both
@property (weak, readonly, nonatomic) NSSet *containingObjects;
@property (readonly, copy, nonatomic) NSSet *groups;
- (void)removeContainingGroup:(AIListGroup *)group;
- (void)addContainingGroup:(AIListGroup *)group;
- (void)removeFromGroup:(AIListObject <AIContainingObject> *)group;

//Display
@property (weak, readonly, nonatomic) NSString *formattedUID;
- (void)setFormattedUID:(NSString *)inFormattedUID notify:(NotifyTiming)notify;
@property (weak, readonly, nonatomic) NSString *longDisplayName;

//Prefs
- (void)setPreference:(id)value forKey:(NSString *)inKey group:(NSString *)groupName;
- (void)setPreferences:(NSDictionary *)prefs inGroup:(NSString *)group;
- (id)preferenceForKey:(NSString *)inKey group:(NSString *)groupName;
@property (weak, readonly, nonatomic) NSString *pathToPreferences;

//Key-Value pairing
@property (readonly, nonatomic) BOOL online;

@property (weak, readonly, nonatomic) NSString *statusName;
@property (readonly, nonatomic) AIStatusType statusType;
- (void)setStatusWithName:(NSString *)statusName statusType:(AIStatusType)statusType notify:(NotifyTiming)notify;
@property (weak, readonly, nonatomic) NSAttributedString *statusMessage;
@property (weak, readonly, nonatomic) NSString *statusMessageString;
- (void)setStatusMessage:(NSAttributedString *)statusMessage notify:(NotifyTiming)notify;
- (void)setBaseAvailableStatusAndNotify:(NotifyTiming)notify;
@property (readonly, nonatomic) AIStatusSummary statusSummary;

@property (readonly, nonatomic) BOOL soundsAreMuted;

@property (readonly, nonatomic) BOOL isStranger;
@property (readonly, nonatomic) BOOL isMobile;
@property (readonly, nonatomic) BOOL isBlocked;

@property (readwrite, nonatomic, strong) NSString *displayName;

@property (weak, readonly, nonatomic) NSString *phoneticName;

@property (readwrite, nonatomic, strong) NSString *notes;

@property (readonly, nonatomic) NSInteger idleTime;

@property (unsafe_unretained, readonly, nonatomic) NSImage *userIcon;
@property (unsafe_unretained, readonly, nonatomic) NSImage *menuIcon;
@property (unsafe_unretained, readonly, nonatomic) NSImage *statusIcon;
@property (weak, readwrite, nonatomic) NSData *userIconData;
- (void)setUserIconData:(NSData *)inData;

//For use only by subclasses
@property (unsafe_unretained, readonly, nonatomic) NSImage *internalUserIcon;

//mutableOwnerArray delegate and methods
- (void)listObject:(AIListObject *)listObject mutableOwnerArray:(AIMutableOwnerArray *)inArray didSetObject:(id)anObject withOwner:(AIListObject *)inOwner priorityLevel:(float)priority;

//Comparison
- (NSComparisonResult)compare:(AIListObject *)other;

/*!
 * These methods are part of the AIContainingObject protocol
 * but are implemented by AIListObject (which does not conform to the protocol) for the convenience
 * of subclasses.
 */
@property (readonly, nonatomic) float smallestOrder;
@property (readonly, nonatomic) float largestOrder;
- (void)listObject:(AIListObject *)listObject didSetOrderIndex:(float)inOrderIndex;
- (float)orderIndexForObject:(AIListObject *)listObject;
- (void) moveContainedObject:(AIListObject *)object toIndex:(NSInteger)index;

@end
