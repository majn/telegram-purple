/**
 * @file proxy.h Proxy API
 * @ingroup core
 */

/* purple
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
#ifndef _PURPLE_PROXY_H_
#define _PURPLE_PROXY_H_

#include <glib.h>
#include "eventloop.h"

/**
 * A type of proxy connection.
 */
typedef enum
{
	PURPLE_PROXY_USE_GLOBAL = -1,  /**< Use the global proxy information. */
	PURPLE_PROXY_NONE = 0,         /**< No proxy.                         */
	PURPLE_PROXY_HTTP,             /**< HTTP proxy.                       */
	PURPLE_PROXY_SOCKS4,           /**< SOCKS 4 proxy.                    */
	PURPLE_PROXY_SOCKS5,           /**< SOCKS 5 proxy.                    */
	PURPLE_PROXY_USE_ENVVAR,       /**< Use environmental settings.       */
	PURPLE_PROXY_TOR               /**< Use a Tor proxy (SOCKS 5 really)  */

} PurpleProxyType;

/**
 * Information on proxy settings.
 */
typedef struct
{
	PurpleProxyType type;   /**< The proxy type.  */

	char *host;           /**< The host.        */
	int   port;           /**< The port number. */
	char *username;       /**< The username.    */
	char *password;       /**< The password.    */

} PurpleProxyInfo;

typedef struct _PurpleProxyConnectData PurpleProxyConnectData;

typedef void (*PurpleProxyConnectFunction)(gpointer data, gint source, const gchar *error_message);


#include "account.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Proxy structure API                                             */
/**************************************************************************/
/*@{*/

/**
 * Creates a proxy information structure.
 *
 * @return The proxy information structure.
 */
PurpleProxyInfo *purple_proxy_info_new(void);

/**
 * Destroys a proxy information structure.
 *
 * @param info The proxy information structure to destroy.
 */
void purple_proxy_info_destroy(PurpleProxyInfo *info);

/**
 * Sets the type of proxy.
 *
 * @param info The proxy information.
 * @param type The proxy type.
 */
void purple_proxy_info_set_type(PurpleProxyInfo *info, PurpleProxyType type);

/**
 * Sets the proxy host.
 *
 * @param info The proxy information.
 * @param host The host.
 */
void purple_proxy_info_set_host(PurpleProxyInfo *info, const char *host);

/**
 * Sets the proxy port.
 *
 * @param info The proxy information.
 * @param port The port.
 */
void purple_proxy_info_set_port(PurpleProxyInfo *info, int port);

/**
 * Sets the proxy username.
 *
 * @param info     The proxy information.
 * @param username The username.
 */
void purple_proxy_info_set_username(PurpleProxyInfo *info, const char *username);

/**
 * Sets the proxy password.
 *
 * @param info     The proxy information.
 * @param password The password.
 */
void purple_proxy_info_set_password(PurpleProxyInfo *info, const char *password);

/**
 * Returns the proxy's type.
 *
 * @param info The proxy information.
 *
 * @return The type.
 */
PurpleProxyType purple_proxy_info_get_type(const PurpleProxyInfo *info);

/**
 * Returns the proxy's host.
 *
 * @param info The proxy information.
 *
 * @return The host.
 */
const char *purple_proxy_info_get_host(const PurpleProxyInfo *info);

/**
 * Returns the proxy's port.
 *
 * @param info The proxy information.
 *
 * @return The port.
 */
int purple_proxy_info_get_port(const PurpleProxyInfo *info);

/**
 * Returns the proxy's username.
 *
 * @param info The proxy information.
 *
 * @return The username.
 */
const char *purple_proxy_info_get_username(const PurpleProxyInfo *info);

/**
 * Returns the proxy's password.
 *
 * @param info The proxy information.
 *
 * @return The password.
 */
const char *purple_proxy_info_get_password(const PurpleProxyInfo *info);

/*@}*/

/**************************************************************************/
/** @name Global Proxy API                                                */
/**************************************************************************/
/*@{*/

/**
 * Returns purple's global proxy information.
 *
 * @return The global proxy information.
 */
PurpleProxyInfo *purple_global_proxy_get_info(void);

/**
 * Set purple's global proxy information.
 *
 * @param info     The proxy information.
 * @since 2.6.0
 */
void purple_global_proxy_set_info(PurpleProxyInfo *info);

/*@}*/

/**************************************************************************/
/** @name Proxy API                                                       */
/**************************************************************************/
/*@{*/

/**
 * Returns the proxy subsystem handle.
 *
 * @return The proxy subsystem handle.
 */
void *purple_proxy_get_handle(void);

/**
 * Initializes the proxy subsystem.
 */
void purple_proxy_init(void);

/**
 * Uninitializes the proxy subsystem.
 */
void purple_proxy_uninit(void);

/**
 * Returns configuration of a proxy.
 *
 * @param account The account for which the configuration is needed.
 *
 * @return The configuration of a proxy.
 */
PurpleProxyInfo *purple_proxy_get_setup(PurpleAccount *account);

