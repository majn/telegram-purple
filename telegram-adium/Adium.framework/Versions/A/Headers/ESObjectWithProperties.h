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


@class AIMutableOwnerArray;

typedef enum {
	NotifyNever = -9999,
	NotifyLater = NO,   /* 0 */
	NotifyNow = YES		/* 1 */
} NotifyTiming;

#define KEY_KEY		@"Key"
#define KEY_VALUE	@"Value"

@interface ESObjectWithProperties : NSObject {
	NSMutableDictionary		*propertiesDictionary;
	NSMutableSet			*changedProperties;		//Properties that have changed since the last notification
	
	NSMutableDictionary		*displayDictionary;		//A dictionary of values affecting this object's display
	
	NSMutableSet *proxyObjects;
}

//Setting properties
- (void)setValue:(id)value forProperty:(NSString *)key notify:(NotifyTiming)notify;
- (void)setValue:(id)value forProperty:(NSString *)key afterDelay:(NSTimeInterval)delay;
- (void)notifyOfChangedPropertiesSilently:(BOOL)silent;

//Getting properties
@property (readonly, nonatomic) NSDictionary *properties;
- (id)valueForProperty:(NSString *)key;
- (NSInteger)integerValueForProperty:(NSString *)key;
- (int)intValueForProperty:(NSString *)key;
- (BOOL)boolValueForProperty:(NSString *)key;
- (NSNumber *)numberValueForProperty:(NSString *)key;

//Properties: Specifically for subclasses
- (void)object:(id)inObject didChangeValueForProperty:(NSString *)key notify:(NotifyTiming)notify;
- (void)didModifyProperties:(NSSet *)keys silent:(BOOL)silent;
- (void)didNotifyOfChangedPropertiesSilently:(BOOL)silent;

//Display array
- (AIMutableOwnerArray *)displayArrayForKey:(NSString *)inKey;
- (AIMutableOwnerArray *)displayArrayForKey:(NSString *)inKey create:(BOOL)create;
- (id)displayArrayObjectForKey:(NSString *)inKey;

//Name
@property (readonly, nonatomic, retain) NSString *displayName;

@property (readonly, nonatomic) NSString *internalObjectID;

//Mutable owner array delegate method
- (void)mutableOwnerArray:(AIMutableOwnerArray *)inArray didSetObject:(id)anObject withOwner:(id)inOwner priorityLevel:(float)priority;

- (NSSet *)proxyObjects;
- (void)noteProxyObject:(id)proxyObject;

@end
