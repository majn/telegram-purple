/**
 * @file oim.h			Header file for oim.c
 *	Author
 *		MaYuan<mayuan2006@gmail.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA
 */
#ifndef MSN_OIM_H
#define MSN_OIM_H

typedef struct _MsnOim MsnOim;

/* OIM Retrieval Info */
#define MSN_OIM_RETRIEVE_HOST	"rsi.hotmail.com"
#define MSN_OIM_RETRIEVE_URL	"/rsi/rsi.asmx"

/* OIM GetMetadata SOAP Template */
#define MSN_OIM_GET_METADATA_ACTION "http://www.hotmail.msn.com/ws/2004/09/oim/rsi/GetMetadata"

#define MSN_OIM_GET_METADATA_TEMPLATE "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"\
	"<soap:Header>"\
		"<PassportCookie xmlns=\"http://www.hotmail.msn.com/ws/2004/09/oim/rsi\">"\
			"<t>EMPTY</t>"\
			"<p>EMPTY</p>"\
		"</PassportCookie>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<GetMetadata xmlns=\"http://www.hotmail.msn.com/ws/2004/09/oim/rsi\" />"\
	"</soap:Body>"\
"</soap:Envelope>"

/*OIM GetMessage SOAP Template*/
#define MSN_OIM_GET_SOAP_ACTION	"http://www.hotmail.msn.com/ws/2004/09/oim/rsi/GetMessage"

#define MSN_OIM_GET_TEMPLATE "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"\
	"<soap:Header>"\
		"<PassportCookie xmlns=\"http://www.hotmail.msn.com/ws/2004/09/oim/rsi\">"\
			"<t>EMPTY</t>"\
			"<p>EMPTY</p>"\
		"</PassportCookie>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<GetMessage xmlns=\"http://www.hotmail.msn.com/ws/2004/09/oim/rsi\">"\
			"<messageId>%s</messageId>"\
			"<alsoMarkAsRead>false</alsoMarkAsRead>"\
		"</GetMessage>"\
	"</soap:Body>"\
"</soap:Envelope>"

/*OIM DeleteMessages SOAP Template*/
#define MSN_OIM_DEL_SOAP_ACTION	"http://www.hotmail.msn.com/ws/2004/09/oim/rsi/DeleteMessages"

#define MSN_OIM_DEL_TEMPLATE "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"\
	"<soap:Header>"\
		"<PassportCookie xmlns=\"http://www.hotmail.msn.com/ws/2004/09/oim/rsi\">"\
			"<t>EMPTY</t>"\
			"<p>EMPTY</p>"\
		"</PassportCookie>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<DeleteMessages xmlns=\"http://www.hotmail.msn.com/ws/2004/09/oim/rsi\">"\
			"<messageIds>"\
				"<messageId>%s</messageId>"\
			"</messageIds>"\
		"</DeleteMessages>"\
	"</soap:Body>"\
"</soap:Envelope>"

/*OIM Send SOAP Template*/
#define MSN_OIM_MSG_TEMPLATE "MIME-Version: 1.0\n"\
	"Content-Type: text/plain; charset=UTF-8\n"\
	"Content-Transfer-Encoding: base64\n"\
	"X-OIM-Message-Type: OfflineMessage\n"\
	"X-OIM-Run-Id: {%s}\n"\
	"X-OIM-Sequence-Num: %d\n\n"

#define MSN_OIM_SEND_HOST	"ows.messenger.msn.com"
#define MSN_OIM_SEND_URL	"/OimWS/oim.asmx"
#define MSN_OIM_SEND_SOAP_ACTION	"http://messenger.live.com/ws/2006/09/oim/Store2"
#define MSN_OIM_SEND_TEMPLATE "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"\
	"<soap:Header>"\
		"<From"\
			" memberName=\"%s\""\
			" friendlyName=\"%s\""\
			" xml:lang=\"en-US\""\
			" proxy=\"MSNMSGR\""\
			" xmlns=\"http://messenger.msn.com/ws/2004/09/oim/\""\
			" msnpVer=\"MSNP15\""\
			" buildVer=\"8.5.1288\"/>"\
		"<To memberName=\"%s\" xmlns=\"http://messenger.msn.com/ws/2004/09/oim/\"/>"\
		"<Ticket passport=\"EMPTY\" appid=\"%s\" lockkey=\"%s\" xmlns=\"http://messenger.msn.com/ws/2004/09/oim/\"/>"\
		"<Sequence xmlns=\"http://schemas.xmlsoap.org/ws/2003/03/rm\">"\
			"<Identifier xmlns=\"http://schemas.xmlsoap.org/ws/2002/07/utility\">http://messenger.msn.com</Identifier>"\
			"<MessageNumber>%d</MessageNumber>"\
		"</Sequence>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<MessageType xmlns=\"http://messenger.msn.com/ws/2004/09/oim/\">text</MessageType>"\
		"<Content xmlns=\"http://messenger.msn.com/ws/2004/09/oim/\">%s</Content>"\
	"</soap:Body>"\
"</soap:Envelope>"

struct _MsnOim
{
	MsnSession *session;

	GList * oim_list;

	char *challenge;
	char *run_id;
	gint send_seq;
	GQueue *send_queue;
};

/****************************************************
 * function prototype
 * **************************************************/
MsnOim * msn_oim_new(MsnSession *session);
void msn_oim_destroy(MsnOim *oim);

void msn_parse_oim_msg(MsnOim *oim,const char *xmlmsg);

/*Send OIM Message*/
void msn_oim_prep_send_msg_info(MsnOim *oim, const char *membername,
								const char *friendname, const char *tomember,
								const char * msg);

void msn_oim_send_msg(MsnOim *oim);

#endif/* MSN_OIM_H*/
