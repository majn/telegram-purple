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

#define Interface_ContactSelectionChanged				@"Interface_ContactSelectionChanged"

/*!
 * @brief AIContactInfoInspectorDidChangeInspectedObject notification name
 *
 * userinfo is an NSDictionary with keys KEY_PREVIOUS_INSPECTED_OBJECT and KEY_NEW_INSPECTED_OBJECT and objects of class AIListObject
 */
#define AIContactInfoInspectorDidChangeInspectedObject	@"AIContactInfoInspectorWillChangeInspectedObject"
#define KEY_PREVIOUS_INSPECTED_OBJECT	@"PreviousInspectedObject"
#define KEY_NEW_INSPECTED_OBJECT		@"NewInspectedObject"

#define Interface_SendEnteredMessage				@"Interface_SendEnteredMessage"
#define Interface_WillSendEnteredMessage 			@"Interface_WillSendEnteredMessage"
#define Interface_DidSendEnteredMessage				@"Interface_DidSendEnteredMessage"
#define Interface_ShouldDisplayErrorMessage			@"Interface_ShouldDisplayErrorMessage"
#define Interface_ShouldDisplayQuestion				@"Interface_ShouldDisplayQuestion"
#define Interface_ContactListDidBecomeMain			@"Interface_ContactListDidBecomeMain"
#define Interface_ContactListDidResignMain			@"Interface_contactListDidResignMain"
#define Interface_ContactListDidClose				@"Interface_contactListDidClose"
#define Interface_TabArrangingPreferenceChanged		@"Interface_TabArrangingPreferenceChanged"
#define AIViewDesiredSizeDidChangeNotification		@"AIViewDesiredSizeDidChangeNotification"

#define PREF_GROUP_INTERFACE			@"Interface"
#define KEY_TABBED_CHATTING				@"Tabbed Chatting"
#define KEY_GROUP_CHATS_BY_GROUP		@"Group Chats By Group"
#define KEY_SAVE_CONTAINERS				@"Save Containers On Quit"
#define KEY_CONTAINERS					@"Containers"

#define KEY_CL_WINDOW_LEVEL					@"Window Level"
#define KEY_CL_HIDE							@"Hide While in Background"
#define KEY_CL_EDGE_SLIDE					@"Hide On Screen Edges"
#define KEY_CL_FLASH_UNVIEWED_CONTENT		@"Flash Unviewed Content"
#define KEY_CL_ANIMATE_CHANGES				@"Animate Changes"
#define KEY_CL_SHOW_TOOLTIPS				@"Show Tooltips"
#define KEY_CL_SHOW_TOOLTIPS_IN_BACKGROUND	@"Show Tooltips in Background"
#define KEY_CL_WINDOW_HAS_SHADOW			@"Window Has Shadow"

#define PREF_GROUP_CONFIRMATIONS		@"Confirmations"
#define KEY_CONFIRM_QUIT_FT				@"Suppress Quit Confirmation for File Transfers"
#define KEY_CONFIRM_QUIT_UNREAD			@"Suppress Quit Confirmation for Unread Messages"
#define KEY_CONFIRM_MSG_CLOSE			@"Confirm Message Window Close"
#define KEY_CONFIRM_LOGGED_OTR			@"Confirm Logging When Using OTR"
#define KEY_CONFIRM_SEND_CRASH			@"Suppress Send Crash Reports"

typedef enum {
	AINormalWindowLevel = 0,
	AIFloatingWindowLevel = 1,
	AIDesktopWindowLevel = 2
} AIWindowLevel;

//Identifiers for the various message views
typedef enum {
	DCStandardMessageView = 1,	//webkit is not available
	DCWebkitMessageView			//Preferred message view
} DCMessageViewType;

typedef enum {
	AITextAndButtonsDefaultReturn			= 1,
	AITextAndButtonsAlternateReturn			= 0,
	AITextAndButtonsOtherReturn				= -1,
	AITextAndButtonsClosedWithoutResponse	= -2
} AITextAndButtonsReturnCode;

typedef enum {
	AITextAndButtonsWindowButtonDefault = 0,
	AITextAndButtonsWindowButtonAlternate,
	AITextAndButtonsWindowButtonOther
} AITextAndButtonsWindowButton;

