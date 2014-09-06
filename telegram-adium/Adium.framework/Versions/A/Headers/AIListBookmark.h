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

#import "AIListContact.h"

#import <Adium/AIChatControllerProtocol.h>

#define KEY_AUTO_JOIN			@"Automatically Join"
#define KEY_UNREAD_STATUS		@"Unread Status"

#define GROUP_LIST_BOOKMARK		@"List Bookmark Settings"

@class AIChat;

@interface AIListBookmark : AIListContact <AIChatObserver, NSCoding> {
	NSDictionary		*chatCreationDictionary;

	NSString			*password;
	NSString			*name;
}

- (id)initWithChat:(AIChat *)inChat;

@property (nonatomic)	NSString *password;
@property (readonly, nonatomic)	NSString *name;
@property (readonly, nonatomic)	NSDictionary *chatCreationDictionary;

- (AIChat *)openChat;

@end
