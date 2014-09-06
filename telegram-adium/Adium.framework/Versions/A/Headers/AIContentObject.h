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


@class AIChat, AIListObject;

/*!	@class	AIContentObject
 *	@brief	A complete unit of content. Abstract class.
 *
 *	@par	“Content” is a vague word, but this is an abstract class, so vagueness is to be expected. A content object is anything that travels from a source to a destination, relevant to a chat, that may or may not be outgoing, carrying a datestamp and a message.
 *
 *	@par	The most common kind of content object is a content message, which is what people use to communicate over chats (except on MSN, where some people use their status messages).
 *
 *	@par	A content object may be incoming or outgoing. AIContentObject's basic rule for determining the direction of the content is to examine the content's source: If the source is an account, then the content is outgoing; if the source is a contact, then the content is incoming. The direction of a content object determines which events are triggered by the content (if that isn't disabled for the content object), and can affect how the content is displayed in the message view.
 *
 *	@par	Not all content objects have a message. In particular, a typing notification (\c AIContentTyping) does not have a message.
 *
 *	@par	In addition, AIContentObject includes a number of methods and properties that exist for the convenience of other parts of Adium. These include:
 *	@li	<code>isSimilarToContent:</code> (used by message view to determine whether styles may want to group two content objects together)
 *	@li	<code>isFromSameDayAsContent:</code> (used by message view to determine whether to insert a date separator)
 *	@li	<code>displayClasses</code> (used by message view)
 */

@interface AIContentObject : NSObject {
    AIChat				*__weak chat;
    AIListObject		*source;
    AIListObject		*destination;
	NSString			*sourceNick;
    BOOL				outgoing;
    
	NSAttributedString	*message;
    NSDate  			*date;
	
	BOOL				filterContent;
	BOOL				trackContent;
	BOOL				displayContent;
	BOOL				displayContentImmediately;
	BOOL				sendContent;
	BOOL				postProcessContent;
	
	NSDictionary		*userInfo;
	
	NSMutableArray		*customDisplayClasses;
}

/*!	@brief	Create a content object without a message.
 *
 *	@par	If your subclass represents a type of content that wouldn't have a message associated with it (such as a typing notification), then this is the message you should send to \c super.
 *
 *	@param	inChat	The chat with which this content is associated. The content may be presented in the message view and/or effect a change in the status icon for the tab (as well as the contact's row in the contact list).
 *	@param	inSource	The account or contact from which the content originated. If it's an account, then this is outgoing content.
 *	@param	inDest	The contact to which the content is addressed.
 *	@param	inDate	The date and time at which the content was received or sent.
 *	@return	A shiny new content object.
 */
- (id)initWithChat:(AIChat *)inChat
			source:(AIListObject *)inSource
	   destination:(AIListObject *)inDest
	          date:(NSDate*)inDate;
/*!	@brief	Create a content object with a message.
 *
 *	@par	Most content objects have a styled-text or plain-text message as their content. If your subclass represents a type of content that may have a message associated with it (such as a chat message), then this is the message you should send to \c super.
 *
 *	@param	inChat	The chat with which this content is associated. The content may be presented in the message view and/or effect a change in the status icon for the tab (as well as the contact's row in the contact list).
 *	@param	inSource	The account or contact from which the content originated. If it's an account, then this is outgoing content.
 *	@param	inDest	The contact to which the content is addressed.
 *	@param	inDate	The date and time at which the content was received or sent.
 *	@param	inMessage	The message that is this object's content. May be \c nil; in that case, this is the same as the init method that doesn't have a message: parameter.
 *	@return	A shiny new content object.
 */
- (id)initWithChat:(AIChat *)inChat
			source:(AIListObject *)inSource
	   destination:(AIListObject *)inDest
			  date:(NSDate*)inDate
		   message:(NSAttributedString *)inMessage;

- (id)initWithChat:(AIChat *)inChat
			source:(AIListObject *)inSource
		sourceNick:(NSString *)inSourceNick
	   destination:(AIListObject *)inDest
			  date:(NSDate*)inDate
		   message:(NSAttributedString *)inMessage;

