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

#import <AIUtilities/AIMultiCellOutlineView.h>
#import <Adium/AIAbstractListController.h>
#import <Adium/AIContactControllerProtocol.h>
#import <Adium/AIListOutlineView+Drawing.h>

@class AIListObject;

typedef enum {
	AINormalBackground = 0,
	AITileBackground,
	AIFillProportionatelyBackground,
	AIFillStretchBackground
} AIBackgroundStyle;

@interface AIListOutlineView : AIMultiCellOutlineView <ContactListOutlineView> {    
	BOOL				groupsHaveBackground;
	BOOL				updateShadowsWhileDrawing;	

	NSImage				*backgroundImage;
	CGFloat				backgroundFade;
	AIBackgroundStyle	backgroundStyle;
	AIContactListWindowStyle windowStyle;
	
	NSColor				*backgroundColor;
	NSColor				*_backgroundColorWithOpacity;
	CGFloat				backgroundOpacity;
	
	NSColor				*highlightColor;

	NSColor				*rowColor;
	NSColor				*_rowColorWithOpacity;
	
	CGFloat				minimumDesiredWidth;
	BOOL	 			desiredHeightPadding;
}

@property (readonly, nonatomic) NSInteger desiredHeight;
@property (readonly, nonatomic) NSInteger desiredWidth;

// Contact menu
@property (weak, readonly, nonatomic) AIListObject *listObject;
@property (weak, readonly, nonatomic) NSArray *arrayOfListObjects;
@property (weak, readonly, nonatomic) NSArray *arrayOfListObjectsWithGroups;
@property (weak, readonly, nonatomic) AIListContact *firstVisibleListContact;

// Contacts

/*!
 * @brief Index of the first visible list contact
 *
 * @result The index, or -1 if no list contact is visible
 */
@property (readonly, nonatomic) int indexOfFirstVisibleListContact;

- (void)setMinimumDesiredWidth:(CGFloat)inMinimumDesiredWidth;
- (void)setDesiredHeightPadding:(int)inPadding;

@end

@interface AIListOutlineView (AIListOutlineView_Drawing)

@property (readwrite, nonatomic, retain) NSColor *backgroundColor;
@property (readwrite, nonatomic, retain) NSColor *highlightColor;
@property (readwrite, nonatomic, retain) NSColor *alternatingRowColor;

// Shadows
- (void)setUpdateShadowsWhileDrawing:(BOOL)update;

// Backgrounds
- (void)setBackgroundImage:(NSImage *)inImage;
- (void)setBackgroundStyle:(AIBackgroundStyle)inBackgroundStyle;
- (void)setBackgroundOpacity:(CGFloat)opacity forWindowStyle:(AIContactListWindowStyle)windowStyle;
- (void)setBackgroundFade:(CGFloat)fade;

@end
