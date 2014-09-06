/**
 * @file stun.h STUN API
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
#ifndef _PURPLE_STUN_H_
#define _PURPLE_STUN_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name STUN API                                                        */
/**************************************************************************/
/*@{*/

typedef struct _PurpleStunNatDiscovery PurpleStunNatDiscovery;

typedef enum {
	PURPLE_STUN_STATUS_UNDISCOVERED = -1,
	PURPLE_STUN_STATUS_UNKNOWN, /* no STUN server reachable */
	PURPLE_STUN_STATUS_DISCOVERING,
	PURPLE_STUN_STATUS_DISCOVERED
} PurpleStunStatus;

typedef enum {
	PURPLE_STUN_NAT_TYPE_PUBLIC_IP,
	PURPLE_STUN_NAT_TYPE_UNKNOWN_NAT,
	PURPLE_STUN_NAT_TYPE_FULL_CONE,
	PURPLE_STUN_NAT_TYPE_RESTRICTED_CONE,
	PURPLE_STUN_NAT_TYPE_PORT_RESTRICTED_CONE,
	PURPLE_STUN_NAT_TYPE_SYMMETRIC
} PurpleStunNatType;

struct _PurpleStunNatDiscovery {
	PurpleStunStatus status;
	PurpleStunNatType type;
	char publicip[16];
	char *servername;
	time_t lookup_time;
};

typedef void (*StunCallback) (PurpleStunNatDiscovery *);

/**
 * Starts a NAT discovery. It returns a PurpleStunNatDiscovery if the discovery
 * is already done. Otherwise the callback is called when the discovery is over
 * and NULL is returned.
 *
 * @param cb The callback to call when the STUN discovery is finished if the
 *           discovery would block.  If the discovery is done, this is NOT
 *           called.
 *
 * @return a PurpleStunNatDiscovery which includes the public IP and the type
 *         of NAT or NULL is discovery would block
 */
PurpleStunNatDiscovery *purple_stun_discover(StunCallback cb);

void purple_stun_init(void);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_STUN_H_ */