/*!	@brief	The type of content.
 *
 *	@par	There is at least one type defined for every concrete subclass of \c AIContentObject.
 *
 *	@par	Subclassing note: You must override the getter and return a string (not \c nil). This string should probably be constant, so that types can be compared using the <code>==</code> operator rather than string comparison.
 *
 *	@return	A string representing the type of content that this object bears.
 */
@property (weak, nonatomic, readonly) NSString *type;

#pragma mark Comparing

/*!	@brief	Compares the proximity of two content objects.
 *
 *	@par	The content objects are similar if they are of the same type, are from the same source, and have datestamps within five minutes of each other.
 *
 *	@param	inContent	The content object to compare the receiver to.
 *	@return	\c YES if the receiver is similar to \a inContent; \c NO if not.
 */
- (BOOL)isSimilarToContent:(AIContentObject *)inContent;
/*!	@brief	Reports whether two content objects were delivered on the same day.
 *
 *	@par	The content objects are on the same day if their datestamps have the same year, month, and date. The comparison is performed within the Common Era, i.e., the Gregorian calendar.
 *
 *	@param	inContent	The content object to compare the receiver to.
 *	@return	\c YES if the receiver is from the same day of the common era as \a inContent; \c NO if not.
 */
- (BOOL)isFromSameDayAsContent:(AIContentObject *)inContent;

#pragma mark Content

/*!	@brief	The list object (usually, account or contact) that originated the content.
 *
 *	@par	If the source is an account, then the content is outgoing (the user sent it). Otherwise, it's incoming (the user received it).
 *
 *	@par	Content objects' source should not be not be confused with their chat. See \c chat if that's what you're looking for.
 *
 *	@par	This is set by the init methods.
 *
 *	@return	The source of the content.
 */
@property (nonatomic, readonly) AIListObject *source;
/*!	@brief	The list object (usually, contact) that the content is addressed to.
 *
 *	@par	This is set by the init methods.
 *
 *	@return	The intended recipient of the content.
 */
@property (nonatomic, readonly) AIListObject *destination;

/*!	@brief	The date and time at which the content was sent.
 *
 *	@par	This is set by the init methods.
 *
 *	@return	The datestamp of the content.
 */
@property (nonatomic, readonly) NSDate *date;

/*!	@brief	Returns whether the content is outgoing (the user sent it), rather than incoming (the user received it).
 *
 *	@par	If the content's source is an account, then the content is outgoing (the user sent it). Otherwise, it's incoming (the user received it).
 *
 *	@par	Whether a content object is outgoing determines which events are triggered by the content (if that isn't disabled for the content object), and can affect how the content is displayed in the message view.
 *
 *	@return	\c YES if the content is outgoing; \c NO if it is incoming.
 */
@property (nonatomic, readonly) BOOL isOutgoing;

/*!	@brief	The chat with which this content is associated.
 *
 *	@par	All content is associated with a chat, even if only incidentally. While the content may not be chat-oriented, all content may eventually end up displayed in a chat somewhere, either inline in the message view (status changes and file transfers) or in the tab or title bar (typing notification).
 *
 *	@return	The chat with which this content is associated.
 */
@property (nonatomic, readwrite, weak) AIChat *chat;

/*!	@brief	Obtain the current message in the content.
 *
 *	@return	The current message.
 */
@property (nonatomic, strong) NSAttributedString *message;
@property (weak, readonly, nonatomic) NSString *messageString;
/*!	@brief	Get an array of CSS class names with which this content should be displayed.
 *
 *	@par	You should use these classes whenever inserting the content into an HTML or XHTML document, or anywhere else where CSS would be used with the content.
 *
 *	@return	An array of strings, each containing a class name.
 */
- (NSMutableArray *)displayClasses;

/*! @brief	Add a custom display class to this object
 *
 *  @par	This can be used to tag messages with custom metadata which display engines can use to style them accordingly
 */
