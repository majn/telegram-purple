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


@interface AIIconState : NSObject {
    BOOL			animated;
    BOOL			overlay;

    //Static
    NSImage			*image;

    //Animated
    NSMutableArray	*imageArray;
    CGFloat			delay;
    BOOL 			looping;
    NSInteger				currentFrame;
    NSInteger				numberOfFrames;

    //Animated w/ delayed rendering
    NSArray			*iconRendering_states;
    AIIconState		*iconRendering_baseState;
    AIIconState		*iconRendering_animationState;
}

- (id)initWithImages:(NSArray *)inImages delay:(CGFloat)inDelay looping:(BOOL)inLooping overlay:(BOOL)inOverlay;
- (id)initWithImage:(NSImage *)inImage overlay:(BOOL)inOverlay;
- (id)initByCompositingStates:(NSArray *)inIconStates;
- (BOOL)animated;
- (CGFloat)animationDelay;
- (BOOL)looping;
- (BOOL)overlay;
- (NSArray *)imageArray;
- (NSImage *)image;
- (NSImage *)_compositeStates:(NSArray *)iconStateArray withBaseState:(AIIconState *)baseState animatingState:(AIIconState *)animatingState forFrame:(NSInteger)frame;
- (NSInteger)currentFrame;
- (void)nextFrame;
- (NSInteger)numberOfFrames;

@end

