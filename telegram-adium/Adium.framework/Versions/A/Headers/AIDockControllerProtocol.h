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

#import <Adium/AIControllerProtocol.h>

#define PREF_GROUP_APPEARANCE		@"Appearance"

#define KEY_ACTIVE_DOCK_ICON		@"Dock Icon"
#define FOLDER_DOCK_ICONS			@"Dock Icons"

#define KEY_ANIMATE_DOCK_ICON		@"Animate Dock Icon on Unread Messages"
#define KEY_BADGE_DOCK_ICON			@"Badge Dock Icon on Unread Messages"

@class AIIconState;

typedef enum {
    AIDockBehaviorStopBouncing = 0,
    AIDockBehaviorBounceOnce,
    AIDockBehaviorBounceRepeatedly,
    AIDockBehaviorBounceDelay_FiveSeconds,
    AIDockBehaviorBounceDelay_TenSeconds,
    AIDockBehaviorBounceDelay_FifteenSeconds,
    AIDockBehaviorBounceDelay_ThirtySeconds,
    AIDockBehaviorBounceDelay_OneMinute
} AIDockBehavior;

@protocol AIDockController <AIController>
//Icon animation & states
- (void)setIconStateNamed:(NSString *)inName;
- (void)removeIconStateNamed:(NSString *)inName;
- (void)setIconState:(AIIconState *)iconState named:(NSString *)inName;
- (NSImage *)baseApplicationIconImage;
- (void)setOverlay:(NSImage *)image;

//Special access to icon pack loading
- (NSArray *)availableDockIconPacks;
- (BOOL)currentIconSupportsIconStateNamed:(NSString *)inName;
- (NSMutableDictionary *)iconPackAtPath:(NSString *)folderPath;
- (void)getName:(NSString **)outName previewState:(AIIconState **)outIconState forIconPackAtPath:(NSString *)folderPath;
- (AIIconState *)previewStateForIconPackAtPath:(NSString *)folderPath;

//Bouncing & behavior
- (BOOL)performBehavior:(AIDockBehavior)behavior;
- (NSString *)descriptionForBehavior:(AIDockBehavior)behavior;
@end
