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

#import "AdiumTelegramAccount.h"
#import <libpurple/conversation.h>
#import <Adium/ESFileTransfer.h>
#import <Adium/AIListContact.h>
#import "tgp-ft.h"

@implementation AdiumTelegramAccount

- (const char*)protocolPlugin
{
	return "prpl-telegram";
}

- (NSString *)host
{
	return @"149.154.167.50";
}

- (BOOL)shouldDisplayOutgoingMUCMessages
{
  return NO;
}

- (int)port
{
	return 443;
}

- (BOOL)canSendOfflineMessageToContact:(AIListContact *)inContact
{
	return YES;
}

- (void)beginSendOfFileTransfer:(ESFileTransfer *)fileTransfer
{
  [super _beginSendOfFileTransfer:fileTransfer];
}

- (void)acceptFileTransferRequest:(ESFileTransfer *)fileTransfer
{
  [super acceptFileTransferRequest:fileTransfer];
}

- (void)rejectFileReceiveRequest:(ESFileTransfer *)fileTransfer
{
  [super rejectFileReceiveRequest:fileTransfer];
}

- (void)cancelFileTransfer:(ESFileTransfer *)fileTransfer
{
  [super cancelFileTransfer:fileTransfer];
}

@end