- (void) addDisplayClass:(NSString *)className;

/*!	@brief	Associate any object you want with this content object.
 *
 *	@par	\c AIContentObject does not use this object at all; it simply retains it. This property is entirely for your own use. This sort of property is also called a “reference constant”, especially by Carbon APIs.
 *
 *	@par	Beware of setting a mutable object as the user-info object, because the \c AIContentObject will not make a copy; it will only retain it. There's nothing wrong with putting a mutable object here; just make sure that you know you're doing that, that you document that you're willfully setting a mutable object here, and that nothing mutates the object contrarily to your expectations.
 *
 *	@par	It is safe to set this property to \c nil.
 *
 *	@param	inUserInfo	The new user-info object.
 */
@property (nonatomic, strong) id userInfo;

#pragma mark Behavior

/*!	@brief	Return whether the content passes through content filters.
 *
 *	@par	Content filtering is performed by the content controller; see \c AIContentControllerProtocol. This property tells the content controller whether to apply its filters to the content object, or leave the content object unfiltered.
 *
 *	@return	\c YES if the content will be passed through content filters; \c NO if it will not be passed through filters.
 */
@property (nonatomic) BOOL filterContent;

/*!	@brief	Return whether the content will cause notifications and events.
 *
 *	@par	If this is \c YES, the content controller will post notifications and generate events so that other objects can monitor the progress of receipt and sending of content.
 *
 *	@par	See the \c AIContentControllerProtocol documentation for more information on content controller events.
 *
 *	@par	XXX We should list reasons why you would want or not want a content object to be tracked.
 *
 *	@return	\c YES if the content controller should generate events and post notifications about this content object; \c NO if it should keep mum.
 */
@property (nonatomic) BOOL trackContent;


/*!	@brief	Return whether the content will be displayed to the user.
 *
 *	@par	Once a content object has completely passed through the content controller's receiving or sending procedure, including all filtering, the content controller then looks to this property to decide whether it should display the content. Some types of content, such as typing notification (\c AIContentTyping), are sent, but not displayed.
 *
 *	@par	This property also controls whether the content will be entered into its chat's transcript (if the logger knows how to transcribe this content). If it is not displayed, neither will it be logged. See also the \c postProcessContent property, which controls postprocessing (such as logging) independently of whether the content will be displayed.
 *
 *	@return	\c YES if the content controller will display this content; \c NO otherwise.
 */
@property (nonatomic) BOOL displayContent;

/*!	@brief	Return whether the content will be displayed immediately, or enqueued.
 *
 *	@par	The message view keeps a queue of messages which it eventually displays all at once rather than one at a time. When this is \c NO, the content is held in this queue until a suitable moment to display all the queued messages. Such a moment generally does arrive very soon; it is unlikely that the content will be held even for a whole second.
 *
 *	@par	The queue is for the benefit of the message history: it would be distracting if the message view were to scroll as one message after another were introduced into it, so the messages are inserted with <code>displayContentImmediately = YES</code>, so that they bypass the queue and instead all together instantly.
 *
 *	@return	\c YES if the content object should not be held in the queue; \c NO if it should.
 */
@property (nonatomic) BOOL displayContentImmediately;

/*!	@brief	Return whether to allow sending the content over the network.
 *
 *	@par	The content controller will not send ask the account to send a content object's content if the content object's \c sendContent property is \c NO. The content will still be displayed locally.
 *
 *	@return	\c YES if this content is OK for the content controller to send; \c NO if it should not be sent.
 */
@property (nonatomic) BOOL sendContent;


/*!	@brief	Return whether any post-delivery treatment should be applied to the message.
 *
 *	@par	Logging (aka transcription) is one example of something that is done to messages and other content objects after they have finished being received or sent.
 *
 *	@return	\c YES if the content should be postprocessed; \c NO if it should not.
 */
@property (nonatomic) BOOL postProcessContent;

@property (nonatomic, retain) NSString *sourceNick;

@end
