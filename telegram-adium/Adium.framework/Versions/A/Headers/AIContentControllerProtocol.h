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

#import <Adium/AIControllerProtocol.h>

#define Content_ContentObjectAdded					@"Content_ContentObjectAdded"
#define Content_ChatDidFinishAddingUntrackedContent	@"Content_ChatDidFinishAddingUntrackedContent"
#define Content_WillSendContent						@"Content_WillSendContent"
#define Content_WillReceiveContent					@"Content_WillReceiveContent"

//XXX - This is really UI, but it can live here for now
#define PREF_GROUP_FORMATTING				@"Formatting"
#define KEY_FORMATTING_FONT					@"Default Font"
#define KEY_FORMATTING_TEXT_COLOR			@"Default Text Color"
#define KEY_FORMATTING_BACKGROUND_COLOR		@"Default Background Color"

// Used to hide from events' descriptions parts of messages
#define AIHiddenMessagePartAttributeName	@"AIHiddenMessagePart"

//Not displayed, but used for internal identification of the encryption menu
#define ENCRYPTION_MENU_TITLE						@"Encryption Menu"

typedef enum {
	AIFilterContent = 0,		// Changes actual message and non-message content
	AIFilterDisplay,			// Changes only how non-message content is displayed locally (Profiles, aways, auto-replies, ...)
	AIFilterMessageDisplay,  	// Changes only how messages are displayed locally
	AIFilterTooltips,			// Changes only information displayed in contact tooltips
	AIFilterContactList,		// Changes only information in statuses displayed in the contact list
	/* A special content mode for AIM auto-replies that will only apply to bounced away messages.  This allows us to
	 * filter %n,%t,... just like the official client.  A small tumor in our otherwise beautiful filter system *cry*
	 */
	AIFilterAutoReplyContent
	
} AIFilterType;
#define FILTER_TYPE_COUNT 6

typedef enum {
	AIFilterIncoming = 0,   // Content we are receiving
	AIFilterOutgoing		// Content we are sending
} AIFilterDirection;
#define FILTER_DIRECTION_COUNT 2

#define HIGHEST_FILTER_PRIORITY 0
#define HIGH_FILTER_PRIORITY 0.25f
#define DEFAULT_FILTER_PRIORITY 0.5f
#define LOW_FILTER_PRIORITY 0.75f
#define LOWEST_FILTER_PRIORITY 1.0f

@protocol AIContentFilter, AIDelayedContentFilter, AIHTMLContentFilter;
@protocol AdiumMessageEncryptor;

@class AIAccount, AIChat, AIListContact, AIListObject, AIContentObject;
@class AIContentMessage;

@protocol AIContentController <AIController>
//Typing
- (void)userIsTypingContentForChat:(AIChat *)chat hasEnteredText:(BOOL)hasEnteredText;

	//Formatting
- (NSDictionary *)defaultFormattingAttributes;

	//Content Filtering
- (void)registerContentFilter:(id <AIContentFilter>)inFilter
					   ofType:(AIFilterType)type
					direction:(AIFilterDirection)direction;
- (void)registerDelayedContentFilter:(id <AIDelayedContentFilter>)inFilter
							  ofType:(AIFilterType)type
						   direction:(AIFilterDirection)direction;
- (void)registerHTMLContentFilter:(id <AIHTMLContentFilter>)inFilter
						direction:(AIFilterDirection)direction;
- (void)unregisterContentFilter:(id <AIContentFilter>)inFilter;
- (void)unregisterDelayedContentFilter:(id <AIDelayedContentFilter>)inFilter;
- (void)unregisterHTMLContentFilter:(id <AIHTMLContentFilter>)inFilter;
- (void)registerFilterStringWhichRequiresPolling:(NSString *)inPollString;
- (BOOL)shouldPollToUpdateString:(NSString *)inString;

- (NSAttributedString *)filterAttributedString:(NSAttributedString *)attributedString
							   usingFilterType:(AIFilterType)type
									 direction:(AIFilterDirection)direction
									   context:(id)context;
