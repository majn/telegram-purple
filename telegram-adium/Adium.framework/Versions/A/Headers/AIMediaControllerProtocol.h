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

typedef enum {
	AIMediaTypeAudio,
	AIMediaTypeVideo
} AIMediaType;

typedef enum {
	AIMediaStateWaiting = 1, 	/* Waiting for response */
	AIMediaStateRequested,		/* Got request */
	AIMediaStateAccepted,		/* Accepted call */
	AIMediaStateRejected,		/* Rejected call */
} AIMediaState;

@class AIMedia, AIListContact, AIAccount;

@protocol AIMediaWindowController
@property (readwrite, retain, nonatomic) AIMedia *media;
@property (readwrite, retain, nonatomic) NSView *outgoingVideo;
@property (readwrite, retain, nonatomic) NSView *incomingVideo;
@end

@protocol AIMediaController <AIController>
- (AIMedia *)mediaWithContact:(AIListContact *)contact
					onAccount:(AIAccount *)account;

- (AIMedia *)existingMediaWithContact:(AIListContact *)contact
							onAccount:(AIAccount *)account;

- (NSWindowController <AIMediaWindowController> *)windowControllerForMedia:(AIMedia *)media;
- (void)closeMediaWindowController:(NSWindowController <AIMediaWindowController> *)mediaWindowController;

- (void)media:(AIMedia *)media didSetState:(AIMediaState)state;

@end
