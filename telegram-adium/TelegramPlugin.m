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
 */

#import "TelegramPlugin.h"
#import "TelegramService.h"
#import "telegram-purple.h"
#import <tgl.h>

extern void purple_init_telegram_plugin();

@implementation TelegramPlugin

- (void) installPlugin
{
    pk_path = [self getPkName];
    purple_init_telegram_plugin();
    [TelegramService registerService];
}

- (void) installLibpurplePlugin
{
}

- (void) loadLibpurplePlugin
{
}

- (void) uninstallPlugin
{
}

- (NSString *)pluginAuthor
{
    return @TG_AUTHOR;
}

-(NSString *)pluginVersion
{
    return @TG_VERSION;
}

-(NSString *)pluginDescription
{
    return @"Telegram";
}

-(char*)getPkName
{
    const char* utf8String = (char *)[[[NSBundle bundleForClass: [self class]] pathForResource: @"tg-server" ofType: @"pub"] UTF8String];
    size_t len = strlen(utf8String) + 1;
    char *buf = malloc(len);
    memcpy(buf, utf8String, len);
    return buf;
}

@end
