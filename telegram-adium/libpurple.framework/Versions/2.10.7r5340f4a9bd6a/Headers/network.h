/**
 * @file network.h Network API
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
#ifndef _PURPLE_NETWORK_H_
#define _PURPLE_NETWORK_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Network API                                                     */
/**************************************************************************/
/*@{*/

typedef struct _PurpleNetworkListenData PurpleNetworkListenData;

typedef void (*PurpleNetworkListenCallback) (int listenfd, gpointer data);

/**
 * Converts a dot-decimal IP address to an array of unsigned
 * chars.  For example, converts 192.168.0.1 to a 4 byte
 * array containing 192, 168, 0 and 1.
 *
 * @param ip An IP address in dot-decimal notiation.
 * @return An array of 4 bytes containing an IP addresses
 *         equivalent to the given parameter, or NULL if
 *         the given IP address is invalid.  This value
 *         is statically allocated and should not be
 *         freed.
 */
const unsigned char *purple_network_ip_atoi(const char *ip);

/**
 * Sets the IP address of the local system in preferences.  This
 * is the IP address that should be used for incoming connections
 * (file transfer, direct IM, etc.) and should therefore be
 * publicly accessible.
 *
 * @param ip The local IP address.
 */
void purple_network_set_public_ip(const char *ip);

/**
 * Returns the IP address of the local system set in preferences.
 *
 * This returns the value set via purple_network_set_public_ip().
 * You probably want to use purple_network_get_my_ip() instead.
 *
 * @return The local IP address set in preferences.
 */
const char *purple_network_get_public_ip(void);

/**
 * Returns the IP address of the local system.
 *
 * You probably want to use purple_network_get_my_ip() instead.
 *
 * @note The returned string is a pointer to a static buffer. If this
 *       function is called twice, it may be important to make a copy
 *       of the returned string.
 *
 * @param fd The fd to use to help figure out the IP, or else -1.
 * @return The local IP address.
 */
const char *purple_network_get_local_system_ip(int fd);

/**
 * Returns all IP addresses of the local system.
 *
 * @note The caller must free this list.  If libpurple was built with
 *       support for it, this function also enumerates IPv6 addresses.
 * @since 2.7.0
 *
 * @return A list of local IP addresses.
 */
GList *purple_network_get_all_local_system_ips(void);

/**
 * Returns the IP address that should be used anywhere a
 * public IP addresses is needed (listening for an incoming
 * file transfer, etc).
 *
 * If the user has manually specified an IP address via
 * preferences, then this IP is returned.  Otherwise the
 * IP address returned by purple_network_get_local_system_ip()
 * is returned.
 *
 * @note The returned string is a pointer to a static buffer. If this
 *       function is called twice, it may be important to make a copy
 *       of the returned string.
 *
 * @param fd The fd to use to help figure out the IP, or -1.
 * @return The local IP address to be used.
 */
const char *purple_network_get_my_ip(int fd);

/**
 * Should calls to purple_network_listen() and purple_network_listen_range()
 * map the port externally using NAT-PMP or UPnP?
 * The default value is TRUE
 *
 * @param map_external Should the open port be mapped externally?
 * @deprecated In 3.0.0 a boolean will be added to the functions mentioned
 *             above to perform the same function.
 * @since 2.3.0
 */
void purple_network_listen_map_external(gboolean map_external);

/**
 * Attempts to open a listening port ONLY on the specified port number.
 * You probably want to use purple_network_listen_range() instead of this.
 * This function is useful, for example, if you wanted to write a telnet
 * server as a Purple plugin, and you HAD to listen on port 23.  Why anyone
 * would want to do that is beyond me.
 *
 * This opens a listening port. The caller will want to set up a watcher
 * of type PURPLE_INPUT_READ on the fd returned in cb. It will probably call
 * accept in the watcher callback, and then possibly remove the watcher and
 * close the listening socket, and add a new watcher on the new socket accept
 * returned.
 *
 * @param port The port number to bind to.  Must be greater than 0.
 * @param socket_type The type of socket to open for listening.
 *   This will be either SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
 * @param cb The callback to be invoked when the port to listen on is available.
 *           The file descriptor of the listening socket will be specified in
 *           this callback, or -1 if no socket could be established.
 * @param cb_data extra data to be returned when cb is called
 *
 * @return A pointer to a data structure that can be used to cancel
 *         the pending listener, or NULL if unable to obtain a local
 *         socket to listen on.
 */
PurpleNetworkListenData *purple_network_listen(unsigned short port,
		int socket_type, PurpleNetworkListenCallback cb, gpointer cb_data);

/**
 * \copydoc purple_network_listen
 *
 * Libpurple does not currently do any port mapping (stateful firewall hole
 * poking) for IPv6-only listeners (if an IPv6 socket supports v4-mapped
 * addresses, a mapping is done).
 *
 * @param socket_family The protocol family of the socket.  This should be
 *                      AF_INET for IPv4 or AF_INET6 for IPv6.  IPv6 sockets
 *                      may or may not be able to accept IPv4 connections
 *                      based on the system configuration (use
 *                      purple_socket_speaks_ipv4 to check).  If an IPv6
 *                      socket doesn't accept V4-mapped addresses, you will
 *                      need a second listener to support both v4 and v6.
 * @since 2.7.0
 * @deprecated This function will be renamed to purple_network_listen in 3.0.0.
 */
PurpleNetworkListenData *purple_network_listen_family(unsigned short port,
	int socket_family, int socket_type, PurpleNetworkListenCallback cb,
	gpointer cb_data);

