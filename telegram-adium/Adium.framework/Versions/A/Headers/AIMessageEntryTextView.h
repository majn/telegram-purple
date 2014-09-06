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

#import <AIUtilities/AISendingTextView.h>
#import <Adium/AIInterfaceControllerProtocol.h>
#import <Adium/AIContactObserverManager.h>

@class AIListContact, AIAccount, AIChat, AIButtonWithCursor;

@protocol AIMessageEntryTextViewDelegate <NSTextViewDelegate,NSObject>

@optional
/*!
 * @brief Should the tab key trigger an autocomplete?
 */
- (BOOL)textViewShouldTabComplete:(NSTextView *)inTextView;
- (NSRange)textView:(NSTextView *)inTextView rangeForCompletion:(NSRange)completionRange;
- (void)textViewDidCancel:(NSTextView *)inTextView;
@end

@interface AISimpleTextView : NSView {
	NSAttributedString	*string;
}

@property (readwrite, nonatomic) NSAttributedString *string;

@end

@interface AIMessageEntryTextView : AISendingTextView <AITextEntryView, AIListObjectObserver> {
	AIChat				*chat;

	BOOL                 clearOnEscape;
	BOOL				 historyEnabled;
	BOOL                 pushPopEnabled;
	BOOL				 homeToStartOfLine;
	BOOL				 enableTypingNotifications;

	NSMutableArray		*historyArray;
	int                  currentHistoryLocation;

	NSMutableArray		*pushArray;
	BOOL                 pushIndicatorVisible;
	AIButtonWithCursor	*pushIndicator;

	NSSize               lastPostedSize;
	NSSize               _desiredSizeCached;
	BOOL				 resizing;

	NSView              *associatedView;

	AISimpleTextView	*characterCounter;
	NSString			*characterCounterPrefix;
	NSInteger			maxCharacters;
	NSColor				*savedTextColor;
    
@protected
    // Emoticons
    BOOL hasEmoticonsMenu;
    AIButtonWithCursor *emoticonsMenuButton;
}

@property (readwrite, unsafe_unretained, nonatomic) id<AIMessageEntryTextViewDelegate> delegate;

// Configure
@property (readwrite, nonatomic) BOOL clearOnEscape;
@property (readwrite, nonatomic) BOOL homeToStartOfLine;
@property (readwrite, nonatomic) NSView *associatedView;

// Context
@property (readwrite, retain, nonatomic) AIChat *chat;
@property (readonly, nonatomic) AIListContact *listObject;

// History
@property (readwrite, nonatomic) BOOL historyEnabled;

// Emoticons
@property (assign) BOOL hasEmoticonsMenu;
@property (retain) NSButton *emoticonsMenuButton;

// Adium Text Entry
- (void)setAttributedString:(NSAttributedString *)inAttributedString;
- (void)setString:(NSString *)string;
- (void)setTypingAttributes:(NSDictionary *)attrs;
- (void)pasteAsPlainTextWithTraits:(id)sender;
- (void)pasteAsRichText:(id)sender;
- (NSSize)desiredSize;

// Paging
- (void)scrollPageUp:(id)sender;
- (void)scrollPageDown:(id)sender;

// History
- (void)historyUp;
- (void)historyDown;

// Push and Pop
- (void)setPushPopEnabled:(BOOL)inBool;
- (void)pushContent;
- (void)popContent;
- (void)swapContent;

// Emoticons
- (void)popUpEmoticonsMenu;

@end
