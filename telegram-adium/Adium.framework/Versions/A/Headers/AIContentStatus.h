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

#import <Adium/AIContentObject.h>

#define CONTENT_STATUS_TYPE		@"Status"		//Type ID for this content

@interface AIContentStatus : AIContentObject {
	NSString			*statusType;
	NSAttributedString	*loggedMessage;
	NSString			*coalescingKey;
}

+ (id)statusInChat:(AIChat *)inChat
		withSource:(id)inSource
	   destination:(id)inDest
			  date:(NSDate *)inDate 
		   message:(NSAttributedString *)inMessage
		  withType:(NSString *)inType;

- (id)initWithChat:(AIChat *)inChat
			source:(id)inSource
	   destination:(id)inDest
			  date:(NSDate *)inDate
		   message:(NSAttributedString *)inMessage
		  withType:(NSString *)inStatus;

/*!
 * @brief A key on which multiple status messages should be consolidated if possible
 *
 * The message view can choose to collapse multiple consecutive status messages into a single
 * or combined display if those messages have the same coalescing key.
 */
@property (readwrite, nonatomic, copy) NSString *coalescingKey;
@property (readonly, nonatomic) NSString *status;
@property (readwrite, nonatomic) NSAttributedString *loggedMessage;

@end
