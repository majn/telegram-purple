/**
 * @file nexus.h MSN Nexus functions
 *
 * purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
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
#ifndef MSN_NEXUS_H
#define MSN_NEXUS_H

#include "internal.h"

typedef struct _MsnNexus MsnNexus;
typedef struct _MsnTicketToken MsnTicketToken;

/* Index into ticket_tokens in nexus.c Keep updated! */
typedef enum
{
	MSN_AUTH_MESSENGER     = 0,
	MSN_AUTH_MESSENGER_WEB = 1,
	MSN_AUTH_CONTACTS      = 2,
	MSN_AUTH_LIVE_SECURE   = 3,
	MSN_AUTH_STORAGE       = 4,
	MSN_AUTH_WHATSNEW      = 5
} MsnAuthDomains;

#define MSN_SSO_SERVER	"login.live.com"
#define SSO_POST_URL	"/RST.srf"

#define MSN_SSO_RST_TEMPLATE \
"<wst:RequestSecurityToken xmlns=\"http://schemas.xmlsoap.org/ws/2004/04/trust\" Id=\"RST%d\">"\
	"<wst:RequestType>http://schemas.xmlsoap.org/ws/2004/04/security/trust/Issue</wst:RequestType>"\
	"<wsp:AppliesTo xmlns=\"http://schemas.xmlsoap.org/ws/2002/12/policy\">"\
		"<wsa:EndpointReference xmlns=\"http://schemas.xmlsoap.org/ws/2004/03/addressing\">"\
			"<wsa:Address>%s</wsa:Address>"\
		"</wsa:EndpointReference>"\
	"</wsp:AppliesTo>"\
	"<wsse:PolicyReference xmlns=\"http://schemas.xmlsoap.org/ws/2003/06/secext\" URI=\"%s\"></wsse:PolicyReference>"\
"</wst:RequestSecurityToken>"

#define MSN_SSO_TEMPLATE "<?xml version='1.0' encoding='utf-8'?>"\
"<Envelope xmlns=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\""\
	" xmlns:saml=\"urn:oasis:names:tc:SAML:1.0:assertion\""\
	" xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2002/12/policy\""\
	" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\""\
	" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/03/addressing\""\
	" xmlns:wssc=\"http://schemas.xmlsoap.org/ws/2004/04/sc\""\
	" xmlns:wst=\"http://schemas.xmlsoap.org/ws/2004/04/trust\">"\
	"<Header>"\
		"<ps:AuthInfo"\
			" xmlns:ps=\"http://schemas.microsoft.com/Passport/SoapServices/PPCRL\""\
			" Id=\"PPAuthInfo\">"\
			"<ps:HostingApp>{7108E71A-9926-4FCB-BCC9-9A9D3F32E423}</ps:HostingApp>"\
			"<ps:BinaryVersion>4</ps:BinaryVersion>"\
			"<ps:UIVersion>1</ps:UIVersion>"\
			"<ps:Cookies></ps:Cookies>"\
			"<ps:RequestParams>AQAAAAIAAABsYwQAAAAxMDMz</ps:RequestParams>"\
		"</ps:AuthInfo>"\
		"<wsse:Security>"\
			"<wsse:UsernameToken Id=\"user\">"\
				"<wsse:Username>%s</wsse:Username>"\
				"<wsse:Password>%s</wsse:Password>"\
			"</wsse:UsernameToken>"\
		"</wsse:Security>"\
	"</Header>"\
	"<Body>"\
		"<ps:RequestMultipleSecurityTokens"\
			" xmlns:ps=\"http://schemas.microsoft.com/Passport/SoapServices/PPCRL\""\
			" Id=\"RSTS\">"\
			"<wst:RequestSecurityToken Id=\"RST0\">"\
				"<wst:RequestType>http://schemas.xmlsoap.org/ws/2004/04/security/trust/Issue</wst:RequestType>"\
				"<wsp:AppliesTo>"\
					"<wsa:EndpointReference>"\
						"<wsa:Address>http://Passport.NET/tb</wsa:Address>"\
					"</wsa:EndpointReference>"\
				"</wsp:AppliesTo>"\
			"</wst:RequestSecurityToken>"\
			"%s"	/* Other RSTn tokens */\
		"</ps:RequestMultipleSecurityTokens>"\
	"</Body>"\
"</Envelope>"

#define MSN_SSO_AUTHINFO_TEMPLATE \
"<ps:AuthInfo xmlns:ps=\"http://schemas.microsoft.com/Passport/SoapServices/PPCRL\" Id=\"PPAuthInfo\">"\
	"<ps:HostingApp>{7108E71A-9926-4FCB-BCC9-9A9D3F32E423}</ps:HostingApp>"\
	"<ps:BinaryVersion>4</ps:BinaryVersion>"\
	"<ps:UIVersion>1</ps:UIVersion>"\
	"<ps:Cookies></ps:Cookies>"\
	"<ps:RequestParams>AQAAAAIAAABsYwQAAAA0MTA1</ps:RequestParams>"\
"</ps:AuthInfo>"
/* Not sure what's editable here, so I'll just hard-code the SHA1 hash */
#define MSN_SSO_AUTHINFO_SHA1_BASE64 "d2IeTF4DAkPEa/tVETHznsivEpc="

