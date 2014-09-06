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

typedef enum {
	ESPresetNameSheetOkayReturn			= 1,
    ESPresetNameSheetCancelReturn		= 0
} ESPresetNameSheetReturnCode;

@interface ESPresetNameSheetController : AIWindowController {
	IBOutlet	NSTextField		*textField_name;
	IBOutlet	NSTextField		*label_name;
	IBOutlet	NSButton		*button_ok;
	IBOutlet	NSButton		*button_cancel;	

	IBOutlet	NSTextView		*textView_explanatoryText;
	IBOutlet	NSScrollView	*scrollView_explanatoryText;

	NSString	*defaultName;
	NSString	*explanatoryText;
	id			target;
	id			userInfo;
}
- (id)initWithDefaultName:(NSString *)inDefaultName explanatoryText:(NSString *)inExplanatoryText notifyingTarget:(id)inTarget userInfo:(id)inUserInfo;
- (IBAction)okay:(id)sender;
- (IBAction)cancel:(id)sender;

@end

@interface NSObject (ESPresetNameSheetControllerTarget)
//Required
- (void)presetNameSheetControllerDidEnd:(ESPresetNameSheetController *)controller 
							 returnCode:(ESPresetNameSheetReturnCode)returnCode
								newName:(NSString *)newName
							   userInfo:(id)userInfo;

//Optional
- (BOOL)presetNameSheetController:(ESPresetNameSheetController *)controller
			  shouldAcceptNewName:(NSString *)newName
						 userInfo:(id)userInfo;

@end
