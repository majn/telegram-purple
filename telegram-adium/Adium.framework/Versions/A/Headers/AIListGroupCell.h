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

#import <Adium/AIListCell.h>

@class AIGradient;

@interface AIListGroupCell : AIListCell {
	NSColor		*shadowColor;
	NSColor		*backgroundColor;
	NSColor		*gradientColor;
	BOOL		drawsBackground;
	BOOL		drawsGradientEdges;
	NSLayoutManager	*layoutManager;
	
	NSImage		*_gradient;
	NSSize		_gradientSize;
}

- (CGFloat)flippyIndent;
- (void)setShadowColor:(NSColor *)inColor;
- (NSColor *)shadowColor;
- (NSRect)drawGroupCountWithFrame:(NSRect)inRect;
- (void)setBackgroundColor:(NSColor *)inBackgroundColor gradientColor:(NSColor *)inGradientColor;
- (void)setDrawsBackground:(BOOL)inValue;
- (void)setDrawsGradientEdges:(BOOL)inValue;
- (NSImage *)cachedGradient:(NSSize)inSize;
- (void)drawBackgroundGradientInRect:(NSRect)inRect;
- (NSGradient *)backgroundGradient;
- (void)flushGradientCache;
- (NSColor *)flippyColor;
	
@end