#define MSN_SSO_TIMESTAMP_TEMPLATE \
"<wsu:Timestamp xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\" Id=\"Timestamp\">"\
	"<wsu:Created>%s</wsu:Created>"\
	"<wsu:Expires>%s</wsu:Expires>"\
"</wsu:Timestamp>"

#define MSN_SSO_SIGNEDINFO_TEMPLATE \
"<SignedInfo xmlns=\"http://www.w3.org/2000/09/xmldsig#\">"\
	"<CanonicalizationMethod Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"></CanonicalizationMethod>"\
	"<SignatureMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#hmac-sha1\"></SignatureMethod>"\
	"<Reference URI=\"#RST%d\">"\
		"<Transforms>"\
			"<Transform Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"></Transform>"\
		"</Transforms>"\
		"<DigestMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\"></DigestMethod>"\
		"<DigestValue>%s</DigestValue>"\
	"</Reference>"\
	"<Reference URI=\"#Timestamp\">"\
		"<Transforms>"\
			"<Transform Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"></Transform>"\
		"</Transforms>"\
		"<DigestMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\"></DigestMethod>"\
		"<DigestValue>%s</DigestValue>"\
	"</Reference>"\
	"<Reference URI=\"#PPAuthInfo\">"\
		"<Transforms>"\
			"<Transform Algorithm=\"http://www.w3.org/2001/10/xml-exc-c14n#\"></Transform>"\
		"</Transforms>"\
		"<DigestMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\"></DigestMethod>"\
		"<DigestValue>" MSN_SSO_AUTHINFO_SHA1_BASE64 "</DigestValue>"\
	"</Reference>"\
"</SignedInfo>"

#define MSN_SSO_TOKEN_UPDATE_TEMPLATE "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<Envelope"\
	" xmlns=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\""\
	" xmlns:saml=\"urn:oasis:names:tc:SAML:1.0:assertion\""\
	" xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2002/12/policy\""\
	" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\""\
	" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/03/addressing\""\
	" xmlns:wssc=\"http://schemas.xmlsoap.org/ws/2004/04/sc\""\
	" xmlns:wst=\"http://schemas.xmlsoap.org/ws/2004/04/trust\">"\
	"<Header>"\
		MSN_SSO_AUTHINFO_TEMPLATE /* ps:AuthInfo */ \
		"<wsse:Security>"\
			"<EncryptedData xmlns=\"http://www.w3.org/2001/04/xmlenc#\" Id=\"BinaryDAToken0\" Type=\"http://www.w3.org/2001/04/xmlenc#Element\">"\
				"<EncryptionMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#tripledes-cbc\"></EncryptionMethod>"\
				"<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">"\
					"<ds:KeyName>http://Passport.NET/STS</ds:KeyName>"\
				"</ds:KeyInfo>"\
				"<CipherData>"\
					"<CipherValue>%s</CipherValue>"\
				"</CipherData>"\
			"</EncryptedData>"\
			"<wssc:DerivedKeyToken Id=\"SignKey\">"\
				"<wsse:RequestedTokenReference>"\
					"<wsse:KeyIdentifier ValueType=\"http://docs.oasis-open.org/wss/2004/XX/oasis-2004XX-wss-saml-token-profile-1.0#SAMLAssertionID\" />"\
					"<wsse:Reference URI=\"#BinaryDAToken0\" />"\
				"</wsse:RequestedTokenReference>"\
				"<wssc:Nonce>%s</wssc:Nonce>"\
			"</wssc:DerivedKeyToken>"\
			"%s" /* wsu:Timestamp */\
			"<Signature xmlns=\"http://www.w3.org/2000/09/xmldsig#\">"\
				"%s" /* SignedInfo */\
				"<SignatureValue>%s</SignatureValue>"\
				"<KeyInfo>"\
					"<wsse:SecurityTokenReference>"\
						"<wsse:Reference URI=\"#SignKey\" />"\
					"</wsse:SecurityTokenReference>"\
				"</KeyInfo>"\
			"</Signature>"\
		"</wsse:Security>"\
	"</Header>"\
	"<Body>"\
		"%s" /* wst:RequestSecurityToken */ \
	"</Body>"\
"</Envelope>"

struct _MsnTicketToken {
	GHashTable *token;
	char *secret;
	time_t expiry;
	GSList *updates;
};

struct _MsnNexus
{
	MsnSession *session;

	/* From server via USR command */
	char *policy;
	char *nonce;

	/* From server via SOAP stuff */
	char *cipher;
	char *secret;
	MsnTicketToken *tokens;
	int token_len;
};

void msn_nexus_connect(MsnNexus *nexus);
MsnNexus *msn_nexus_new(MsnSession *session);
void msn_nexus_destroy(MsnNexus *nexus);
GHashTable *msn_nexus_get_token(MsnNexus *nexus, MsnAuthDomains id);
const char *msn_nexus_get_token_str(MsnNexus *nexus, MsnAuthDomains id);
void msn_nexus_update_token(MsnNexus *nexus, int id, GSourceFunc cb, gpointer data);

#endif /* MSN_NEXUS_H */
