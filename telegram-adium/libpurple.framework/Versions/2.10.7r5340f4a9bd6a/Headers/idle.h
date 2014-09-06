/**
 * @file idle.h Idle API
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
#ifndef _PURPLE_IDLE_H_
#define _PURPLE_IDLE_H_

#include <time.h>

/**
 * Idle UI operations.
 */
typedef struct
{
	time_t (*get_time_idle)(void);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
} PurpleIdleUiOps;

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Idle API                                                        */
/**************************************************************************/
/*@{*/

/**
 * Touch our idle tracker.  This signifies that the user is
 * 'active'.  The conversation code calls this when the
 * user sends an IM, for example.
 */
void purple_idle_touch(void);

/**
 * Fake our idle time by setting the time at which our
 * accounts purportedly became idle.  This is used by
 * the I'dle Mak'er plugin.
 */
void purple_idle_set(time_t time);

/*@}*/

/**************************************************************************/
/** @name Idle Subsystem                                                  */
/**************************************************************************/
/*@{*/

/**
 * Sets the UI operations structure to be used for idle reporting.
 *
 * @param ops The UI operations structure.
 */
void purple_idle_set_ui_ops(PurpleIdleUiOps *ops);

/**
 * Returns the UI operations structure used for idle reporting.
 *
 * @return The UI operations structure in use.
 */
PurpleIdleUiOps *purple_idle_get_ui_ops(void);

/**
 * Initializes the idle system.
 */
void purple_idle_init(void);

/**
 * Uninitializes the idle system.
 */
void purple_idle_uninit(void);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_IDLE_H_ */