@protocol AIInterfaceComponent, AIContactListComponent, AIMessageDisplayController, AIMessageDisplayPlugin;
@protocol AIContactListTooltipEntry, AIFlashObserver, AIPlugin;

@class AIListWindowController, AIMessageWindowController, AIMessageViewController;

@class AIChat, AIListObject, AIListGroup, AIContactList;

@protocol AIInterfaceController <AIController>
- (void)registerInterfaceController:(id <AIInterfaceComponent>)inController;
- (void)registerContactListController:(id <AIContactListComponent>)inController;

/*!	@brief	Implement handling of the reopen Apple Event.
 *
 *	@par	The reopen handler should respond by making sure that at least one of Adium's windows is visible.
 *
 *	@par	Adium.app's implementation handles this event this way:
 *
 *	@li	If there are no chat windows, shows the Contact List.
 *	@li	Else, if the foremost chat window and chat tab has unviewed content, make sure it stays foremost (bringing it forward of the Contact List, if necessary).
 *	@li	Else, if any chat window has unviewed content, bring foremost the chat window and chat tab with the most recent unviewed content.
 *	@li	Else, if all chat windows are minimized, unminimize one of them.
 *	@li	If the application is hidden, unhide it.
 *
 *	@return	A value suitable for returning from the \c NSApplication delegate method <code>applicationShouldHandleReopen:hasVisibleWindows:
</code>. Specifically: \c YES if AppKit should perform its usual response to the event; \c NO if AppKit should do nothing.
 */
- (BOOL)handleReopenWithVisibleWindows:(BOOL)visibleWindows;

//Contact List
/*! @name Contact List */
/* @{ */
/*!
 * @brief Brings contact list to the front
 */
- (IBAction)showContactList:(id)sender;
/*!
 * @brief Close the contact list window
 */
- (IBAction)closeContactList:(id)sender;
/*!
 * @brief Toggle the contact list
 *
 * Show the contact list if it's closed, bring it to the front if it's open but not in the front,
 * or close it if it's active.
 */
- (IBAction)toggleContactList:(id)sender;
/*!
 * @returns YES if contact list is visible and selected, otherwise NO
 */
@property (nonatomic, readonly) BOOL contactListIsVisibleAndMain;
/*!
 * @returns YES if contact list is visible, otherwise NO
 */
@property (nonatomic, readonly) BOOL contactListIsVisible;
/*! @} */

#pragma mark Detachable Contact List
/*!
 * @brief Create a new AIListWindowController to display a given contact list group in a detached window
 *
 * @result Created contact list controller for detached contact list
 */
- (AIListWindowController *)detachContactList:(AIContactList *)aContactList;

#pragma mark Messaging
/*!
 * @brief Opens tab or window for a chat (following user's preferences)
 *
 * @param inChat The chat. If already open, the chat will be brought to the front.
 */
- (void)openChat:(AIChat *)inChat;

/*!
 * @brief Opens a chat in a given container at a specific index
 *
 * NOTE: If the chat is already open, this won't move it and instead does nothing. Perhaps it should, though :)
 *
 * @param inChat The chat.
 * @param containerID The name of the container window.
 * @param index The index within that containter window.
 */
- (id)openChat:(AIChat *)inChat inContainerWithID:(NSString *)containerID atIndex:(NSUInteger)index;

/*!
 * @brief Move a chat to a new window container
 *
 * NOTE: The chat must already be open. That's a bug.
 */
- (void)moveChatToNewContainer:(AIChat *)inChat;

/*!
 * @brief Close the interface for a chat
 *
 * @param inChat The chat
 */
- (void)closeChat:(AIChat *)inChat;

/*!
 * @brief Consolidate all open chats into a single container
 */
- (void)consolidateChats;

/*!
 * @brief Active Chat property
 *
 * Setter brings the tab/window for a chat to the front and sets it as active
 * If no chat is active (a non-chat window is focused, or Adium is not focused), getter returns nil.
 */
@property (nonatomic, retain) AIChat *activeChat;

/*!
 * @brief Get the chat which was most recently active
 *
 * If -[self activeChat] is non-nil, this will be the same as activeChat. However, if no chat is active, so long
 * as any chat is open, this will return the chat most recently active.  If no chats are open, this will return nil.
 */
@property (nonatomic, readonly) AIChat *mostRecentActiveChat;

