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

@class AIListObject, AIChat, AIActionDetailsPane;

@protocol AIEventHandler, AIActionHandler;

//Event preferences
#define PREF_GROUP_CONTACT_ALERTS	@"Contact Alerts"
#define KEY_CONTACT_ALERTS			@"Contact Alerts"
#define KEY_DEFAULT_EVENT_ID		@"Default Event ID"
#define KEY_DEFAULT_ACTION_ID		@"Default Action ID"

//Event Dictionary keys
#define	KEY_EVENT_ID				@"EventID"
#define	KEY_ACTION_ID				@"ActionID"
#define	KEY_ACTION_DETAILS			@"ActionDetails"
#define KEY_ONE_TIME_ALERT			@"OneTime"

typedef enum {
	AIContactsEventHandlerGroup = 0,
	AIMessageEventHandlerGroup,
	AIAccountsEventHandlerGroup,
	AIFileTransferEventHandlerGroup,
	AIOtherEventHandlerGroup
} AIEventHandlerGroupType;
#define EVENT_HANDLER_GROUP_COUNT 5

@protocol AIContactAlertsController <AIController>
/*!
 * @brief Register an event
 *
 * An event must have a unique eventID. handler is responsible for providing information
 * about the event, such as short and long descriptions. The group determines how the event will be displayed in the events
 * preferences; events in the same group are displayed together.
 *
 * @param eventID Unique event ID
 * @param handler The handler, which must conform to AIEventHandler
 * @param inGroup The group
 * @param global If YES, the event will only be displayed in the global Events preferences; if NO, the event is available for contacts and groups via Get Info, as well.
 */
- (void)registerEventID:(NSString *)eventID withHandler:(id <AIEventHandler>)handler inGroup:(AIEventHandlerGroupType)inGroup globalOnly:(BOOL)global;

/*!
 * @brief Generate an event, returning a set of the actionIDs which were performed.
 *
 * This should be called when eventID occurs; it triggers all associated actions.
 *
 * @param eventID The event which occurred
 * @param listObject The object for which the event occurred
 * @param userInfo Event-specific user info
 * @param previouslyPerformedActionIDs If non-nil, a set of actionIDs which should be treated as if they had already been performed in this invocation.
 *
 * @result The set of actions which were performed, suitable for being passed back in for another event generation via previouslyPerformedActionIDs
 */
- (NSSet *)generateEvent:(NSString *)eventID forListObject:(AIListObject *)listObject userInfo:(id)userInfo previouslyPerformedActionIDs:(NSSet *)previouslyPerformedActionIDs;

/*!
 * @brief Return all event IDs
 */
- (NSArray *)allEventIDs;

/*!
 * @brief Return all event IDs for groups/contacts (but not global ones)
 */
- (NSArray *)nonGlobalEventIDs;

/*!
 * @brief Returns a menu of all events
 * 
 * A menu item's represented object is the dictionary describing the event it represents
 *
 * @param target The target on which @selector(selectEvent:) will be called on selection.
 * @param global If YES, the events listed will include global ones (such as Error Occurred) in addition to contact-specific ones.
 * @result An NSMenu of the events
 */
- (NSMenu *)menuOfEventsWithTarget:(id)target forGlobalMenu:(BOOL)global;

/*!
 * @brief Sort an array of event IDs
 *
 * Given an array of various event IDs, this sorts them by category.
 *
 * @brief inArray The array of eventIDs to sort
 * @return Sorted version of inArray
 */
- (NSArray *)sortedArrayOfEventIDsFromArray:(NSArray *)inArray;

/*!
 * @brief Return the default event ID for a new alert
 */
- (NSString *)defaultEventID;

#pragma mark Descriptions

/*!
 * @brief Find the eventID associated with an English name
 *
 * This exists for compatibility with old AdiumXtras...
 */
- (NSString*)eventIDForEnglishDisplayName:(NSString *)displayName;

/*!
 * @brief Return a short description to describe eventID when considered globally
 */
- (NSString *)globalShortDescriptionForEventID:(NSString *)eventID;

/*!
 * @brief Return a description to describe eventID; this is more verbose than the short description
 * @param listObject The object for which the eventID should be described. If nil, it will be described globally.
 */
- (NSString *)longDescriptionForEventID:(NSString *)eventID forListObject:(AIListObject *)listObject;

/*!
 * @brief Return a natural language, localized description for an event
 *
 * This will be suitable for display to the user such as in a message window or a Growl notification
 *
 * @param eventID The event
 * @param listObject The object for which the event occurred
 * @param userInfo Event-specific userInfo
 * @param includeSubject If YES, the return value is a complete sentence. If NO, the return value is suitable for display after a name or other identifier.
 * @result The natural language description
 */
