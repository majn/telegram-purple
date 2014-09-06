/**
 * @file dnssrv.h
 */

/* purple
 *
 * Copyright (C) 2005, Thomas Butter <butter@uni-mannheim.de>
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

#ifndef _PURPLE_DNSSRV_H
#define _PURPLE_DNSSRV_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PurpleSrvTxtQueryData PurpleSrvTxtQueryData;
typedef struct _PurpleSrvResponse PurpleSrvResponse;
typedef struct _PurpleTxtResponse PurpleTxtResponse;

/* For compatibility, should be removed for 3.0.0
 */
typedef struct _PurpleSrvTxtQueryData PurpleSrvQueryData;

#include <glib.h>

enum PurpleDnsType {
	PurpleDnsTypeTxt = 16,
	PurpleDnsTypeSrv = 33
};

struct _PurpleSrvResponse {
	char hostname[256];
	int port;
	int weight;
	int pref;
};

struct _PurpleTxtResponse {
	char *content;
};

typedef void  (*PurpleSrvTxtQueryResolvedCallback) (PurpleSrvTxtQueryData *query_data, GList *records);
typedef void  (*PurpleSrvTxtQueryFailedCallback) (PurpleSrvTxtQueryData *query_data, const gchar *error_message);

/**
 * SRV Request UI operations;  UIs should implement this if they want to do SRV
 * lookups themselves, rather than relying on the core.
 *
 * @see @ref ui-ops
 */
typedef struct
{
	/** If implemented, return TRUE if the UI takes responsibility for SRV
	  * queries. When returning FALSE, the standard implementation is used. 
	  * These callbacks MUST be called asynchronously. */
	gboolean (*resolve)(PurpleSrvTxtQueryData *query_data,
	                    PurpleSrvTxtQueryResolvedCallback resolved_cb,
	                    PurpleSrvTxtQueryFailedCallback failed_cb);

	/** Called just before @a query_data is freed; this should cancel any
	 *  further use of @a query_data the UI would make. Unneeded if
	 *  #resolve_host is not implemented.
	 */
	void (*destroy)(PurpleSrvTxtQueryData *query_data);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
} PurpleSrvTxtQueryUiOps;

/**
 * @param resp An array of PurpleSrvResponse of size results.  The array
 *        is sorted based on the order described in the DNS SRV RFC.
 *        Users of this API should try each record in resp in order,
 *        starting at the beginning.
 */
typedef void (*PurpleSrvCallback)(PurpleSrvResponse *resp, int results, gpointer data);

/**
 * Callback that returns the data retrieved from a DNS TXT lookup.
 *
 * @param responses   A GList of PurpleTxtResponse objects.
 * @param data        The extra data passed to purple_txt_resolve.
 */
typedef void (*PurpleTxtCallback)(GList *responses, gpointer data);

/**
 * Queries an SRV record.
 *
 * @param account the account that the query is being done for (or NULL)
 * @param protocol Name of the protocol (e.g. "sip")
 * @param transport Name of the transport ("tcp" or "udp")
 * @param domain Domain name to query (e.g. "blubb.com")
 * @param cb A callback which will be called with the results
 * @param extradata Extra data to be passed to the callback
 *
 * @since 2.8.0
 */
PurpleSrvTxtQueryData *purple_srv_resolve_account(PurpleAccount *account, const char *protocol, const char *transport, const char *domain, PurpleSrvCallback cb, gpointer extradata);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_DNSSRV_C_)
/**
 * Queries an SRV record.
 *
 * @param protocol Name of the protocol (e.g. "sip")
 * @param transport Name of the transport ("tcp" or "udp")
 * @param domain Domain name to query (e.g. "blubb.com")
 * @param cb A callback which will be called with the results
 * @param extradata Extra data to be passed to the callback
 *
 * @deprecated Use purple_srv_resolve_account instead
 */
PurpleSrvTxtQueryData *purple_srv_resolve(const char *protocol, const char *transport, const char *domain, PurpleSrvCallback cb, gpointer extradata);
#endif

/**
 * Cancel an SRV or DNS query.
 *
 * @param query_data The request to cancel.
 *
 * @deprecated Use purple_srv_txt_query_destroy instead
 */
void purple_srv_cancel(PurpleSrvTxtQueryData *query_data);

/**
 * Queries an TXT record.
 *
 * @param account the account that the query is being done for (or NULL)
 * @param owner Name of the protocol (e.g. "_xmppconnect")
 * @param domain Domain name to query (e.g. "blubb.com")
 * @param cb A callback which will be called with the results
 * @param extradata Extra data to be passed to the callback
 *
 * @since 2.8.0
 */
PurpleSrvTxtQueryData *purple_txt_resolve_account(PurpleAccount *account, const char *owner, const char *domain, PurpleTxtCallback cb, gpointer extradata);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_DNSSRV_C_)
/**
 * Queries an TXT record.
 *
 * @param owner Name of the protocol (e.g. "_xmppconnect")
 * @param domain Domain name to query (e.g. "blubb.com")
 * @param cb A callback which will be called with the results
 * @param extradata Extra data to be passed to the callback
 *
 * @deprecated Use purple_txt_resolve_account instead
 *
 * @since 2.6.0
 */
PurpleSrvTxtQueryData *purple_txt_resolve(const char *owner, const char *domain, PurpleTxtCallback cb, gpointer extradata);
#endif

/**
 * Cancel an TXT DNS query.
 *
 * @param query_data The request to cancel.
 * @since 2.6.0
 *
 * @deprecated Use purple_srv_txt_query_destroy instead
 */
void purple_txt_cancel(PurpleSrvTxtQueryData *query_data);

/**
 * Get the value of the current TXT record.
 *
 * @param response  The TXT response record
 * @returns The value of the current TXT record.
 * @since 2.6.0
 */
const gchar *purple_txt_response_get_content(PurpleTxtResponse *response);

/**
 * Destroy a TXT DNS response object.
 *
 * @param response The PurpleTxtResponse to destroy.
 * @since 2.6.0
 */
void purple_txt_response_destroy(PurpleTxtResponse *response);

/**
 * Cancel a SRV/TXT query and destroy the associated data structure.
 *
 * @param query_data The SRV/TXT query to cancel.  This data structure
 *        is freed by this function.
 */
void purple_srv_txt_query_destroy(PurpleSrvTxtQueryData *query_data);

/**
 * Sets the UI operations structure to be used when doing a SRV/TXT
 * resolve.  The UI operations need only be set if the UI wants to
 * handle the resolve itself; otherwise, leave it as NULL.
 *
 * @param ops The UI operations structure.
 */
void purple_srv_txt_query_set_ui_ops(PurpleSrvTxtQueryUiOps *ops);

/**
 * Returns the UI operations structure to be used when doing a SRV/TXT
 * resolve.
 *
 * @return The UI operations structure.
 */
PurpleSrvTxtQueryUiOps *purple_srv_txt_query_get_ui_ops(void);

/**
 * Get the query from a PurpleDnsQueryData
 *
 * @param query_data The SRV/TXT query
 * @return The query.
 */
char *purple_srv_txt_query_get_query(PurpleSrvTxtQueryData *query_data);

/**
 * Get the type from a PurpleDnsQueryData (TXT or SRV)
 *
 * @param query_data The query
 * @return The query.
 */
int purple_srv_txt_query_get_type(PurpleSrvTxtQueryData *query_data);

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_DNSSRV_H */
