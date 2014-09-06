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

#import <Adium/AIAbstractListObjectMenu.h>
#import <Adium/AIContactObserverManager.h>
#import <Adium/AIStatusMenu.h>

@class AIAccount;
@protocol AIAccountMenuDelegate;

typedef enum {
	AIAccountNoSubmenu = 0,
	AIAccountStatusSubmenu,
	AIAccountOptionsSubmenu
} AIAccountSubmenuType;

@interface AIAccountMenu : AIAbstractListObjectMenu <AIListObjectObserver, AIStatusMenuDelegate, NSMenuDelegate> {
	id<AIAccountMenuDelegate>				delegate;
	BOOL			delegateRespondsToDidSelectAccount;
	BOOL			delegateRespondsToShouldIncludeAccount;	

	BOOL			useSystemFont;
	BOOL			submenuType;
	BOOL			showTitleVerbs;
	BOOL			includeDisabledAccountsMenu;
	BOOL			includeAddAccountsMenu;
	BOOL			delegateRespondsToSpecialMenuItem;

	NSControlSize	controlSize;

	AIStatusMenu	*statusMenu;
}

+ (id)accountMenuWithDelegate:(id<AIAccountMenuDelegate>)inDelegate
				  submenuType:(AIAccountSubmenuType)inSubmenuType
			   showTitleVerbs:(BOOL)inShowTitleVerbs;

/*!	@brief	Whether to use the system font instead of the menu font.
 *
 *	@par	By default, menu items in the account menu use the menu font, but a client can request them with the system font instead.
 */
@property (readwrite, nonatomic) BOOL useSystemFont;

@property (readwrite, nonatomic, unsafe_unretained) id<AIAccountMenuDelegate> delegate;

- (NSMenuItem *)menuItemForAccount:(AIAccount *)account;

@end

@protocol AIAccountMenuDelegate <NSObject>
- (void)accountMenu:(AIAccountMenu *)inAccountMenu didRebuildMenuItems:(NSArray *)menuItems;

@optional
- (void)accountMenu:(AIAccountMenu *)inAccountMenu didSelectAccount:(AIAccount *)inAccount; 	
- (BOOL)accountMenu:(AIAccountMenu *)inAccountMenu shouldIncludeAccount:(AIAccount *)inAccount; 

/*!
 * @brief At what size will this menu be used?
 *
 * If not implemented, the default is NSRegularControlSize. NSMiniControlSize is not supported.
 */
- (NSControlSize)controlSizeForAccountMenu:(AIAccountMenu *)inAccountMenu; 

//Should the account menu include a submenu of services for adding accounts?
- (BOOL)accountMenuShouldIncludeAddAccountsMenu:(AIAccountMenu *)inAccountMenu;			

//Does the menu require a special topmost item + seperator?
- (NSMenuItem *)accountMenuSpecialMenuItem:(AIAccountMenu *)inAccountMenu;			

//Should the account menu include a submenu of 'disabled accounts'?
- (BOOL)accountMenuShouldIncludeDisabledAccountsMenu:(AIAccountMenu *)inAccountMenu;			
@end