- (NSString *)naturalLanguageDescriptionForEventID:(NSString *)eventID
										listObject:(AIListObject *)listObject
										  userInfo:(id)userInfo
									includeSubject:(BOOL)includeSubject;

/*!
 * @brief Return the image associated with an event
 */
- (NSImage *)imageForEventID:(NSString *)eventID;

/*!
 * @brief The description for multiple combined events.
 *
 * @param eventID The event
 * @param listObject The object for which the event references
 * @param chat The chat for which the event references
 * @param count The count of combined events
 *
 * @return The description of the event
 *
 */
- (NSString *)descriptionForCombinedEventID:(NSString *)eventID
							  forListObject:(AIListObject *)listObject
									forChat:(AIChat *)chat
								  withCount:(NSUInteger)count;

#pragma mark Event types
/*!
 * @brief Is the passed event a message event?
 *
 * Examples of messages events are "message sent" and "message received."
 *
 * @result YES if it is a message event
 */
- (BOOL)isMessageEvent:(NSString *)eventID;

/*!
 * @brief Is the passed event a contact status event?
 *
 * Examples of messages events are "contact signed on" and "contact went away."
 *
 * @result YES if it is a contact status event
 */
- (BOOL)isContactStatusEvent:(NSString *)eventID;

#pragma mark Actions
/*!
 * @brief Register an actionID and its handler
 *
 * When an event occurs -- that is, when the event is generated via
 * -[id<AIContactAlertsController> generateEvent:forListObject:userInfo:] -- the handler for each action
 * associated with that event within the appropriate list object's heirarchy (object -> containing group -> global)
 * will be called as per the AIActionHandler protocol.
 *
 * @param actionID The actionID
 * @param handler The handler, which must conform to the AIActionHandler protocol
 */
- (void)registerActionID:(NSString *)actionID withHandler:(id <AIActionHandler>)handler;

/*!
 * @brief Return a dictionary whose keys are action IDs and whose objects are objects conforming to AIActionHandler
 */
- (NSDictionary *)actionHandlers;

/*!
 * @brief Returns a menu of all actions
 *
 * A menu item's represented object is the dictionary describing the action it represents
 *
 * @param target The target on which @selector(selectAction:) will be called on selection
 * @result The NSMenu, which does not send validateMenuItem: messages
 */
- (NSMenu *)menuOfActionsWithTarget:(id)target;

/*!
 * @brief Return the default action ID for a new alert
 */
- (NSString *)defaultActionID;

#pragma mark Alerts
/*!
 * @brief Returns an array of all the alerts of a given list object
 *
 * This calls alertsForListObject:withEventID:actionID with nil for eventID and actionID.
 *
 * @param listObject The object
 */
- (NSArray *)alertsForListObject:(AIListObject *)listObject;

/*!
 * @brief Return an array of all alerts for a list object with filtering
 *
 * @param listObject The object, or nil for global
 * @param eventID If specified, only return events matching eventID. If nil, don't filter based on events.
 * @param actionID If specified, only return actions matching actionID. If nil, don't filter based on actionID.
 */
- (NSArray *)alertsForListObject:(AIListObject *)listObject withEventID:(NSString *)eventID actionID:(NSString *)actionID;

/*!
 * @brief Add an alert (passed as a dictionary) to a list object
 *
 * @param newAlert The alert to add
 * @param listObject The object to which to add, or nil for global
 * @param setAsNewDefaults YES to make the type and details of newAlert be the new default for new alerts
 */
- (void)addAlert:(NSDictionary *)alert toListObject:(AIListObject *)listObject setAsNewDefaults:(BOOL)setAsNewDefaults;

/*!
 * @brief Add an alert at the global level
 */
- (void)addGlobalAlert:(NSDictionary *)newAlert;

/*!
 * @brief Remove an alert from a listObject
 *
 * @param victimAlert The alert to remove; it will be tested against existing alerts using isEqual: so must be identical
 * @param listObject The object (or nil, for global) from which to remove victimAlert
 */
- (void)removeAlert:(NSDictionary *)victimAlert fromListObject:(AIListObject *)listObject;

/*!
 * @brief Remove all current global alerts and replace them with the alerts in allGlobalAlerts
 *
 * Used for setting a preset of events
 */
- (void)setAllGlobalAlerts:(NSArray *)allGlobalAlerts;

/*!
 * @brief Remove all global (root-level) alerts with a given action ID
 */
- (void)removeAllGlobalAlertsWithActionID:(NSString *)actionID;

/*!
 * @brief Move all contact alerts from oldObject to newObject
 *
 * This is useful when adding oldObject to the metaContact newObject so that any existing contact alerts for oldObject
 * are applied at the contact-general level, displayed and handled properly for the new, combined contact.
 *
 * @param oldObject The object from which to move contact alerts
 * @param newObject The object to which to we want to add the moved contact alerts
 */
