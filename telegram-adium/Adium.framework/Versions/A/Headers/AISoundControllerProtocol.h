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

#define PREF_GROUP_SOUNDS					@"Sounds"
#define KEY_SOUND_CUSTOM_VOLUME_LEVEL		@"Custom Volume Level"

@protocol AISoundController <AIController>
//Sound
- (void)playSoundAtPath:(NSString *)inPath;
- (void)stopPlayingSoundAtPath:(NSString *)inPath;

//Speech
- (void)speakDemoTextForVoice:(NSString *)voiceString withPitch:(float)pitch andRate:(float)rate;
@property (nonatomic, readonly) float defaultRate;
@property (nonatomic, readonly) float defaultPitch;
- (void)speakText:(NSString *)text;
- (void)speakText:(NSString *)text withVoice:(NSString *)voiceString pitch:(float)pitch rate:(float)rate;

//Soundsets
@property (nonatomic, readonly) NSArray *soundSets;

- (void)setSoundsAreMuted:(BOOL)muted;
@end
