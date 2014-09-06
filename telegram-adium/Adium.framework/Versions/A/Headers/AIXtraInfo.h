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


@interface AIXtraInfo : NSObject {
	NSString		*name;
	NSString		*path;
	NSString		*version;
	NSImage			*icon;
	NSImage			*previewImage;
	NSString		*resourcePath;
	NSString		*type;
	NSString		*readMePath;
	NSBundle		*xtraBundle;
	
	BOOL			enabled;
}

+ (AIXtraInfo *) infoWithURL:(NSURL *)url;
- (id) initWithURL:(NSURL *)url;

- (NSString *)type;
- (NSString *)path;
- (NSString *)name;
- (NSString *)version;
- (NSString *)resourcePath;
- (NSString *)readMePath;
- (NSImage *)icon;
- (NSImage *)previewImage;
- (void)setName:(NSString *)name;
- (NSBundle *)bundle; //returns nil if no bundle is available

- (BOOL)enabled;
- (void)setEnabled:(BOOL)inEnabled;
@end
