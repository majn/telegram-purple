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
#import <Adium/AIStatusDefines.h>

@class AIStatus, AIAccount, AIStatusGroup, AIService;

//Status State Notifications
#define AIStatusStateArrayChangedNotification	@"AIStatusStateArrayChangedNotification"
#define AIStatusActiveStateChangedNotification	@"AIStatusActiveStateChangedNotification"

//Idle Notifications
#define AIMachineIsIdleNotification				@"AIMachineIsIdleNotification"
#define AIMachineIsActiveNotification			@"AIMachineIsActiveNotification"
#define AIMachineIdleUpdateNotification			@"AIMachineIdleUpdateNotification"

//Preferences
#define PREF_GROUP_SAVED_STATUS					@"Saved Status"
#define KEY_SAVED_STATUS						@"Saved Status Array"

#define KEY_STATUS_NAME							@"Status Name"
#define KEY_STATUS_DESCRIPTION					@"Status Description"
#define	KEY_STATUS_TYPE							@"Status Type"

#define PREF_GROUP_STATUS_PREFERENCES			@"Status Preferences"
#define KEY_STATUS_CONVERSATION_COUNT			@"Unread Conversations"
#define KEY_STATUS_MENTION_COUNT				@"Unread Mentions"
#define KEY_STATUS_REPORT_IDLE					@"Report Idle"
#define KEY_STATUS_REPORT_IDLE_INTERVAL			@"Report Idle Interval"
#define	KEY_STATUS_AUTO_AWAY					@"Auto Away"
#define KEY_STATUS_AUTO_AWAY_STATUS_STATE_ID	@"Auto Away Status State ID"
#define KEY_STATUS_FUS							@"Fast User Switching Auto Away"
#define KEY_STATUS_FUS_STATUS_STATE_ID			@"Fast User Switching Status State ID"
#define KEY_STATUS_SS							@"ScreenSaver Auto Away"
#define KEY_STATUS_SS_STATUS_STATE_ID			@"ScreenSaver Status State ID"
#define KEY_STATUS_AUTO_AWAY_INTERVAL			@"Auto Away Interval"

#define KEY_STATUS_SHOW_STATUS_WINDOW				@"Show Status Window"
#define KEY_STATUS_STATUS_WINDOW_ON_TOP				@"Status Window Always On Top"
#define KEY_STATUS_STATUS_WINDOW_HIDE_IN_BACKGROUND	@"Status Window Hide in Background"

//Built-in names and descriptions, which services should use when they support identical or approximately identical states
#define	STATUS_NAME_AVAILABLE				@"Generic Available"
#define STATUS_NAME_FREE_FOR_CHAT			@"Free for Chat"
#define STATUS_NAME_AVAILABLE_FRIENDS_ONLY	@"Available for Friends Only"

#define	STATUS_NAME_AWAY					@"Generic Away"
#define STATUS_NAME_EXTENDED_AWAY			@"Extended Away"
#define STATUS_NAME_AWAY_FRIENDS_ONLY		@"Away for Friends Only"
#define STATUS_NAME_DND						@"DND"
#define STATUS_NAME_NOT_AVAILABLE			@"Not Available"
#define STATUS_NAME_OCCUPIED				@"Occupied"
#define STATUS_NAME_BRB						@"BRB"
#define STATUS_NAME_BUSY					@"Busy"
#define STATUS_NAME_PHONE					@"Phone"
#define STATUS_NAME_LUNCH					@"Lunch"
#define STATUS_NAME_NOT_AT_HOME				@"Not At Home"
#define STATUS_NAME_NOT_AT_DESK				@"Not At Desk"
#define STATUS_NAME_NOT_IN_OFFICE			@"Not In Office"
#define STATUS_NAME_VACATION				@"Vacation"
#define STATUS_NAME_STEPPED_OUT				@"Stepped Out"

#define STATUS_NAME_INVISIBLE				@"Invisible"