- (void)filterAttributedString:(NSAttributedString *)attributedString
			   usingFilterType:(AIFilterType)type
					 direction:(AIFilterDirection)direction
				 filterContext:(id)filterContext
			   notifyingTarget:(id)target
					  selector:(SEL)selector
					   context:(id)context;
- (void)delayedFilterDidFinish:(NSAttributedString *)attributedString uniqueID:(unsigned long long)uniqueID;
- (NSString *)filterHTMLString:(NSString *)htmlString
					 direction:(AIFilterDirection)direction
					   content:(AIContentObject*)content;

	//Sending / Receiving content
- (void)receiveContentObject:(AIContentObject *)inObject;
- (BOOL)sendContentObject:(AIContentObject *)inObject;
- (void)sendRawMessage:(NSString *)inString toContact:(AIListContact *)inContact;
/*!
 * @brief Display content, optionally using content filters
 *
 * This should only be used for content which is not being sent or received but only displayed, such as message history. If you
 *
 * The ability to force filtering to be completed immediately exists for message history, which needs to put its display
 * in before the first message; otherwise, the use of delayed filtering would mean that message history showed up after the first message.
 * 
 * @param inObject The object to display
 * @param useContentFilters Should filters be used?
 * @param immediately If YES, only immediate filters will be used, and inObject will have its message set before we return.
 *					  If NO, immediate and delayed filters will be used, and inObject will be filtered over the course of some number of future run loops.
 */
- (void)displayContentObject:(AIContentObject *)inObject usingContentFilters:(BOOL)useContentFilters immediately:(BOOL)immediately;
- (void)displayEvent:(NSString *)message ofType:(NSString *)type inChat:(AIChat *)inChat;

	//Encryption
- (NSAttributedString *)decodedIncomingMessage:(NSString *)inString
								   fromContact:(AIListContact *)inListContact
									 onAccount:(AIAccount *)inAccount
									tryDecrypt:(BOOL)decrypt;
- (NSString *)decryptedIncomingMessage:(NSString *)inString
						   fromContact:(AIListContact *)inListContact
							 onAccount:(AIAccount *)inAccount;

- (NSMenu *)encryptionMenuNotifyingTarget:(id)target withDefault:(BOOL)withDefault;

- (BOOL)chatIsReceivingContent:(AIChat *)chat;

	//OTR
- (void)setEncryptor:(id<AdiumMessageEncryptor>)inEncryptor;
- (void)requestSecureOTRMessaging:(BOOL)inSecureMessaging inChat:(AIChat *)inChat;
- (void)promptToVerifyEncryptionIdentityInChat:(AIChat *)inChat;
- (void)questionVerifyEncryptionIdentityInChat:(AIChat *)inChat;
- (void)sharedVerifyEncryptionIdentityInChat:(AIChat *)inChat;
@end


/*!
 * @protocol AIContentFilter
 * @brief Protocol to be implemented by regular content filter objects
 *
 * See registerContentFilter:ofType:direction: for registration
 */
@protocol AIContentFilter
/*!
 * @brief Filter an attributed string
 *
 * Given an attributed string, the filtering object makes any desired changes and returns the resulting string.
 * If no changes are made, the original attributed string should be returned.
 * If after handling the attributed string it should be discarded rather than used (e.g. processing a message's
 *		attributed string led to an action, and the message itself should no longer be sent), return nil.
 *
 * @param inAttributedString NSAttributedString to filter
 * @param context An object, such as an AIListContact or an AIAccount, potentially relevant to filtration. May be anything, so check its class as needed.
 * @result The filtered attributed string, which may be the same as attributedString, or nil if the attributed string should be discared.
 */
- (NSAttributedString *)filterAttributedString:(NSAttributedString *)inAttributedString context:(id)context;

/*!
 * @brief The priority for this filter
 *
 * This is a float between 0.0 and 1.0.  Lower numbers indicate a higher priority.
 * Filter priority determines the order in which filters are executed on a given string.
 * DEFAULT_FILTER_PRIORITY should be used unless you have a reason to care that your filter happen earlier or later.
 *
 * @result A float between 0.0 and 1.0, with (HIGHEST_FILTER_PRIORITY == 0.0) and (LOWEST_FILTER_PRIORITY == 1.0)
 */
- (CGFloat)filterPriority;
@end

