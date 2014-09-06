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

//Notifications
#define AIServiceIconSetDidChangeNotification	@"AIServiceIconSetDidChangeNotification"

typedef enum {
	AIServiceIconSmall = 0,		//Interface small
	AIServiceIconLarge,			//Interface large
	AIServiceIconList			//Contact List
} AIServiceIconType;
#define NUMBER_OF_SERVICE_ICON_TYPES 	3

typedef enum {
	AIIconNormal = 0,
	AIIconFlipped
} AIIconDirection;
#define NUMBER_OF_ICON_DIRECTIONS		2

@class AIService, AIListObject;

@interface AIServiceIcons : NSObject {

}

+ (NSImage *)serviceIconForObject:(AIListObject *)inObject type:(AIServiceIconType)iconType direction:(AIIconDirection)iconDirection;
+ (NSImage *)serviceIconForService:(AIService *)service type:(AIServiceIconType)iconType direction:(AIIconDirection)iconDirection;
+ (NSImage *)serviceIconForServiceID:(NSString *)serviceID type:(AIServiceIconType)iconType direction:(AIIconDirection)iconDirection;
+ (NSString *)pathForServiceIconForServiceID:(NSString *)serviceID type:(AIServiceIconType)iconType;
+ (BOOL)setActiveServiceIconsFromPath:(NSString *)inPath;

+ (NSImage *)previewMenuImageForIconPackAtPath:(NSString *)inPath;

@end
