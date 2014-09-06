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

@class AIAccount, AIListContact, AIListObject;

@protocol AIContactMenuDelegate;

@interface AIContactMenu : AIAbstractListObjectMenu <AIListObjectObserver,NSMenuDelegate> {
	AIListObject			*containingObject;
	
	id<AIContactMenuDelegate>						delegate;
	BOOL					delegateRespondsToDidSelectContact;
	BOOL					delegateRespondsToShouldIncludeContact;	
	BOOL					delegateRespondsToValidateContact;
	BOOL					shouldUseDisplayName;
	BOOL					shouldDisplayGroupHeaders;
	BOOL					shouldUseUserIcon;
	BOOL					shouldSetTooltip;
	BOOL					shouldIncludeContactListMenuItem;
	BOOL					populateMenuLazily;
}

+ (id)contactMenuWithDelegate:(id<AIContactMenuDelegate>)inDelegate forContactsInObject:(AIListObject *)inContainingObject;
- (void)setContainingObject:(AIListObject *)inContainingObject;

@property (readwrite, nonatomic, unsafe_unretained) id<AIContactMenuDelegate> delegate;

@end

@protocol AIContactMenuDelegate <NSObject>
- (void)contactMenuDidRebuild:(AIContactMenu *)inContactMenu;
@optional
- (void)contactMenu:(AIContactMenu *)inContactMenu didSelectContact:(AIListContact *)inContact; 
- (AIListContact *)contactMenu:(AIContactMenu *)inContactMenu validateContact:(AIListContact *)inContact; 
- (BOOL)contactMenu:(AIContactMenu *)inContactMenu shouldIncludeContact:(AIListContact *)inContact; 
- (BOOL)contactMenuShouldUseUserIcon:(AIContactMenu *)inContactMenu; 
- (BOOL)contactMenuShouldSetTooltip:(AIContactMenu *)inContactMenu; 
- (BOOL)contactMenuShouldIncludeContactListMenuItem:(AIContactMenu *)inContactMenu; 
- (BOOL)contactMenuShouldPopulateMenuLazily:(AIContactMenu *)inContactMenu; 

// Called on each rebuild:
- (BOOL)contactMenuShouldDisplayGroupHeaders:(AIContactMenu *)inContactMenu; //only applies to contained groups
- (BOOL)contactMenuShouldUseDisplayName:(AIContactMenu *)inContactMenu; 
@end