/**
 * Opens a listening port selected from a range of ports.  The range of
 * ports used is chosen in the following manner:
 * If a range is specified in preferences, these values are used.
 * If a non-0 values are passed to the function as parameters, these
 * values are used.
 * Otherwise a port is chosen at random by the operating system.
 *
 * This opens a listening port. The caller will want to set up a watcher
 * of type PURPLE_INPUT_READ on the fd returned in cb. It will probably call
 * accept in the watcher callback, and then possibly remove the watcher and close
 * the listening socket, and add a new watcher on the new socket accept
 * returned.
 *
 * @param start The port number to bind to, or 0 to pick a random port.
 *              Users are allowed to override this arg in prefs.
 * @param end The highest possible port in the range of ports to listen on,
 *            or 0 to pick a random port.  Users are allowed to override this
 *            arg in prefs.
 * @param socket_type The type of socket to open for listening.
 *   This will be either SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
 * @param cb The callback to be invoked when the port to listen on is available.
 *           The file descriptor of the listening socket will be specified in
 *           this callback, or -1 if no socket could be established.
 * @param cb_data extra data to be returned when cb is called
 *
 * @return A pointer to a data structure that can be used to cancel
 *         the pending listener, or NULL if unable to obtain a local
 *         socket to listen on.
 */
PurpleNetworkListenData *purple_network_listen_range(unsigned short start,
		unsigned short end, int socket_type,
		PurpleNetworkListenCallback cb, gpointer cb_data);

/**
 * \copydoc purple_network_listen_range
 *
 * Libpurple does not currently do any port mapping (stateful firewall hole
 * poking) for IPv6-only listeners (if an IPv6 socket supports v4-mapped
 * addresses, a mapping is done).
 *
 * @param socket_family The protocol family of the socket.  This should be
 *                      AF_INET for IPv4 or AF_INET6 for IPv6.  IPv6 sockets
 *                      may or may not be able to accept IPv4 connections
 *                      based on the system configuration (use
 *                      purple_socket_speaks_ipv4 to check).  If an IPv6
 *                      socket doesn't accept V4-mapped addresses, you will
 *                      need a second listener to support both v4 and v6.
 * @since 2.7.0
 * @deprecated This function will be renamed to purple_network_listen_range
 *             in 3.0.0.
 */
PurpleNetworkListenData *purple_network_listen_range_family(
	unsigned short start, unsigned short end, int socket_family,
	int socket_type, PurpleNetworkListenCallback cb, gpointer cb_data);

/**
 * This can be used to cancel any in-progress listener connection
 * by passing in the return value from either purple_network_listen()
 * or purple_network_listen_range().
 *
 * @param listen_data This listener attempt will be cancelled and
 *        the struct will be freed.
 */
void purple_network_listen_cancel(PurpleNetworkListenData *listen_data);

/**
 * Gets a port number from a file descriptor.
 *
 * @param fd The file descriptor. This should be a tcp socket. The current
 *           implementation probably dies on anything but IPv4. Perhaps this
 *           possible bug will inspire new and valuable contributors to Purple.
 * @return The port number, in host byte order.
 */
unsigned short purple_network_get_port_from_fd(int fd);

/**
 * Detects if there is an available network connection.
 *
 * @return TRUE if the network is available
 */
gboolean purple_network_is_available(void);

/**
 * Makes purple_network_is_available() always return @c TRUE.
 *
 * This is what backs the --force-online command line argument in Pidgin,
 * for example.  This is useful for offline testing, especially when
 * combined with nullprpl.
 *
 * @since 2.6.0
 */
void purple_network_force_online(void);

/**
 * Get the handle for the network system
 *
 * @return the handle to the network system
 */
void *purple_network_get_handle(void);

/**
 * Update the STUN server IP given the host name
 * Will result in a DNS query being executed asynchronous
 *
 * @param stun_server The host name of the STUN server to set
 * @since 2.6.0
 */
void purple_network_set_stun_server(const gchar *stun_server);

/**
 * Get the IP address of the STUN server as a string representation
 *
 * @return the IP address
 * @since 2.6.0
 */
const gchar *purple_network_get_stun_ip(void);

/**
 * Update the TURN server IP given the host name
 * Will result in a DNS query being executed asynchronous
 *
 * @param turn_server The host name of the TURN server to set
 * @since 2.6.0
 */
void purple_network_set_turn_server(const gchar *turn_server);

/**
 * Get the IP address of the TURN server as a string representation
 *
 * @return the IP address
 * @since 2.6.0
 */
const gchar *purple_network_get_turn_ip(void);

/**
 * Remove a port mapping (UPnP or NAT-PMP) associated with listening socket
 *
 * @param fd Socket to remove the port mapping for
 * @since 2.6.0
 */
void purple_network_remove_port_mapping(gint fd);

/**
 * Convert a UTF-8 domain name to ASCII in accordance with the IDNA
 * specification. If libpurple is compiled without IDN support, this function
 * copies the input into the output buffer.
 *
 * Because this function is used by DNS resolver child/threads, it uses no
 * other libpurple API and is threadsafe.
 *
 * In general, a buffer of about 512 bytes is the appropriate size to use.
 *
 * @param in      The hostname to be converted.
 * @param out     The output buffer where an allocated string will be returned.
 *                The caller is responsible for freeing this.
 * @returns       0 on success, -1 if the out is NULL, or an error code
 *                that currently corresponds to the Idna_rc enum in libidn.
 * @since 2.6.0
 */
int purple_network_convert_idn_to_ascii(const gchar *in, gchar **out);

/**
 * Initializes the network subsystem.
 */
void purple_network_init(void);

/**
 * Shuts down the network subsystem.
 */
void purple_network_uninit(void);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_NETWORK_H_ */
