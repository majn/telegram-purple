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

#define AIMenuDidChange  @"AIMenuDidChange"

/* Each of the items in this enum must correspond to an NSMenuItem which is:
 *		1. Declared in the AIMenuController.h interface
 *		2. Connected to AIMenuController in MainMenu.nib
 *		3. Be included in the locationArray in -[AIMenuController awakeFromNib]
 *
 * If the menu item isn't the first in its menu, it can be connected to the Quit Adium menu item; AIMenuController will move it
 * to the appropriate place.
 */
typedef enum {
    LOC_Adium_About = 0, LOC_Adium_Preferences, LOC_Adium_Other,
    LOC_File_New, LOC_File_Close, LOC_File_Save, LOC_File_Accounts, LOC_File_Additions,
    LOC_Edit_Bottom, LOC_Edit_Links, LOC_Edit_Additions,
	LOC_View_General, LOC_View_Sorting, LOC_View_Toggles, LOC_View_Counting_Toggles, LOC_View_Appearance_Toggles, LOC_View_Additions, 
	LOC_Display_General, LOC_Display_Jump, LOC_Display_MessageControl,
    LOC_Contact_Manage, LOC_Contact_Info, LOC_Contact_Action, LOC_Contact_NegativeAction, LOC_Contact_Additions,
	LOC_Status_State, LOC_Status_SocialNetworking, LOC_Status_Accounts, LOC_Status_Additions,
    LOC_Format_Styles, LOC_Format_Palettes, LOC_Format_Additions, 
    LOC_Window_Top, LOC_Window_Commands, LOC_Window_Auxiliary, LOC_Window_Fixed,
    LOC_Help_Local, LOC_Help_Web, LOC_Help_Additions,
    LOC_Dock_Status
} AIMenuLocation;

typedef enum {
    Context_Group_AttachDetach,Context_Contact_AttachDetach,Context_Group_Manage,Context_Contact_Manage,Context_Contact_Action, Context_Contact_NegativeAction,
    Context_Contact_Message, Context_Contact_Additions, Context_Contact_ChatAction, Context_Contact_Stranger_ChatAction, Context_Contact_ListAction,
	Context_Contact_GroupChat_ParticipantAction, Context_GroupChat_Manage, Context_GroupChat_Action,
	Context_Tab_Action,
	Context_TextView_LinkEditing, Context_TextView_Edit
} AIContextMenuLocation;

@class AIListObject, AIChat;

@protocol AIMenuController <AIController>
//Custom menu items
- (void)addMenuItem:(NSMenuItem *)newItem toLocation:(AIMenuLocation)location;
- (void)removeMenuItem:(NSMenuItem *)targetItem;

- (void)delayMenuItemPostProcessing;
- (void)endDelayMenuItemPostProcessing;

//Contextual menu items
- (void)addContextualMenuItem:(NSMenuItem *)newItem toLocation:(AIContextMenuLocation)location;
- (NSMenu *)contextualMenuWithLocations:(NSArray *)inLocationArray forListObject:(AIListObject *)inObject;
- (NSMenu *)contextualMenuWithLocations:(NSArray *)inLocationArray forChat:(AIChat *)inObject;
- (NSMenu *)contextualMenuWithLocations:(NSArray *)inLocationArray forListObject:(AIListObject *)inObject inChat:(AIChat *)inChat;
@property (nonatomic, readonly) AIListObject *currentContextMenuObject;
@property (nonatomic, readonly) AIChat *currentContextMenuChat;
- (NSMenu *)contextualMenuWithLocations:(NSArray *)inLocationArray;

//Control over the italics menu item
- (void)removeItalicsKeyEquivalent;
- (void)restoreItalicsKeyEquivalent;
@end

@interface NSObject (MenuItemUpdating)
- (void)menu:(NSMenu *)menu needsUpdateForMenuItem:(NSMenuItem *)menuItem;
@end

