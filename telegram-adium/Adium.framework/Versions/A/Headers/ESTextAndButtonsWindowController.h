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
#import <Adium/AIInterfaceControllerProtocol.h>

@interface ESTextAndButtonsWindowController : AIWindowController {
    IBOutlet	NSTextView		*textView_messageHeader;
    IBOutlet	NSScrollView	*scrollView_messageHeader;
    
	IBOutlet	NSTextView		*textView_message;
    IBOutlet	NSScrollView	*scrollView_message;
	IBOutlet	NSButton		*button_default;
	IBOutlet	NSButton		*button_alternate;
	IBOutlet	NSButton		*button_other;
	IBOutlet	NSButton		*checkbox_suppression;

	IBOutlet	NSImageView		*imageView;

	NSString			*title;
	NSString			*defaultButton;
	NSString			*alternateButton;
	NSString			*otherButton;
	NSString			*suppression;
	NSString			*messageHeader;
	NSAttributedString	*message;
	NSImage				*image;
	id					target;
	id					userInfo;
	
	BOOL				userClickedButton; //Did the user click a button to begin closing the window?
	BOOL				allowsCloseWithoutResponse; //Is it okay to close without clicking a button?
}

- (id)initWithTitle:(NSString *)inTitle
	  defaultButton:(NSString *)inDefaultButton
	alternateButton:(NSString *)inAlternateButton
		otherButton:(NSString *)inOtherButton
		suppression:(NSString *)inSuppression
  withMessageHeader:(NSString *)inMessageHeader
		 andMessage:(NSAttributedString *)inMessage
			  image:(NSImage *)inImage
			 target:(id)inTarget
		   userInfo:(id)inUserInfo;

- (id)initWithTitle:(NSString *)inTitle
	  defaultButton:(NSString *)inDefaultButton
	alternateButton:(NSString *)inAlternateButton
		otherButton:(NSString *)inOtherButton
  withMessageHeader:(NSString *)inMessageHeader
		 andMessage:(NSAttributedString *)inMessage
			 target:(id)inTarget
		   userInfo:(id)inUserInfo;

- (void)changeWindowToTitle:(NSString *)inTitle
			  defaultButton:(NSString *)inDefaultButton
			alternateButton:(NSString *)inAlternateButton
				otherButton:(NSString *)inOtherButton
				suppression:(NSString *)inSuppression
		  withMessageHeader:(NSString *)inMessageHeader
				 andMessage:(NSAttributedString *)inMessage
					  image:(NSImage *)inImage
					 target:(id)inTarget
				   userInfo:(id)inUserInfo;

- (IBAction)pressedButton:(id)sender;

- (void)show;
- (void)setAllowsCloseWithoutResponse:(BOOL)inAllowsCloseWithoutResponse;
- (void)setImage:(NSImage *)image;
- (void)setKeyEquivalent:(NSString *)keyEquivalent modifierMask:(unsigned int)mask forButton:(AITextAndButtonsWindowButton)windowButton;

@end

@interface NSObject (ESTextAndButtonsTarget)
//Return YES to let the window close; NO not to let it close
- (BOOL)textAndButtonsWindowDidEnd:(NSWindow *)window returnCode:(AITextAndButtonsReturnCode)returnCode suppression:(BOOL)suppression userInfo:(id)userInfo;
@end

