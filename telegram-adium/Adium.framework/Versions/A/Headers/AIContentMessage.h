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

#define CONTENT_MESSAGE_TYPE		@"Message"		//Type ID for this content

@class AIChat;

/*!	@class	AIContentMessage
 *	@brief	A message in a chat. Subclass of AIContentObject.
 *
 *	@par	A content message is a type of content object that represents a message sent by one user to another user (except in the case of an autoreply, which is sent by the user's client).
 *
 *	@par	The content of the message is handled as an attributed string. This allows AIContentMessages to be used with all services, regardless of the format they use for text (HTML, XHTML, Markdown, etc.).
 *
 *	@par	A content message can be an autoreply. This happens when the message was sent automatically (or will be) in response to the sender receiving a previous message while away. Not all services support autoreplies; on those that don't, the AIContentMessage for an incoming autoreply will not have its \c autoreply flag set.
 *
 *	@par	AIContentMessage is a concrete subclass of AIContentObject. All methods and properties of AIContentObject also work with AIContentMessage.
 */

@interface AIContentMessage : AIContentObject {
	BOOL		isAutoreply;
	NSString	*encodedMessage;
	id			encodedMessageAccountData;
}

/*!	@brief	Create and autorelease an AIContentMessage.
 *	@return	An autoreleased AIContentMessage.
 */
+ (id)messageInChat:(AIChat *)inChat
		 withSource:(id)inSource
		destination:(id)inDest
			   date:(NSDate *)inDate
			message:(NSAttributedString *)inMessage
		  autoreply:(BOOL)inAutoReply;

+ (id)messageInChat:(AIChat *)inChat
		 withSource:(id)inSource
		 sourceNick:(NSString *)inSourceNick
		destination:(id)inDest
			   date:(NSDate *)inDate
			message:(NSAttributedString *)inMessage
		  autoreply:(BOOL)inAutoreply;

/*!	@brief	Create an AIContentMessage.
 *
 *	@par	See <code>AIContentObject</code>'s <code>initWithChat:source:destination:date:message:</code> method for more information.
 *
 *	@return	An AIContentMessage.
 */
- (id)initWithChat:(AIChat *)inChat
			source:(id)inSource
		sourceNick:(NSString *)inSourceNick
	   destination:(id)inDest
			  date:(NSDate *)inDate
		   message:(NSAttributedString *)inMessage
		 autoreply:(BOOL)inAutoreply;

/*!	
 * @brief	Whether this message is an autoreply.
 */
@property (readwrite, nonatomic) BOOL isAutoreply;

/*!	@brief	The encoded string for the contents of this content message.
 *
 *	@par	This is the string containing the same message that the content message was initialized with (which is unchangeable), in a format suitable for use by the service (e.g., to be transmitted over the wire or having been received over the wire).
 *
 *	@par	Usually, the encoded string is obtained from the account by the content controller. In addition, it may be passed through one or more secondary encoders, such as an encrypter. Traditionally, the new string is set as the content message's encoded message using this method after every step.
 *
 *	@return	A string containing the message encoded to some sort of marked-up (or plain) source code, such as HTML source code.
 */
@property (readwrite, nonatomic) NSString *encodedMessage;

/*!	@brief	The object associated with this method for an account's benefit.
 *
 *	@par	This property is intended for use by accounts that need to associate some private data with a message. It is not used by anything else.
 *
 *	@return	The object associated with this message.
 */
@property (readwrite, nonatomic) id encodedMessageAccountData;

/*!
 * @brief The prefix string for the sender of this message.
 *
 * @return A textual representation of the highest level of flags the sender has in a chat.
 *
 * This is returned based on the user's flags in a group chat. For a normal chat, this is an empty string.
 */
@property (weak, readonly, nonatomic) NSString *senderPrefix;

@end