#define STATUS_NAME_OFFLINE					@"Offline"

//Current version state ID string
#define STATE_SAVED_STATE					@"State"

@protocol AIStatusController <AIController>
/*!
 * @brief Register a status for a service
 *
 * Implementation note: Each AIStatusType has its own NSMutableDictionary, statusDictsByServiceCodeUniqueID.
 * statusDictsByServiceCodeUniqueID is keyed by serviceCodeUniqueID; each object is an NSMutableSet of NSDictionaries.
 * Each of these dictionaries has KEY_STATUS_NAME, KEY_STATUS_DESCRIPTION, and KEY_STATUS_TYPE.
 *
 * @param statusName A name which will be passed back to accounts of this service.  Internal use only.  Use the AIStatusController.h \#defines where appropriate.
 * @param description A human-readable localized description which will be shown to the user.  Use the AIStatusController.h \#defines where appropriate.
 * @param type An AIStatusType, the general type of this status.
 * @param service The AIService for which to register the status
 */
- (void)registerStatus:(NSString *)statusName
	   withDescription:(NSString *)description
				ofType:(AIStatusType)type 
			forService:(AIService *)service;
/*!
 * @brief Generate and return a menu of status types (Away, Be right back, etc.)
 *
 * @param service The service for which to return a specific list of types, or nil to return all available types
 * @param target The target for the menu items, which will have an action of \@selector(selectStatus:)
 *
 * @result The menu of statuses, separated by available and away status types
 */
- (NSMenu *)menuOfStatusesForService:(AIService *)service withTarget:(id)target;

@property (readonly, nonatomic) NSSet *flatStatusSet;
@property (readonly, nonatomic) NSArray *sortedFullStateArray;
@property (readonly, nonatomic) AIStatus *offlineStatusState;
@property (readonly, nonatomic) AIStatus *availableStatus;
@property (readonly, nonatomic) AIStatus *awayStatus;
@property (readonly, nonatomic) AIStatus *invisibleStatus;
@property (readonly, nonatomic) AIStatus *offlineStatus;
- (AIStatus *)statusStateWithUniqueStatusID:(NSNumber *)uniqueStatusID;

- (void)setActiveStatusState:(AIStatus *)state;
- (void)setActiveStatusState:(AIStatus *)state forAccount:(AIAccount *)account;
- (void)setDelayStatusMenuRebuilding:(BOOL)shouldDelay;
- (void)applyState:(AIStatus *)statusState toAccounts:(NSArray *)accountArray;
@property (readonly, nonatomic) AIStatus *activeStatusState;
- (NSSet *)allActiveStatusStates;
- (AIStatusType)activeStatusTypeTreatingInvisibleAsAway:(BOOL)invisibleIsAway;
- (NSSet *)activeUnavailableStatusesAndType:(AIStatusType *)activeUnvailableStatusType 
								   withName:(NSString **)activeUnvailableStatusName
			 allOnlineAccountsAreUnvailable:(BOOL *)allOnlineAccountsAreUnvailable;
- (AIStatus *)defaultInitialStatusState;

- (NSString *)descriptionForStateOfStatus:(AIStatus *)statusState;
- (NSString *)localizedDescriptionForCoreStatusName:(NSString *)statusName;
- (NSString *)localizedDescriptionForStatusName:(NSString *)statusName statusType:(AIStatusType)statusType;
- (NSString *)defaultStatusNameForType:(AIStatusType)statusType;

//State Editing
- (void)addStatusState:(AIStatus *)state;
- (void)removeStatusState:(AIStatus *)state;
- (void)statusStateDidSetUniqueStatusID;

//State menu support
- (void)setDelayActiveStatusUpdates:(BOOL)shouldDelay;
- (BOOL)removeIfNecessaryTemporaryStatusState:(AIStatus *)originalState;
- (AIStatusGroup *)rootStateGroup;

- (void)savedStatusesChanged;
@end
