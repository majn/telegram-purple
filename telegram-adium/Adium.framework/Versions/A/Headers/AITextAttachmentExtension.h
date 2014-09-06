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

@interface AITextAttachmentExtension : NSTextAttachment <NSCopying> {
	NSString	*stringRepresentation;
	BOOL	shouldSaveImageForLogging;
	BOOL	hasAlternate;
	NSString	*path;
	NSImage		*image;
	NSString	*imageClass; //set as class attribute in html, used to tell images apart for CSS
	BOOL		shouldAlwaysSendAsText;
}

+ (AITextAttachmentExtension *)textAttachmentExtensionFromTextAttachment:(NSTextAttachment *)textAttachment;

@property (readwrite, nonatomic, copy) NSString *string;
@property (readwrite, nonatomic, copy) NSString *imageClass;
@property (readwrite, nonatomic) BOOL shouldSaveImageForLogging;
@property (readwrite, nonatomic) BOOL hasAlternate;
@property (readwrite, nonatomic, copy) NSString *path;
@property (readwrite, nonatomic) NSImage *image;
@property (unsafe_unretained, readonly, nonatomic) NSImage *iconImage;
@property (readonly, nonatomic) BOOL attachesAnImage;
@property (readwrite, nonatomic) BOOL shouldAlwaysSendAsText;
@end
