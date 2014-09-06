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

@class AIListContact, AIListObject, AIServersideUserIconSource;

typedef enum {
	AIUserIconSourceDidNotFindIcon = 0,
	AIUserIconSourceFoundIcon,
	AIUserIconSourceLookingUpIconAsynchronously
} AIUserIconSourceQueryResult;

typedef CGFloat AIUserIconPriority;
#define AIUserIconHighestPriority   0.00f
#define AIUserIconHighPriority      0.25f
#define AIUserIconMediumPriority    0.50f
#define AIUserIconLowPriority       0.75f
#define AIUserIconLowestPriority    1.00f

@protocol AIUserIconSource
/*!
 * @brief AIUserIcons wants this source to update its user icon for an object
 *
 * Call +[AIUserIcons userIconSource:didDetermineUserIcon:asynchronously:forObject:] with the new icon, if appropriate
 *
 * @result An AIUserIconSourceQueryResult indicating the result
 */
- (AIUserIconSourceQueryResult)updateUserIconForObject:(AIListObject *)inObject;

/*!
 * @brief The priority at which this source should be used. See the \#defines in AIUserIcons.h for posible values.
 */
@property (readonly, nonatomic) AIUserIconPriority priority;
@end

@interface AIUserIcons : NSObject {
	
}

/*!
 * @brief Register a user icon source
 */
+ (void)registerUserIconSource:(id <AIUserIconSource>)inSource;

/*!
 * @brief Get the user icon source currently providingthe icon for an object
 */
+ (void)userIconSource:(id <AIUserIconSource>)inSource priorityDidChange:(AIUserIconPriority)newPriority fromPriority:(AIUserIconPriority)oldPriority;

/*!
 * @brief Determine if a change in a given icon source would potentially change the icon of an object
 */
+ (BOOL)userIconSource:(id <AIUserIconSource>)inSource changeWouldBeRelevantForObject:(AIListObject *)inObject;

/*!
 * @brief Called by an icon source to inform us that its provided icon changed
 */
+ (void)userIconSource:(id <AIUserIconSource>)inSource didChangeForObject:(AIListObject *)inObject;

/*!
 * @brief A user icon source determined a user icon for an object
 *
 * This should be called only by a user icon source upon successful determination of a user icon
 */
+ (void)userIconSource:(id <AIUserIconSource>)inSource didDetermineUserIcon:(NSImage *)inUserIcon asynchronously:(BOOL)wasAsynchronous forObject:(AIListObject *)inObject;

/*!
 * @brief Set what user icon and source an object is currently using (regardless of what AIUserIcon would otherwise do)
 *
 * This is useful if an object knows something AIUserIcons can't. For example, AIMetaContact uses this to let AIUserIcons
 * know how it resolved iterating through its contained contacts based on their respective priorities in order to determine
 * which user icon should be used.  Tracking it here prevents needless repeated lookups of data.
 */
+ (void)setActualUserIcon:(NSImage *)userIcon andSource:(id <AIUserIconSource>)inSource forObject:(AIListObject *)inObject;

/*!
 * @brief Get the user icon source currently providing the icon for an object
 */
+ (id <AIUserIconSource>)userIconSourceForObject:(AIListObject *)inObject;

/*!
 * @brief Retrieve the manually set user icon (a stored preference) for an object, if there is one
 */
+ (NSData *)manuallySetUserIconDataForObject:(AIListObject *)inObject;

/*!
 * @brief Retreive the cached icon for an object, if there is one.
 */
+ (NSData *)cachedUserIconDataForObject:(AIListObject *)inObject;

/*!
 * @brief Returns if a cached user icon exists.
 *
 * @result YES if a cached user icon exists, NO otherwise.
 */
+ (BOOL)cachedUserIconExistsForObject:(AIListObject *)inObject;

/*!
 * @brief Retreive the serverside icon for an object, if there is one.
 */
+ (NSData *)serversideUserIconDataForObject:(AIListObject *)inObject;

/*!
 * @brief Get the user icon for an object
 *
 * If it's not already cached, the icon sources will be queried as needed.
 */
+ (NSImage *)userIconForObject:(AIListObject *)inObject;

/*
 * @brief Retrieve a user icon sized for the contact list
 *
 * @param inObject The object
 * @param size Size of the returned image. If this is the size passed to -[self setListUserIconSize:], a cache will be used.
 */
+ (NSImage *)listUserIconForContact:(AIListObject *)inObject size:(NSSize)size;

/*!
 * @brief Retrieve a user icon sized for a menu
 *
 * Returns the appropriate service icon if no user icon is found
 */
+ (NSImage *)menuUserIconForObject:(AIListObject *)inObject;

/*!
 * @brief Inform AIUserIcons a new manually-set icon data for an object
 *
 * We take responsibility (via AIManuallySetUserIconSource) for saving the data
 */
+ (void)setManuallySetUserIconData:(NSData *)inData forObject:(AIListObject *)inObject;

/*!
 * @brief Inform AIUserIcons of new serverside icon data for an object.
 *
 * This is likely called by a contact for itself or by an accont for a contact.
 */
+ (void)setServersideIconData:(NSData *)inData forObject:(AIListObject *)inObject notify:(NotifyTiming)notify;

/*
 * @brief Set the current contact list user icon size
 * This determines the size at which images are cached for listUserIconForContact:size:
 */
+ (void)setListUserIconSize:(NSSize)inSize;

/*!
 * @brief Clear the cache for a specific object
 */
+ (void)flushCacheForObject:(AIListObject *)inContact;

/*!
 * @brief Clear all caches
 */
+ (void)flushAllCaches;

@end

@interface AIUserIcons (AIUserIconSource)
+ (void)userIconSource:(id <AIUserIconSource>)inSource didDetermineUserIcon:(NSImage *)inUserIcon forObject:(AIListObject *)inObject;
@end