/*!
 * @brief Get all open chats
 *
 * @result The open chats. Returns an empty array if no chats are open.
 */
@property (nonatomic, readonly) NSArray *openChats;

/*!
 * @brief Get all open chats in a given container.
 *
 * @result The array of open chats. Returns nil if no container with containerID exists.
 */
- (NSArray *)openChatsInContainerWithID:(NSString *)containerID;

/*!
 * @brief The container ID for a chat
 *
 * @param chat The chat to look up
 * @returns The container ID for the container the chat is in.
 */
- (NSString *)containerIDForChat:(AIChat *)chat;

/*!
 * @brief Get an array of the containerIDs of all open containers
 */
@property (nonatomic, readonly) NSArray *openContainerIDs;

/*!
 * @brief Open a new container with a given ID and name
 *
 * @param containerID The container's ID. Pass nil to let Adium generate one
 * @param containerName THe name of the container. Pass nil to not specify one.
 *
 * @result The newly created AIMessageWindowController
 */
- (AIMessageWindowController *)openContainerWithID:(NSString *)containerID name:(NSString *)containerName;

/*!
 * @brief Cycles to the next open chat, making it active
 *
 * This does not cross container boundaries. If there is no currently active chat, this has no effect.
 */
- (void)nextChat:(id)sender;

/*!
 * @brief Cycles to the previous open chat, making it active
 *
 * This does not cross container boundaries. If there is no currently active chat, this has no effect.
 */
- (void)previousChat:(id)sender;

#pragma mark Interface plugin callbacks
/*!
 * @brief A chat window did open: rebuild our window menu to show the new chat
 *
 * This should be called by the interface plugin (e.g. AIDualWindowInterfacePlugin) after a chat opens
 *
 * @param inChat Newly created chat 
 */
- (void)chatDidOpen:(AIChat *)inChat;

/*!
 * @brief A chat has become active: update our chat closing keys and flag this chat as selected in the window menu
 *
 * This should be called by the interface plugin (e.g. AIDualWindowInterfacePlugin) 
 *
 * @param inChat Chat which has become active
 */
- (void)chatDidBecomeActive:(AIChat *)inChat;

/*!
 * @brief A chat has become visible: send out a notification for components and plugins to take action
 *
 * This should be called by the interface plugin (e.g. AIDualWindowInterfacePlugin) 
 *
 * @param inChat Chat that has become active
 * @param inWindow Containing chat window
 */
- (void)chatDidBecomeVisible:(AIChat *)inChat inWindow:(NSWindow *)inWindow;

/*!
 * @brief A chat window did close: rebuild our window menu to remove the chat
 *
 * This should be called by the interface plugin (e.g. AIDualWindowInterfacePlugin) 
 * 
 * @param inChat Chat that closed
 */
- (void)chatDidClose:(AIChat *)inChat;

/*!
 * @brief The order of chats has changed: rebuild our window menu to reflect the new order
 *
 * This should be called by the interface plugin (e.g. AIDualWindowInterfacePlugin) 
 */
- (void)chatOrderDidChange;

#pragma mark Chat window access
/*!
 * @brief Find the window currently displaying a chat
 *
 * @returns Window for chat otherwise if the chat is not in any window, or is not visible in any window, returns nil
 */
- (NSWindow *)windowForChat:(AIChat *)inChat;


/*!
 * @brief Find the chat active in a window
 *
 * If the window does not have an active chat, nil is returned
 */
- (AIChat *)activeChatInWindow:(NSWindow *)window;

#pragma mark Interface selection
/*!
 * @brief Get the list object currently focused in the interface
 *
 * We find the first responder that knows about list objects and get its selected object.
 * This may be, for example, the contact list or a chat window.
 */
@property (readonly, nonatomic) AIListObject *selectedListObject;

/*!
 * @brief Get the list object currently selected in the contact list
 *
 * If multiple objects are selected, this returns the first one. If possible, use arrayOfSelectedListObjectsInContactList and
 * handle multiple selections.
 */
@property (readonly, nonatomic) AIListObject *selectedListObjectInContactList;

/*!
 * @brief Get the list objects currently selected in the contact list
 *
 * @result An NSArray of selected objects
 */
@property (readonly, nonatomic) NSArray *arrayOfSelectedListObjectsInContactList;

