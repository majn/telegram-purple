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

#import <Adium/AIWindowController.h>

@class AIImageTextCellView, AIActionDetailsPane;

@interface CSNewContactAlertWindowController : AIWindowController {
	IBOutlet NSView			*view_auxiliary;
	IBOutlet NSPopUpButton	*popUp_event;
	IBOutlet NSPopUpButton	*popUp_action;

	IBOutlet NSButton		*checkbox_oneTime;

	IBOutlet NSButton		*button_OK;
	IBOutlet NSButton		*button_cancel;
	IBOutlet NSTextField	*label_Event;
	IBOutlet NSTextField	*label_Action;

	AIActionDetailsPane		*detailsPane;
	NSView					*detailsView;
	NSMutableDictionary		*alert;

	id				target;
	NSDictionary	*oldAlert;
	
	AIListObject	*listObject;
	
	BOOL			configureForGlobal;
	
	IBOutlet AIImageTextCellView *headerView;
}

/*!
 * @brief Edit or create a contact alert
 *
 * @param inAlert The alert to edit. Pass nil to create a new alert.
 * @param inObject The list object for which to edit. Pass nil for global.
 * @param parentWindow A window on which to show a modal sheet. Pass nil for a panel prompt.
 * @param inTarget The target to notify when complete, which must implement - (void)alertUpdated:(NSDictionary *)newAlert oldAlert:(NSDictionary *)oldAlert
 *                 newAlert will be 'nil' if the user cancels.
 * @param inConfigureForGlobal YES if the display should be for global events
 * @param inDefaultEventID The event to show initially for this alert
 *
 * @result An autoreleased CSNewContactAlertWindowController, which will already have its window or sheet showing.
 */
+ (CSNewContactAlertWindowController *)editAlert:(NSDictionary *)inAlert
								   forListObject:(AIListObject *)inObject
										onWindow:(NSWindow *)parentWindow
								 notifyingTarget:(id)inTarget
							  configureForGlobal:(BOOL)inConfigureForGlobal
								  defaultEventID:(NSString *)inDefaultEventID __attribute__((objc_method_family(new)));
- (IBAction)cancel:(id)sender;
- (IBAction)save:(id)sender;

@end
