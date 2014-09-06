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

#import "CBPurpleAccount.h"

#define KEY_JABBER_CONNECT_SERVER		@"Jabber:Connect Server"
#define KEY_JABBER_BOSH_SERVER			@"Jabber:BOSH Server"
#define KEY_JABBER_PRIORITY_AVAILABLE	@"Jabber:Priority when Available"
#define KEY_JABBER_PRIORITY_AWAY		@"Jabber:Priority when Away"
#define KEY_JABBER_RESOURCE				@"Jabber:Resource"
#define KEY_JABBER_USE_TLS				@"Jabber:Use TLS"
#define KEY_JABBER_FORCE_OLD_SSL		@"Jabber:Force Old SSL"
#define KEY_JABBER_REQUIRE_TLS			@"Jabber:Require TLS"
#define KEY_JABBER_TLS					@"Jabber:TLS"
#define KEY_JABBER_ALLOW_PLAINTEXT		@"Jabber:Allow Plaintext Authentication"
#define KEY_JABBER_SUBSCRIPTION_BEHAVIOR @"Jabber:Subscription Behavior"
#define KEY_JABBER_REGISTER_ON_CONNECT @"Jabber:Register on Connect"
#define KEY_JABBER_SUBSCRIPTION_GROUP	@"Jabber:Default Subscription Group"
#define KEY_JABBER_VERIFY_CERTS         @"Jabber:Verify Certificates"
#define KEY_JABBER_FT_PROXIES			@"Jabber:File Transfer Proxies"

#define PURPLE_SSL_CDSA_BUGGY_TLS_WORKAROUND "ssl_cdsa_buggy_tls_workaround"

typedef enum : short {
	AIJabberTLSAllowed = 1,
	AIJabberTLSRequired,
	AIJabberTLSForwardSecrecRequired
} AIJabberTLSSetting;

@class AIJabberConsoleController, AMPurpleJabberServiceDiscoveryBrowsing, AMPurpleJabberAdHocServer;

@interface ESPurpleJabberAccount : CBPurpleAccount <AIAccount_Files> {
    AIJabberConsoleController *xmlConsoleController;
	AMPurpleJabberServiceDiscoveryBrowsing *discoveryBrowserController;
	
	AMPurpleJabberAdHocServer *adhocServer;
	
	NSMutableArray *gateways;
}

- (NSString *)serverSuffix;
- (NSString *)resourceName;
- (AMPurpleJabberAdHocServer*)adhocServer;

@end