/*!
 * @brief Get the list objects currently selected in the contact list with their groups
 *
 * Each entry in the NSArray is an NSDictionary of the following layout:
 *
 * @"ListObject" => the AIListObject
 * @"ContainingObject" => the containing object
 *
 * @result An NSArray of selected objects
 */
@property (readonly, nonatomic) NSArray *arrayOfSelectedListObjectsWithGroupsInContactList;

#pragma mark Message View
/*!
 * @brief Register an object which can handle displaying messages
 *
 * See @protocol AIMessageDisplayPlugin for details. An example of such an object is the WebKit Message View plugin.
 *
 * @param inPlugin The object, which must conform to @protocol(AIMessageDisplayPlugin)
 */
- (void)registerMessageDisplayPlugin:(id <AIMessageDisplayPlugin>)inPlugin;

/*!
 * @brief Unregister an object previously registered with registerMessageDisplayPlugin.
 */
- (void)unregisterMessageDisplayPlugin:(id <AIMessageDisplayPlugin>)inPlugin;

/*!
 * @brief Get the AIMessageDisplayController-conforming object for a chat
 *
 * Every chat has exactly one. You can get it!
 */
- (id <AIMessageDisplayController>)messageDisplayControllerForChat:(AIChat *)inChat;

#pragma mark Error Display
- (void)handleErrorMessage:(NSString *)inTitle withDescription:(NSString *)inDesc;
- (void)handleMessage:(NSString *)inTitle withDescription:(NSString *)inDesc withWindowTitle:(NSString *)inWindowTitle;

#pragma mark Question Display
- (void)displayQuestion:(NSString *)inTitle withAttributedDescription:(NSAttributedString *)inDesc withWindowTitle:(NSString *)inWindowTitle
		  defaultButton:(NSString *)inDefaultButton alternateButton:(NSString *)inAlternateButton otherButton:(NSString *)inOtherButton suppression:(NSString *)inSuppression
responseHandler:(void (^)(AITextAndButtonsReturnCode ret, BOOL suppressed, id userInfo))handler;
- (void)displayQuestion:(NSString *)inTitle withDescription:(NSString *)inDesc withWindowTitle:(NSString *)inWindowTitle
		  defaultButton:(NSString *)inDefaultButton alternateButton:(NSString *)inAlternateButton otherButton:(NSString *)inOtherButton suppression:(NSString *)inSuppression
responseHandler:(void (^)(AITextAndButtonsReturnCode ret, BOOL suppressed, id userInfo))handler;

- (void)displayQuestion:(NSString *)inTitle withAttributedDescription:(NSAttributedString *)inDesc withWindowTitle:(NSString *)inWindowTitle
		  defaultButton:(NSString *)inDefaultButton alternateButton:(NSString *)inAlternateButton otherButton:(NSString *)inOtherButton suppression:(NSString *)inSuppression
				makeKey:(BOOL)key responseHandler:(void (^)(AITextAndButtonsReturnCode ret, BOOL suppressed, id userInfo))handler;
- (void)displayQuestion:(NSString *)inTitle withDescription:(NSString *)inDesc withWindowTitle:(NSString *)inWindowTitle
		  defaultButton:(NSString *)inDefaultButton alternateButton:(NSString *)inAlternateButton otherButton:(NSString *)inOtherButton suppression:(NSString *)inSuppression
				makeKey:(BOOL)key responseHandler:(void (^)(AITextAndButtonsReturnCode ret, BOOL suppressed, id userInfo))handler;

#pragma mark Synchronized Flashing
- (void)registerFlashObserver:(id <AIFlashObserver>)inObserver;
- (void)unregisterFlashObserver:(id <AIFlashObserver>)inObserver;
- (int)flashState;

#pragma mark Tooltips
/*!
 * @brief Register a tooltip-supplying object
 *
 * @param inEntry The object which will be queried for information when building a tooltip. Must conform to the AIContactListTooltipEntry protocol.
 * @param isSecondary Most tooltips are secondary; this means they are displayed in the bottom half of the tooltip which can expand as needed.
 */
- (void)registerContactListTooltipEntry:(id <AIContactListTooltipEntry>)inEntry secondaryEntry:(BOOL)isSecondary;

