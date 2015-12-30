/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 *
 * Copyright Matthias Jentsch 2014-2015
 */

#import "TelegramAutocompletionDelegate.h"

@implementation TelegramAutocompletionDelegate

- (NSArray *)tokenField:(NSTokenField *)tokenField
completionsForSubstring:(NSString *)substring
           indexOfToken:(NSInteger)tokenIndex
    indexOfSelectedItem:(NSInteger *)selectedIndex
{
  NSMutableArray *matchingUsers = [NSMutableArray new];
  const char *startsWith = [substring UTF8String];
  if (TLS) {
    int i = 0;
    while (i != -1) {
      char *R = NULL;
      i = tgl_complete_user_list (TLS, i, startsWith, (int) strlen(startsWith), &R);
      if (R) {
        [matchingUsers addObject: [[NSString alloc] initWithUTF8String:R]];
      }
    }
  }
  return matchingUsers;
}

- (id)tokenField:(NSTokenField *)tokenField
representedObjectForEditingString:(NSString *)editingString
{
  return nil;
}

- (NSString *)tokenField:(NSTokenField *)tokenField
displayStringForRepresentedObject:(id)representedObject
{
  return nil;
}

- (void)setTLS:(struct tgl_state *)_TLS
{
  TLS = _TLS;
}

@end