/*!
 * @protocol AIHTMLContentFilter
 * @brief Protocol to be implemented by HTML content filter objects
 *
 * See registerHTMLContentFilter:direction: for registration
 * AIHTMLContentFilters only see strings just as the WKMV is about to display them. This allows outputting custom html for display
 */
@protocol AIHTMLContentFilter
/*!
 * @brief Filter an HTML string
 *
 * Given an HTML string, the filtering object makes any desired changes and returns the resulting string.
 * If no changes are made, the original HTML string should be returned.
 * This filtration occurs immediately before the HTML is displayed to the user in the WebKit Message View
 *
 * @param inHTMLString HTML to filter
 * @param content The AIContentObject associated with this HTML, allowing access to the source and destination for the message
 * @result The filtered HTML string, which may be the same as inHTMLString
 */
- (NSString *)filterHTMLString:(NSString *)inHTMLString content:(AIContentObject*)content;

/*!
 * @brief The priority for this filter
 *
 * This is a float between 0.0 and 1.0.  Lower numbers indicate a higher priority.
 * Filter priority determines the order in which filters are executed on a given string.
 * DEFAULT_FILTER_PRIORITY should be used unless you have a reason to care that your filter happen earlier or later.
 *
 * @result A float between 0.0 and 1.0, with (HIGHEST_FILTER_PRIORITY == 0.0) and (LOWEST_FILTER_PRIORITY == 1.0)
 */
- (CGFloat)filterPriority;
@end

/*!
 * @protocol AIDelayedContentFilter
 * @brief Protocol to be implemented by delayed content filter objects
 *
 * A delayed content filter is able to begin working on filtering an attributed string and then
 * return its results an unspecified amount of time later.  This allows nonblocking filtration.
 *
 * This should only be used if the expected time for execution is nontrivial. Use AIContentFilter if
 * the filtration occurs quickly.
 *
 * See registerDelayedContentFilter:ofType:direction: for registration
 */
@protocol AIDelayedContentFilter
/*!
 * @brief Filter an attributed string over an unspecified period of time
 * 
 * Given an attributed string, the filtering object may begin working on the result of its filter
 *
 * [adium.contentController delayedFilterDidFinish:uniqueID:] should be called with the eventual result if this method returns YES.
 * If the filter eventually fails, this method MUST be called with the original inAttributedString.
 *
 * @param inAttributedString NSAttributedString to filter
 * @param context An object, such as an AIListContact or an AIAccount, potentially relevant to filtration. May be anything, so check its class as needed.
 * @param uniqueID A uniqueID which will be passed back to [adium.contentController delayedFilterDidFinish:uniqueID:] when this filtration is complete.
 *
 * @result YES if a delayed filtration process began; NO if no changes are to be made.
 */
- (BOOL)delayedFilterAttributedString:(NSAttributedString *)inAttributedString context:(id)context uniqueID:(unsigned long long)uniqueID;

/*!
 * @brief The priority for this filter
 *
 * This is a float between 0.0 and 1.0.  Lower numbers indicate a higher priority.
 * Filter priority determines the order in which filters are executed on a given string.
 * DEFAULT_FILTER_PRIORITY should be used unless you have a reason to care that your filter happen earlier or later.
 *
 * @result A float between 0.0 and 1.0, with (HIGHEST_FILTER_PRIORITY == 0.0) and (LOWEST_FILTER_PRIORITY == 1.0)
 */
- (CGFloat)filterPriority;
@end

@protocol AdiumMessageEncryptor <NSObject>
- (void)willSendContentMessage:(AIContentMessage *)inContentMessage;
- (NSString *)decryptIncomingMessage:(NSString *)inString fromContact:(AIListContact *)inListContact onAccount:(AIAccount *)inAccount;

- (void)requestSecureOTRMessaging:(BOOL)inSecureMessaging inChat:(AIChat *)inChat;
- (void)promptToVerifyEncryptionIdentityInChat:(AIChat *)inChat;
- (void)questionVerifyEncryptionIdentityInChat:(AIChat *)inChat;
- (void)sharedVerifyEncryptionIdentityInChat:(AIChat *)inChat;
@end
