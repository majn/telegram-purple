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
#import <Adium/AIPreferencePane.h>

//Preference groups
#define PREF_GROUP_GENERAL              @"General"
#define PREF_GROUP_ACCOUNTS             @"Accounts"
#define PREF_GROUP_TOOLBARS 			@"Toolbars"
#define PREF_GROUP_WINDOW_POSITIONS     @"Window Positions"
#define PREF_GROUP_SPELLING 			@"Spelling"
#define OBJECT_PREFS_PATH               @"ByObject"			//Path to object specific preference folder
#define ACCOUNT_PREFS_PATH              @"Accounts"			//Path to account specific preference folder

@class AIAdium, AIListObject;
@class AIPreferencePane, AIAdvancedPreferencePane;

@protocol AIPreferenceController <AIController>
//Preference Window
- (IBAction)showPreferenceWindow:(id)sender;
- (IBAction)closePreferenceWindow:(id)sender;
- (void)openPreferencesToCategoryWithIdentifier:(NSString *)identifier;
- (void)addPreferencePane:(AIPreferencePane *)inPane;
- (void)removePreferencePane:(AIPreferencePane *)inPane;
- (NSArray *)paneArray;
- (NSArray *)paneArrayForCategory:(AIPreferenceCategory)paneCategory;

//Observing
- (void)registerPreferenceObserver:(id)observer forGroup:(NSString *)group;
- (void)unregisterPreferenceObserver:(id)observer;

- (void)addObserver:(NSObject *)anObserver forKeyPath:(NSString *)keyPath options:(NSKeyValueObservingOptions)options context:(void *)context;
- (void)addObserver:(NSObject *)anObserver forKeyPath:(NSString *)keyPath ofObject:(AIListObject *)listObject options:(NSKeyValueObservingOptions)options context:(void *)context;

- (void)informObserversOfChangedKey:(NSString *)key inGroup:(NSString *)group object:(AIListObject *)object;
- (void)delayPreferenceChangedNotifications:(BOOL)inDelay;

//Setting Preferences
- (void)setPreference:(id)value forKey:(NSString *)key group:(NSString *)group;
- (void)setPreference:(id)value forKey:(NSString *)inKey group:(NSString *)group object:(AIListObject *)object;
- (void)setPreferences:(NSDictionary *)inPrefDict inGroup:(NSString *)group;
- (void)setPreferences:(NSDictionary *)inPrefDict inGroup:(NSString *)group object:(AIListObject *)object;

//Retrieving Preferences
- (id)preferenceForKey:(NSString *)key group:(NSString *)group;
- (id)preferenceForKey:(NSString *)key group:(NSString *)group object:(AIListObject *)object;
- (id)preferenceForKey:(NSString *)key group:(NSString *)group objectIgnoringInheritance:(AIListObject *)object;
- (NSDictionary *)preferencesForGroup:(NSString *)group;
- (id)defaultPreferenceForKey:(NSString *)key group:(NSString *)group object:(AIListObject *)object;

//Defaults
- (void)registerDefaults:(NSDictionary *)defaultDict forGroup:(NSString *)group;
- (void)registerDefaults:(NSDictionary *)defaultDict forGroup:(NSString *)group object:(AIListObject *)object;

//Default download location
- (NSString *)userPreferredDownloadFolder;
- (void)setUserPreferredDownloadFolder:(NSString *)path;
@end

@interface NSObject (AIPreferenceObserver)
- (void)preferencesChangedForGroup:(NSString *)group key:(NSString *)key
							object:(AIListObject *)object preferenceDict:(NSDictionary *)prefDict firstTime:(BOOL)firstTime;
@end
