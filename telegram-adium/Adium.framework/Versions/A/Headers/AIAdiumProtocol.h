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

@class AICoreComponentLoader, AICorePluginLoader;

@protocol AIAccountController, AIChatController, AIContactAlertsController, AIDebugController, AIEmoticonController,
		AIPreferenceController, AIMenuController, AIApplescriptabilityController, AIStatusController,
		AIContentController, AIToolbarController, AISoundController, AIDockController,
		AIFileTransferController, AILoginController, AIInterfaceController, AIContactController;

@protocol AIAdium <NSObject>
@property (readonly, nonatomic) NSObject<AIAccountController> *accountController;
@property (readonly, nonatomic) NSObject<AIChatController> *chatController;
@property (readonly, nonatomic) NSObject<AIContactController> *contactController;
@property (readonly, nonatomic) NSObject<AIContentController> *contentController;
@property (readonly, nonatomic) NSObject<AIDockController> *dockController;
@property (readonly, nonatomic) NSObject<AIEmoticonController> *emoticonController;
@property (readonly, nonatomic) NSObject<AIInterfaceController> *interfaceController;
@property (readonly, nonatomic) NSObject<AILoginController> *loginController;
@property (readonly, nonatomic) NSObject<AIMenuController> *menuController;
@property (readonly, nonatomic) NSObject<AIPreferenceController> *preferenceController;
@property (readonly, nonatomic) NSObject<AISoundController> *soundController;
@property (readonly, nonatomic) NSObject<AIStatusController> *statusController;
@property (readonly, nonatomic) NSObject<AIToolbarController> *toolbarController;
@property (readonly, nonatomic) NSObject<AIContactAlertsController> *contactAlertsController;
@property (readonly, nonatomic) NSObject<AIFileTransferController> *fileTransferController;
@property (readonly, nonatomic) NSObject<AIDebugController> *debugController;
@property (readonly, nonatomic) NSObject<AIApplescriptabilityController> *applescriptabilityController;

@property (readonly, nonatomic) AICoreComponentLoader *componentLoader;
@property (readonly, nonatomic) AICorePluginLoader *pluginLoader;

@property (readonly, nonatomic) NSString *applicationSupportDirectory;
- (NSString *)createResourcePathForName:(NSString *)name;
- (NSArray *)resourcePathsForName:(NSString *)name;
- (NSArray *)allResourcesForName:(NSString *)name withExtensions:(id)extensions;
- (NSString *)pathOfPackWithName:(NSString *)name extension:(NSString *)extension resourceFolderName:(NSString *)folderName;
@property (readonly, nonatomic) NSString *cachesPath;

- (NSComparisonResult)compareVersion:(NSString *)versionA toVersion:(NSString *)versionB;
- (NSString *)earliestLaunchedAdiumVersion;

@property (readonly, nonatomic) BOOL isQuitting;

@end

//Adium events
#define KEY_EVENT_DISPLAY_NAME		@"DisplayName"
#define KEY_EVENT_NOTIFICATION		@"Notification"

//Adium Notifications
#define CONTACT_STATUS_ONLINE_YES			@"Contact_StatusOnlineYes"	// Contact signs on
#define CONTACT_STATUS_ONLINE_NO			@"Contact_StatusOnlineNo"	// Contact signs off
#define CONTACT_STATUS_AWAY_YES				@"Contact_StatusAwayYes"
#define CONTACT_STATUS_AWAY_NO				@"Contact_StatusAwayNo"
#define CONTACT_STATUS_IDLE_YES				@"Contact_StatusIdleYes"
#define CONTACT_STATUS_IDLE_NO				@"Contact_StatusIdleNo"
#define CONTACT_STATUS_MESSAGE				@"Contact_StatusMessage"
#define CONTACT_SEEN_ONLINE_YES				@"Contact_SeenOnlineYes"
#define CONTACT_SEEN_ONLINE_NO				@"Contact_SeenOnlineNo"
#define CONTACT_STATUS_MOBILE_YES			@"Contact_StatusMobileYes"
#define CONTACT_STATUS_MOBILE_NO			@"Contact_StatusMobileNo"
#define CONTENT_MESSAGE_SENT				@"Content_MessageSent"
#define CONTENT_MESSAGE_SENT_GROUP			@"Content_MessageSentGroup"
#define CONTENT_MESSAGE_RECEIVED			@"Content_MessageReceived"
#define CONTENT_MESSAGE_RECEIVED_GROUP		@"Content_MessageReceivedGroup"
#define CONTENT_MESSAGE_RECEIVED_FIRST		@"Content_MessageReceivedFirst"
#define CONTENT_MESSAGE_RECEIVED_BACKGROUND	@"Content_MessageReceivedBackground"
#define CONTENT_MESSAGE_RECEIVED_BACKGROUND_GROUP @"Content_MessageReceivedBackgroundGroup"
#define CONTENT_MESSAGE_RECEIVED_AWAY		@"Content_MessageReceivedAway"
#define CONTENT_MESSAGE_RECEIVED_AWAY_GROUP	@"Content_MessageReceivedAwayGroup"
#define CONTENT_NUDGE_BUZZ_OCCURED			@"Content_NudgeBuzzOccured"
#define CONTENT_CONTACT_JOINED_CHAT			@"Content_ContactJoinedChat"
#define CONTENT_CONTACT_LEFT_CHAT			@"Content_ContactLeftChat"
#define CONTENT_GROUP_CHAT_INVITE			@"Content_GroupChatInvite"
#define CONTENT_GROUP_CHAT_MENTION			@"Content_GroupChatMention"
#define INTERFACE_ERROR_MESSAGE				@"Interface_ErrorMessageReceived"

/* Note: The account connected/disconnected events are aggregated for many accounts connecting simultaneously.
 * Use a list object observer (see AIContactController) if you are concerned about specific account connectivity changes.
 */
#define ACCOUNT_CONNECTED					@"Account_Connected"
#define ACCOUNT_DISCONNECTED				@"Account_Disconnected"

#define	ACCOUNT_RECEIVED_EMAIL				@"Account_NewMailReceived"
#define FILE_TRANSFER_REQUEST				@"FileTransfer_Request"
#define FILE_TRANSFER_CHECKSUMMING			@"FileTransfer_Checksumming"
#define FILE_TRANSFER_WAITING_REMOTE		@"File_Transfer_WaitingRemote"
#define FILE_TRANSFER_BEGAN					@"FileTransfer_Began"
#define FILE_TRANSFER_CANCELLED				@"FileTransfer_Cancelled"
#define FILE_TRANSFER_FAILED				@"FileTransfer_Failed"
#define FILE_TRANSFER_COMPLETE				@"FileTransfer_Complete"

#define AIXtrasDidChangeNotification				@"AIXtrasDidChange"
#define AIApplicationDidFinishLoadingNotification	@"AIApplicationDidFinishLoading"
#define AIAppWillTerminateNotification				@"AIAppWillTerminate"
#define AIShowLogAtPathNotification					@"AIShowLogAtPath"
#define AIShowLogViewerAndReindexNotification		@"AIShowLogViewerAndReindex"
#define AINetworkDidChangeNotification				@"AINetworkDidChange"
