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


#import <AIUtilities/AIImageCollectionView.h>


@class AIImageCollectionView, AIImageCollectionViewDelegate, AIMessageEntryTextView;

/*!
 * @class AIMessageViewEmoticonsController
 * @brief Handles message view emoticons menu
 *
 * Opens a contextual (pop-up) menu, allowing to insert emoticons.
 */
@interface AIMessageViewEmoticonsController : NSObject <AIImageCollectionViewDelegate, NSMenuDelegate> {
	AIMessageEntryTextView *textView;
	NSArray *emoticons;
	
@private
	NSArray *emoticonTitles;
	NSArray *emoticonSymbols;
}

@property (weak) IBOutlet NSMenu *menu;
@property (weak) IBOutlet AIImageCollectionView *emoticonsCollectionView;
@property (weak) IBOutlet NSTextField *emoticonTitleLabel;
@property (weak) IBOutlet NSTextField *emoticonSymbolLabel;
@property (weak) IBOutlet NSView *alignmentView;

@property (strong) AIMessageEntryTextView *textView;

@property (copy) NSArray *emoticons;
@property (copy) NSArray *emoticonTitles;
@property (copy) NSArray *emoticonSymbols;

/*!
 * @brief Open the menu
 *
 * @param textView	AIMessageEntryTextView
 */
+ (void)popUpMenuForTextView:(AIMessageEntryTextView *)textView atPoint:(NSPoint)aPoint;

@end
