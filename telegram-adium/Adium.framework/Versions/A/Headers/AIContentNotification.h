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

#import <Adium/AIContentEvent.h>

#define CONTENT_NOTIFICATION_TYPE		@"Notification"		//Type ID for this content

typedef enum {
	AIDefaultNotificationType = 0
} AINotificationType;

@interface AIContentNotification : AIContentEvent {
	AINotificationType notificationType;
}

/*!	@brief	Create and autorelease an AIContentNotification.
 *	@return	An autoreleased AIContentNotification.
 */
+ (id)notificationInChat:(AIChat *)inChat
			  withSource:(id)inSource
			 destination:(id)inDest
					date:(NSDate *)inDate
		notificationType:(AINotificationType)inNotificationType;

@property (readonly, nonatomic) AINotificationType notificationType;
@property (weak, readonly, nonatomic) NSString *eventType;

@end