/*!
 * @brief Unregister a previously reigstered tooltip-supplying object
 *
 * @param inEntry The object to unregister
 * @param isSecondary This should match the value passed to registerContactListTooltipEntry:secondaryEntry:
 */
- (void)unregisterContactListTooltipEntry:(id <AIContactListTooltipEntry>)inEntry secondaryEntry:(BOOL)isSecondary;

/*!
 * @brief Get an array of all primary contact list tooltip entries
 *
 * @result An NSArray of objects conforming to AIContactListTooltipEntry
 */
- (NSArray *)contactListTooltipPrimaryEntries;

/*!
 * @brief Get an array of all secondary contact list tooltip entries
 *
 * @result An NSArray of objects conforming to AIContactListTooltipEntry
 */
- (NSArray *)contactListTooltipSecondaryEntries;

/*!
 * @brief Show an object's tooltip at a given point
 *
 * If object is not nil, the tooltip box will be shown for that object at the given point. It is created if needed;
 * if there is an existing object tooltip, that window is moved to the given point and reconfigured for object.
 * This is a rich-text, information-rich tooltip, not a simple text tooltip as seen elsewhere in Mac OS X.
 *
 * @param object The object for which the tooltip should be shown. Pass nil to hide any existing tooltip.
 * @param point The point in screen coordinates for the tooltip's origin. If object is nil, this value is ignored.
 * @param inWindow The window from which the tooltip is originating. If object is nil, this value is ignored.
 */
- (void)showTooltipForListObject:(AIListObject *)object atScreenPoint:(NSPoint)point onWindow:(NSWindow *)inWindow;

#pragma mark Window level menu
/*!
 * @brief Build a menu of possible window levels
 *
 * Window levels may be, for example, 'Above other windows' or 'Normally'.
 *
 * @param target The target on which @selector(selectedWindowLevel:) will be called when a menu item is selected
 */
- (NSMenu *)menuForWindowLevelsNotifyingTarget:(id)target;

@end

//Controls a contact list view
@protocol AIContactListViewController <NSObject>	
- (NSView *)contactListView;
@end

//Manages contact list view controllers
@protocol AIContactListController <NSObject>	
- (id <AIContactListViewController>)contactListViewController;
@end

/*!
 * @protocol AIMessageDisplayController
 * @brief    The message display controller is responsible for, unsurprisingly, the actual display of messages.
 *
 * The display controller manages a view ("messageView") which will be inserted along with other UI elements such
 * as a text entry area into a window.  The Interface Plugin knows nothing about how the AIMessageDisplayController 
 * keeps its messageView up to date, nor should it, but knows that the view will show messages.
 *
 * The AIMessageDisplayController is informed when the message view which is using it is closing.
 *
 * This is, for example, the AIWebKitMessageViewController.
 */
@protocol AIMessageDisplayController <NSObject>
- (void)setChatContentSource:(NSString *)source;
- (NSString *)chatContentSource;
- (NSString *)contentSourceName; // Unique name for this particular style of "content source".

- (NSView *)messageView;
- (NSView *)messageScrollView;
- (void)messageViewIsClosing;
- (void)clearView;

- (void)jumpToPreviousMark;
- (BOOL)previousMarkExists;

- (void)jumpToNextMark;
- (BOOL)nextMarkExists;

- (void)jumpToFocusMark;
- (BOOL)focusMarkExists;

- (void)addMark;
- (void)markForFocusChange;
@end

/*
 * @protocol AIMessageDisplayPlugin
 * @brief    A AIMessageDisplayPlugin provides AIMessageDisplayController objects on demand.
 *
 * The WebKit display plugin is one example.
 */
@protocol AIMessageDisplayPlugin <NSObject, AIPlugin>	
- (id <AIMessageDisplayController>)messageDisplayControllerForChat:(AIChat *)inChat;
@end

@protocol AIContactListTooltipEntry <NSObject>
- (NSString *)labelForObject:(AIListObject *)inObject;
- (NSAttributedString *)entryForObject:(AIListObject *)inObject;
- (BOOL)shouldDisplayInContactInspector;
@end

@protocol AIFlashObserver <NSObject>
- (void)flash:(int)value;
@end

//Handles any attributed text entry
@protocol AITextEntryView 
- (void)setAttributedString:(NSAttributedString *)inAttributedString;
- (void)setTypingAttributes:(NSDictionary *)attrs;
- (BOOL)availableForSending;
- (AIChat *)chat;
@end