- (void)mergeAndMoveContactAlertsFromListObject:(AIListObject *)oldObject intoListObject:(AIListObject *)newObject;
@end

/*!
 * @protocol AIEventHandler
 * @brief Protocol for a class which posts and supplies information about an Event
 *
 * Example Events are Account Connected, Contact Signed On, New Message Received
 */
@protocol AIEventHandler <NSObject>
/*!
 * @brief Short description
 * @result A short localized description of the passed event
 */
- (NSString *)shortDescriptionForEventID:(NSString *)eventID;

/*!
 * @brief Global short description for an event
 * @result A short localized description of the passed event in the case that it is not associated with an object
 */
- (NSString *)globalShortDescriptionForEventID:(NSString *)eventID;

/*!
 * @brief English, non-translated global short description for an event
 *
 * This exists because old X(tras) relied upon matching the description of event IDs, and I don't feel like making
 * a converter for old packs.  If anyone wants to fix this situation, please feel free :)
 *
 * @result English global short description which should only be used internally
 */
- (NSString *)englishGlobalShortDescriptionForEventID:(NSString *)eventID;

/*!
 * @brief Long description for an event
 * @result A localized description of an event, for listObject if passed, which is more verbose than the short description.
 */
- (NSString *)longDescriptionForEventID:(NSString *)eventID forListObject:(AIListObject *)listObject;

/*!
 * @brief Natural language description for an event
 *
 * @param eventID The event identifier
 * @param listObject The listObject triggering the event
 * @param userInfo Event-specific userInfo
 * @param includeSubject If YES, return a full sentence.  If not, return a fragment.
 * @result The natural language description.
 */
- (NSString *)naturalLanguageDescriptionForEventID:(NSString *)eventID
										listObject:(AIListObject *)listObject
										  userInfo:(id)userInfo
									includeSubject:(BOOL)includeSubject;

/*!
 * @brief Return an image icon for the specified eventID.
 */
- (NSImage *)imageForEventID:(NSString *)eventID;

/*!
 * @brief The description for multiple combined events.
 *
 * @param eventID The event
 * @param listObject The object for which the event references
 * @param chat The chat for which the event references
 * @param count The count of combined events
 *
 * @return The description of the event
 *
 */
- (NSString *)descriptionForCombinedEventID:(NSString *)eventID
							  forListObject:(AIListObject *)listObject
									forChat:(AIChat *)chat
								  withCount:(NSUInteger)count;

@end

/*!
 * @protocol AIActionHandler
 * @brief Protocol for an Action which can be taken in response to an Event
 *
 * An action may optionally supply a details pane.  If it does, it can store information in a details dictionary
 * which will be passed back to the action when it is triggered as well as when it is queried for a  long description.
 *
 * Example Actions are Play Sound, Speak Event, Display Growl Notification
 */
@protocol AIActionHandler <NSObject>
/*!
 * @brief Short description
 * @result A short localized description of the action
 */
- (NSString *)shortDescriptionForActionID:(NSString *)actionID;

/*!
 * @brief Long description
 * @result A longer localized description of the action which should take into account the details dictionary as appropraite.
 */
- (NSString *)longDescriptionForActionID:(NSString *)actionID withDetails:(NSDictionary *)details;

/*!
 * @brief Image
 */
- (NSImage *)imageForActionID:(NSString *)actionID;

/*!
 * @brief Details pane
 * @result An <tt>AIActionDetailsPane</tt> to use for configuring this action, or nil if no configuration is possible.
 */
- (AIActionDetailsPane *)detailsPaneForActionID:(NSString *)actionID;

/*!
 * @brief Perform an action
 *
 * @param actionID The ID of the action to perform
 * @param listObject The listObject associated with the event triggering the action. It may be nil
 * @param details If set by the details pane when the action was created, the details dictionary for this particular action
 * @param eventID The eventID which triggered this action
 * @param userInfo Additional information associated with the event; userInfo's type will vary with the actionID.
 *
 * @result YES if the action was performed successfully.  If NO, other actions of the same type will be attempted even if allowMultipleActionsWithID: returns NO for eventID.
 */
- (BOOL)performActionID:(NSString *)actionID forListObject:(AIListObject *)listObject withDetails:(NSDictionary *)details triggeringEventID:(NSString *)eventID userInfo:(id)userInfo;

/*!
 * @brief Allow multiple actions?
 *
 * If this method returns YES, every one of this action associated with the triggering event will be executed.
 * If this method returns NO, only the first will be.
 *
 * Example of relevance: An action which plays a sound may return NO so that if the user has sound actions associated
 * with the "Message Received (Initial)" and "Message Received" events will hear the "Message Received (Initial)"
 * sound [which is triggered first] and not the "Message Received" sound when an initial message is received. If this
 * method returned YES, both sounds would be played.
 */
- (BOOL)allowMultipleActionsWithID:(NSString *)actionID;
@end
