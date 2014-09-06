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

#define	FileTransfer_NewFileTransfer	@"NewFileTransfer"

#define	PREF_GROUP_FILE_TRANSFER		@"FileTransfer"

#define	KEY_FT_AUTO_ACCEPT				@"FT AutoAccept"
#define KEY_FT_AUTO_OPEN_SAFE			@"FT AutoOpenSafe"
#define	KEY_FT_AUTO_CLEAR_COMPLETED		@"FT AutoClearCompleted"
#define	KEY_FT_SHOW_PROGRESS_WINDOW		@"FT ShowProgressWindow"

typedef enum {
	Unknown_FileTransfer = 0,
    Incoming_FileTransfer,
    Outgoing_FileTransfer,
} AIFileTransferType;

typedef enum {
	Unknown_Status_FileTransfer = 0,
	Not_Started_FileTransfer,		//File transfer has not yet started
	Checksumming_Filetransfer,		//Calculating a checksum for a file that is to be sent
	Waiting_on_Remote_User_FileTransfer, //Is pending confirmation from the remote user
	Connecting_FileTransfer,		//Is negotiating a connection
	Accepted_FileTransfer,			//Could also be called Began_FileTransfer or Started_FileTransfer
	In_Progress_FileTransfer,		//Currently transferring, not yet complete
	Complete_FileTransfer,			//File is complete; transferring is finished.
	Cancelled_Local_FileTransfer,	//The local user cancelled the transfer
	Cancelled_Remote_FileTransfer,	//The remote user cancelled the transfer
	Failed_FileTransfer				//The transfer failed.
} AIFileTransferStatus;

typedef enum {
	AutoAccept_None = 0,
    AutoAccept_All,
    AutoAccept_FromContactList,
} AIFileTransferAutoAcceptType;

@class ESFileTransfer, AIAccount, AIListContact;

@protocol AIFileTransferController <AIController>
//Should be the only vendor of new ESFileTransfer* objects, as it creates, tracks, and returns them
- (ESFileTransfer *)newFileTransferWithContact:(AIListContact *)inContact forAccount:(AIAccount *)inAccount type:(AIFileTransferType)type __attribute__((objc_method_family(none)));

- (NSArray *)fileTransferArray;

- (NSUInteger)activeTransferCount;

- (void)receiveRequestForFileTransfer:(ESFileTransfer *)fileTransfer;

- (void)fileTransfer:(ESFileTransfer *)fileTransfer didSetStatus:(AIFileTransferStatus)status;

- (void)sendFile:(NSString *)inFile toListContact:(AIListContact *)listContact;
- (void)requestForSendingFileToListContact:(AIListContact *)listContact;

- (NSString *)stringForSize:(unsigned long long)inSize;
- (NSString *)stringForSize:(unsigned long long)inSize of:(unsigned long long)totalSize ofString:(NSString *)totalSizeString;

- (void)_removeFileTransfer:(ESFileTransfer *)fileTransfer;
@end