@protocol AIInterfaceComponent <NSObject>
- (void)openInterface;
- (void)closeInterface;
- (id)openChat:(AIChat *)chat inContainerWithID:(NSString *)containerID withName:(NSString *)containerName atIndex:(NSUInteger)index;
- (void)setActiveChat:(AIChat *)inChat;
- (void)moveChat:(AIChat *)chat toContainerWithID:(NSString *)containerID index:(NSUInteger)index;
- (void)moveChatToNewContainer:(AIChat *)inChat;
- (void)closeChat:(AIChat *)chat;
- (AIMessageWindowController *)openContainerWithID:(NSString *)containerID name:(NSString *)containerName;
/*!
 * @brief Return an array of NSDictionary objects for all open containers with associated information
 * 
 * The returned array has zero or more NSDictionary objects with the following information for each container
 *	Key			Value
 *	@"ID"		NSString of the containerID
 *	@"Frame"	NSString of the window's [NSWindow frame]
 *	@"Content"	NSArray of the AIChat objects within that container
 *	@"ActiveChat"	AIChat that is currently active
 *	@"Name"		NSString of the container's name
 */
@property (readonly, nonatomic) NSArray *openContainersAndChats;
@property (readonly, nonatomic) NSArray *openContainerIDs;
@property (readonly, nonatomic) NSArray *openChats;
- (NSArray *)openChatsInContainerWithID:(NSString *)containerID;
- (NSString *)containerIDForChat:(AIChat *)chat;
- (NSWindow *)windowForChat:(AIChat *)chat;
- (AIChat *)activeChatInWindow:(NSWindow *)window;
@end

/*!
 * @protocol AIInterfaceContainer
 * @brief This protocol is for a general interface element such as the contact list or the container of a chat
 */
@protocol AIInterfaceContainer <NSObject>
- (void)makeActive:(id)sender;	//Make the container active/front
- (void)close:(id)sender;	//Close the container
@end

/*!
 * @protocol AIChatViewController
 * @brief A AIChatViewController manages everything pertaining to a chat's view area
 *
 * It will manage this view, text input, and any associated controls.
 */
@protocol AIChatViewController <NSObject>
- (NSView *)view;
- (NSScrollView *)messagesScrollView;
- (AIChat *)chat;

/*!
 * @brief Show/hide the user list for a group chat.
 * Does nothing if not in a group chat.
 */
- (void)toggleUserList;

/*!
 * @brief Is the user list visible for a group chat?
 */
- (BOOL)userListVisible;

/*!
 * @brief Swap the side the user list is on for a group chat.
 * Does nothing if not in a group chat.
 */
- (void)toggleUserListSide;
@end

/*!
 * @protocol AIChatContainer
 * @brief This protocol is for an object which displays a single chat (e.g. a tab in a chat window)
 */
@protocol AIChatContainer <AIInterfaceContainer>
/*
 * @brief Get the window controller which holds this AIChatContainer
 */
@property (readonly, nonatomic, retain) AIMessageWindowController *windowController;

/*
 * @brief Get the view controller for this AIChatContainer
 */
@property (readonly, nonatomic) id <AIChatViewController> chatViewController;

/*!
 * @brief Get the message view controller for this AIChatContainer
 */
@property (readonly, nonatomic) AIMessageViewController *messageViewController;
@end

@protocol AIContactListComponent <NSObject>
/*!
 * @brief Show the contact list window and bring Adium to the front
 */
- (void)showContactListAndBringToFront:(BOOL)bringToFront;
- (BOOL)contactListIsVisibleAndMain;
- (BOOL)contactListIsVisible;
- (void)closeContactList;
@end

@protocol AIMultiContactListComponent <AIContactListComponent>
- (id)detachContactList:(AIContactList *)contactList;
- (void)nextDetachedContactList;
- (void)previousDetachedContactList;
@end

//Custom printing informal protocol
@interface NSObject (AdiumPrinting)
- (void)adiumPrint:(id)sender;
- (BOOL)validatePrintMenuItem:(NSMenuItem *)menuItem;
@end

@interface NSWindowController (AdiumBorderlessWindowClosing)
- (BOOL)windowPermitsClose;
@end
