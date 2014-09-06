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

#import "AIWindowController.h"

typedef enum {
	AIPasswordPromptOKReturn = 0,
	AIPasswordPromptCancelReturn
} AIPasswordPromptReturn;

/*!
 * @class AIPasswordPromptController
 * 
 * This abstract superclass simplifies displaying a password prompt and notifying a target when the
 * password is entered.
 */
@interface AIPasswordPromptController : AIWindowController {
	IBOutlet	NSTextField	*textField_password;
	IBOutlet	NSButton	*checkBox_savePassword;
	IBOutlet	NSButton	*button_OK;

	NSString	*password;

	SEL 		selector;
	id			target;

	id			context;
}

/*!
 * @brief Designated initializer for AIPasswordPromptController
 *
 * @param windowNibName Nib name.
 * @param password Iniital password to put in the password field. If nil, the field will be empty.
 * @param inTarget Target to notify when the prompt window is closed
 * @param inSelector Selector on target. Signature is - (void)passwordReturnedForConnect:(NSString *)inPassword returnCode:(AIPasswordPromptReturn)returnCode context:(id)inContext
 * @param inContext Context to pass back when notifying the target.
 *
 * @result An AIPasswordPromptController
 */
- (id)initWithWindowNibName:(NSString *)windowNibName password:(NSString *)password notifyingTarget:(id)inTarget selector:(SEL)inSelector context:(id)inContext;
- (IBAction)cancel:(id)sender;
- (IBAction)okay:(id)sender;
- (IBAction)togglePasswordSaved:(id)sender;

@end

@interface AIPasswordPromptController (PRIVATE_and_Subclasses)
- (NSString *)savedPasswordKey;
- (void)windowDidLoad;
- (void)savePassword:(NSString *)password;
- (void)setTarget:(id)inTarget selector:(SEL)inSelector context:(id)inContext;
@end
