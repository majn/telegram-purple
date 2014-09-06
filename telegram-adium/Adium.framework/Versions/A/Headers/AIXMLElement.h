/* AIXMLElement.h
 *
 * Created by Peter Hosey on 2006-06-07.
 *
 * This class is explicitly released under the BSD license with the following modification:
 * It may be used without reproduction of its copyright notice within The Adium Project.
 *
 * This class was created for use in the Adium project, which is released under the GPL.
 * The release of this specific class (AIXMLElement) under BSD in no way changes the licensing of any other portion
 * of the Adium project.
 *
 ****
 Copyright © 2006 Peter Hosey, Colin Barrett
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 Neither the name of Peter Hosey nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//FIXME: This class is not CodingStyle compliant.
@interface AIXMLElement : NSObject <NSCopying> {
	NSString *name;
	NSMutableArray *attributeNames;
	NSMutableArray *attributeValues;
	NSMutableArray *contents;
	BOOL selfCloses;
}

+ (id) elementWithNamespaceName:(NSString *)namespace elementName:(NSString *)newName;
- (id) initWithNamespaceName:(NSString *)namespace elementName:(NSString *)newName;
+ (id) elementWithName:(NSString *)newName;
- (id) initWithName:(NSString *)newName;

#pragma mark Accessors

- (NSString *) name;

- (NSUInteger)numberOfAttributes;
- (NSDictionary *)attributes;
- (void) setAttributeNames:(NSArray *)newAttrNames values:(NSArray *)newAttrVals;

- (void)setValue:(NSString *)attrVal forAttribute:(NSString *)attrName;
- (NSString *)valueForAttribute:(NSString *)attrName;

@property (readwrite, nonatomic) BOOL selfCloses;

#pragma mark Contents

//NSString: Unescaped string data (will be escaped when making XML data).
//AIXMLElement: Sub-element (e.g. span in a p).
- (void) addEscapedObject:(id)obj;
- (void) addObject:(id)obj;
- (void) addObjectsFromArray:(NSArray *)array;
- (void) insertEscapedObject:(id)obj atIndex:(NSUInteger)idx;
- (void) insertObject:(id)obj atIndex:(NSUInteger)idx;

- (NSArray *)contents;
- (void)setContents:(NSArray *)newContents;
- (NSString *)contentsAsXMLString;

#pragma mark XML representation

- (NSString *) XMLString;
- (void) appendXMLStringtoString:(NSMutableString *)string;

- (NSData *) UTF8XMLData;
- (void) appendUTF8XMLBytesToData:(NSMutableData *)data;

@end
