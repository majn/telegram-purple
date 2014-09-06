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

#import <Adium/AIStatusItem.h>

@class AIStatus;

@interface AIStatusGroup : AIStatusItem {
	NSMutableArray		*containedStatusItems;
	NSMutableSet		*_flatStatusSet;
	NSArray             *_sortedContainedStatusItems;
	
	int					delaySavingAndNotification;
}

+ (id)statusGroup;
+ (id)statusGroupWithContainedStatusItems:(NSArray *)inContainedObjects;

- (void)setContainedStatusItems:(NSArray *)inContainedStatusItems;

- (void)addStatusItem:(AIStatusItem *)inStatusItem atIndex:(NSUInteger)index;
- (void)removeStatusItem:(AIStatusItem *)inStatusItem;
- (NSUInteger)moveStatusItem:(AIStatusItem *)statusState toIndex:(NSUInteger)destIndex;
- (void)replaceExistingStatusState:(AIStatus *)oldStatusState withStatusState:(AIStatus *)newStatusState;

- (NSArray *)containedStatusItems;
- (AIStatus *)anyContainedStatus;
- (NSSet *)flatStatusSet;
- (NSMenu *)statusSubmenuNotifyingTarget:(id)target action:(SEL)selector;

- (void)setDelaySavingAndNotification:(BOOL)inShouldDelay;
- (BOOL)enclosesStatusState:(AIStatus *)inStatusState;
- (BOOL)enclosesStatusStateInSet:(NSSet *)inSet;

+ (void)sortArrayOfStatusItems:(NSMutableArray *)inArray context:(void *)context;

@end
