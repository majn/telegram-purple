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

@class AIXMLElement;

@interface AIHTMLDecoder : NSObject {
	NSString *XMLNamespace;
	NSString *baseURL;

	struct AIHTMLDecoderOptionsBitField {
		unsigned reserved: 18;

		unsigned generateStrictXHTML: 1;
		
		unsigned allowJavascriptURLs: 1;

		//these next ten members are derived from the arguments to
		//  +encodeHTML:::::::::::: in the old AIHTMLDecoder.
		unsigned headers: 1;
		unsigned fontTags: 1;
		unsigned closingFontTags: 1;
		unsigned colorTags: 1;
		unsigned styleTags: 1;

		unsigned nonASCII: 1;
		unsigned allSpaces: 1;
		unsigned attachmentTextEquivalents: 1;
		unsigned onlyIncludeOutgoingImages: 1;
		unsigned bodyBackground: 1;

		unsigned simpleTagsOnly: 1;
		
		unsigned allowAIMsubprofileLinks: 1;
	} thingsToInclude;
	
	//For the horribly ghetto span and div tags used by the HTML logs
	BOOL send;
	BOOL receive;
	BOOL inDiv;
}

#define AIElementClassAttributeName		@"AIElementClass"

#pragma mark Creation

//+decoder, +new, and -init all return an instance with all flags set to 0.

+ (AIHTMLDecoder *)decoder;

//convenience methods to get a decoder that's already been set up a certain way.

- (id)initWithHeaders:(BOOL)includeHeaders
			 fontTags:(BOOL)includeFontTags
		closeFontTags:(BOOL)closeFontTags
			colorTags:(BOOL)includeColorTags
			styleTags:(BOOL)includeStyleTags
	   encodeNonASCII:(BOOL)encodeNonASCII
		 encodeSpaces:(BOOL)encodeSpaces
	attachmentsAsText:(BOOL)attachmentsAsText
onlyIncludeOutgoingImages:(BOOL)onlyIncludeOutgoingImages
	   simpleTagsOnly:(BOOL)simpleOnly
	   bodyBackground:(BOOL)bodyBackground
  allowJavascriptURLs:(BOOL)allowJS;

+ (AIHTMLDecoder *)decoderWithHeaders:(BOOL)includeHeaders
							 fontTags:(BOOL)includeFontTags
						closeFontTags:(BOOL)closeFontTags
							colorTags:(BOOL)includeColorTags
							styleTags:(BOOL)includeStyleTags
					   encodeNonASCII:(BOOL)encodeNonASCII
						 encodeSpaces:(BOOL)encodeSpaces
					attachmentsAsText:(BOOL)attachmentsAsText
			onlyIncludeOutgoingImages:(BOOL)onlyIncludeOutgoingImages
					   simpleTagsOnly:(BOOL)simpleOnly
					   bodyBackground:(BOOL)bodyBackground
                  allowJavascriptURLs:(BOOL)allowJS;

#pragma mark Work methods

//turn HTML source into an attributed string.
//uses no options.
- (NSAttributedString *)decodeHTML:(NSString *)inMessage;

//turn HTML source into an attributed string, passing the default attributes to use when tags don't explicitly set them
- (NSAttributedString *)decodeHTML:(NSString *)inMessage withDefaultAttributes:(NSDictionary *)inDefaultAttributes;

//turn an attributed string into HTML source.
//uses all options.
- (NSString *)encodeHTML:(NSAttributedString *)inMessage imagesPath:(NSString *)imagesPath;

//Turn an attributed string into the root element of a strict XHTML (1.0) document.
//Uses options: XMLNamespace, includeHeaders, attachmentsAsText.
- (AIXMLElement *)rootStrictXHTMLElementForAttributedString:(NSAttributedString *)inMessage imagesPath:(NSString *)imagesSavePath;

//pass a string containing all the attributes of a tag (for example,
//  @"src=\"window.jp2\" alt=\"Window on the World\""). you will get back a
//  dictionary containing those attributes (for example, @{ @"src" =
//  @"window.jp2", @"alt" = @"Window on the World" }).
//uses no options.
- (NSDictionary *)parseArguments:(NSString *)arguments;

#pragma mark Properties

@property (readwrite, nonatomic, copy) NSString *XMLNamespace;
@property (readwrite, nonatomic) BOOL generatesStrictXHTML;
//meaning <HTML> and </HTML>.
@property (readwrite, nonatomic) BOOL includesHeaders;
@property (readwrite, nonatomic) BOOL includesFontTags;
@property (readwrite, nonatomic) BOOL closesFontTags;
@property (readwrite, nonatomic) BOOL includesColorTags;
@property (readwrite, nonatomic) BOOL includesStyleTags;
//turn non-printable characters into entities.
@property (readwrite, nonatomic) BOOL encodesNonASCII;
@property (readwrite, nonatomic) BOOL preservesAllSpaces;
@property (readwrite, nonatomic) BOOL usesAttachmentTextEquivalents;
@property (readwrite, nonatomic) BOOL onlyConvertImageAttachmentsToIMGTagsWhenSendingAMessage;
@property (readwrite, nonatomic) BOOL onlyUsesSimpleTags;
@property (readwrite, nonatomic) BOOL includesBodyBackground;
@property (readwrite, nonatomic) BOOL allowAIMsubprofileLinks;
@property (readwrite, nonatomic) BOOL allowJavascriptURLs;
@property (readwrite, nonatomic, copy) NSString *baseURL;

@end

@interface AIHTMLDecoder (ClassMethodCompatibility)

/*these bring back the class methods that I (boredzo) turned into instance
 *  methods for the sake of clarity.
 *when these methods are no longer used, this category should be deleted.
 */

+ (AIHTMLDecoder *)classMethodInstance;

+ (NSString *)encodeHTML:(NSAttributedString *)inMessage encodeFullString:(BOOL)encodeFullString;
+ (NSString *)encodeHTML:(NSAttributedString *)inMessage
				 headers:(BOOL)includeHeaders 
				fontTags:(BOOL)includeFontTags
	  includingColorTags:(BOOL)includeColorTags 
		   closeFontTags:(BOOL)closeFontTags
			   styleTags:(BOOL)includeStyleTags
 closeStyleTagsOnFontChange:(BOOL)closeStyleTagsOnFontChange 
		  encodeNonASCII:(BOOL)encodeNonASCII
			encodeSpaces:(BOOL)encodeSpaces
			  imagesPath:(NSString *)imagesPath
	   attachmentsAsText:(BOOL)attachmentsAsText
onlyIncludeOutgoingImages:(BOOL)onlyIncludeOutgoingImages
		  simpleTagsOnly:(BOOL)simpleOnly
		  bodyBackground:(BOOL)bodyBackground
     allowJavascriptURLs:(BOOL)allowJS;

+ (NSAttributedString *)decodeHTML:(NSString *)inMessage;
+ (NSAttributedString *)decodeHTML:(NSString *)inMessage withDefaultAttributes:(NSDictionary *)inDefaultAttributes;

+ (NSDictionary *)parseArguments:(NSString *)arguments;

@end
