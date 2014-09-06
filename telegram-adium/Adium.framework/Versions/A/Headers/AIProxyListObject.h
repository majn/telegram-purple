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

@class ESObjectWithProperties, MAZeroingWeakRef;
@protocol AIContainingObject;

@interface AIProxyListObject : NSObject {
	AIListObject *__weak listObject;
    ESObjectWithProperties <AIContainingObject> *__weak containingObject;
	NSString *key;
	NSString *cachedDisplayNameString;
	NSAttributedString *cachedDisplayName;
	NSDictionary *cachedLabelAttributes;
	NSSize cachedDisplayNameSize;
	NSString *nick;
}
@property (nonatomic, copy) NSDictionary *cachedLabelAttributes;
@property (nonatomic, strong) NSString *cachedDisplayNameString;
@property (nonatomic, strong) NSAttributedString *cachedDisplayName;
@property (nonatomic) NSSize cachedDisplayNameSize;

@property (nonatomic, strong) NSString *key;
@property (nonatomic, strong) NSString *nick;

@property (nonatomic, weak) AIListObject *listObject;
@property (nonatomic, weak) ESObjectWithProperties <AIContainingObject> * containingObject;

+ (AIProxyListObject *)proxyListObjectForListObject:(ESObjectWithProperties *)inListObject
									   inListObject:(ESObjectWithProperties<AIContainingObject> *)containingObject;

+ (AIProxyListObject *)existingProxyListObjectForListObject:(ESObjectWithProperties *)inListObject
											   inListObject:(ESObjectWithProperties <AIContainingObject>*)inContainingObject;

+ (AIProxyListObject *)proxyListObjectForListObject:(AIListObject *)inListObject
									   inListObject:(ESObjectWithProperties <AIContainingObject>*)inContainingObject
										   withNick:(NSString *)inNick;

/*!
 * @brief Called when an AIListObject is done with an AIProxyListObject to remove it from the global dictionary
 */
+ (void)releaseProxyObject:(AIProxyListObject *)proxyObject;

/*!
 * @brief Clear out cached display information; should be called when the AIProxyListObject may be used later
 */
- (void)flushCache;

@end
