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
#import <Adium/AIStatus.h>

@class AIAccount, AITextViewWithPlaceholder, AIService, AIAutoScrollView, AISendingTextView;

@interface AIEditStateWindowController : AIWindowController <NSTextViewDelegate> {
	IBOutlet	NSBox				*box_title;
	IBOutlet	NSTextField			*label_title;
	IBOutlet	NSTextField			*textField_title;
	
	IBOutlet	NSBox				*box_separatorLine;
	
	IBOutlet	NSBox				*box_state;
	IBOutlet	NSTextField			*label_state;
	IBOutlet	NSPopUpButton		*popUp_state;
	BOOL		needToRebuildPopUpState;
	
	IBOutlet	NSTextField				*label_statusMessage;
	IBOutlet	NSBox					*box_statusMessage;
	IBOutlet	AISendingTextView		*textView_statusMessage;
	IBOutlet	AIAutoScrollView		*scrollView_statusMessage;

	IBOutlet	NSButton				*checkbox_autoReply;
	IBOutlet	NSButton				*checkbox_customAutoReply;
	IBOutlet	AISendingTextView		*textView_autoReply;
	IBOutlet	AIAutoScrollView		*scrollView_autoReply;

	IBOutlet	NSButton			*checkbox_idle;
	IBOutlet	NSBox				*box_idle;
	IBOutlet	NSTextField			*textField_idleMinutes;
	IBOutlet	NSStepper			*stepper_idleMinutes;
	IBOutlet	NSTextField			*textField_idleHours;
	IBOutlet	NSStepper			*stepper_idleHours;

	IBOutlet	NSButton			*checkBox_save;
	IBOutlet	NSButton			*checkBox_muteSounds;
	IBOutlet	NSButton			*checkBox_silenceGrowl;
	IBOutlet	NSButton			*checkBox_okay;
	IBOutlet	NSButton			*checkBox_cancel;

	AIStatus	*originalStatusState;
	AIStatus	*workingStatusState;
	AIAccount	*account;

	id			target;
	
	BOOL		showSaveCheckbox;
}

+ (id)editCustomState:(AIStatus *)inStatusState forType:(AIStatusType)inStatusType andAccount:(AIAccount *)inAccount withSaveOption:(BOOL)allowSave onWindow:(id)parentWindow notifyingTarget:(id)inTarget;

- (IBAction)cancel:(id)sender;
- (IBAction)okay:(id)sender;
- (IBAction)statusControlChanged:(id)sender;
- (void)updateControlVisibilityAndResizeWindow;

- (void)configureForState:(AIStatus *)state;
- (AIStatus *)currentConfiguration;

@end

@interface NSObject (AICustomStatusWindowTarget)
- (void)customStatusState:(AIStatus *)originalState changedTo:(AIStatus *)newState forAccount:(AIAccount *)account;
@end