/**
 * Makes a connection to the specified host and port.  Note that this
 * function name can be misleading--although it is called "proxy
 * connect," it is used for establishing any outgoing TCP connection,
 * whether through a proxy or not.
 *
 * @param handle     A handle that should be associated with this
 *                   connection attempt.  The handle can be used
 *                   to cancel the connection attempt using the
 *                   purple_proxy_connect_cancel_with_handle()
 *                   function.
 * @param account    The account making the connection.
 * @param host       The destination host.
 * @param port       The destination port.
 * @param connect_cb The function to call when the connection is
 *                   established.  If the connection failed then
 *                   fd will be -1 and error message will be set
 *                   to something descriptive (hopefully).
 * @param data       User-defined data.
 *
 * @return NULL if there was an error, or a reference to an
 *         opaque data structure that can be used to cancel
 *         the pending connection, if needed.
 */
PurpleProxyConnectData *purple_proxy_connect(void *handle,
			PurpleAccount *account,
			const char *host, int port,
			PurpleProxyConnectFunction connect_cb, gpointer data);

/**
 * Makes a connection to the specified host and port.  Note that this
 * function name can be misleading--although it is called "proxy
 * connect," it is used for establishing any outgoing UDP connection,
 * whether through a proxy or not.
 *
 * @param handle     A handle that should be associated with this
 *                   connection attempt.  The handle can be used
 *                   to cancel the connection attempt using the
 *                   purple_proxy_connect_cancel_with_handle()
 *                   function.
 * @param account    The account making the connection.
 * @param host       The destination host.
 * @param port       The destination port.
 * @param connect_cb The function to call when the connection is
 *                   established.  If the connection failed then
 *                   fd will be -1 and error message will be set
 *                   to something descriptive (hopefully).
 * @param data       User-defined data.
 *
 * @return NULL if there was an error, or a reference to an
 *         opaque data structure that can be used to cancel
 *         the pending connection, if needed.
 */
PurpleProxyConnectData *purple_proxy_connect_udp(void *handle,
			PurpleAccount *account,
			const char *host, int port,
			PurpleProxyConnectFunction connect_cb, gpointer data);

/**
 * Makes a connection through a SOCKS5 proxy.
 *
 * Note that if the account that is making the connection uses a proxy, this
 * connection to a SOCKS5 proxy will be made through the account proxy.
 *
 * @param handle     A handle that should be associated with this
 *                   connection attempt.  The handle can be used
 *                   to cancel the connection attempt using the
 *                   purple_proxy_connect_cancel_with_handle()
 *                   function.
 * @param account    The account making the connection.
 * @param gpi        The PurpleProxyInfo specifying the proxy settings
 * @param host       The destination host.
 * @param port       The destination port.
 * @param connect_cb The function to call when the connection is
 *                   established.  If the connection failed then
 *                   fd will be -1 and error message will be set
 *                   to something descriptive (hopefully).
 * @param data       User-defined data.
 *
 * @return NULL if there was an error, or a reference to an
 *         opaque data structure that can be used to cancel
 *         the pending connection, if needed.
 */
PurpleProxyConnectData *purple_proxy_connect_socks5_account(void *handle,
			PurpleAccount *account, PurpleProxyInfo *gpi,
			const char *host, int port,
			PurpleProxyConnectFunction connect_cb, gpointer data);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_PROXY_C_)
/**
 * Makes a connection through a SOCKS5 proxy.
 *
 * @param handle     A handle that should be associated with this
 *                   connection attempt.  The handle can be used
 *                   to cancel the connection attempt using the
 *                   purple_proxy_connect_cancel_with_handle()
 *                   function.
 * @param gpi        The PurpleProxyInfo specifying the proxy settings
 * @param host       The destination host.
 * @param port       The destination port.
 * @param connect_cb The function to call when the connection is
 *                   established.  If the connection failed then
 *                   fd will be -1 and error message will be set
 *                   to something descriptive (hopefully).
 * @param data       User-defined data.
 *
 * @return NULL if there was an error, or a reference to an
 *         opaque data structure that can be used to cancel
 *         the pending connection, if needed.
 * @deprecated Use purple_proxy_connect_socks5_account instead
 */
PurpleProxyConnectData *purple_proxy_connect_socks5(void *handle,
			PurpleProxyInfo *gpi,
			const char *host, int port,
			PurpleProxyConnectFunction connect_cb, gpointer data);
#endif

/**
 * Cancel an in-progress connection attempt.  This should be called
 * by the PRPL if the user disables an account while it is still
 * performing the initial sign on.  Or when establishing a file
 * transfer, if we attempt to connect to a remote user but they
 * are behind a firewall then the PRPL can cancel the connection
 * attempt early rather than just letting the OS's TCP/IP stack
 * time-out the connection.
 */
void purple_proxy_connect_cancel(PurpleProxyConnectData *connect_data);

/*
 * Closes all proxy connections registered with the specified handle.
 *
 * @param handle The handle.
 */
void purple_proxy_connect_cancel_with_handle(void *handle);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_PROXY_H_ */
