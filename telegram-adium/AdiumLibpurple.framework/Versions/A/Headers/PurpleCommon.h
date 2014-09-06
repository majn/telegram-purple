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

//#define MEANWHILE_NOT_AVAILABLE

//Purple includes
#import <libpurple/libpurple.h>

#define PACKAGE "pidgin"

#ifdef ENABLE_NLS
#  include <locale.h>
#  include <libintl.h>
#  define _(String) ((const char *)dgettext(PACKAGE, String))
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  include <locale.h>
#  define N_(String) (String)
#  ifndef _
#    define _(String) ((const char *)String)
#  endif
#  define ngettext(Singular, Plural, Number) ((Number == 1) ? ((const char *)Singular) : ((const char *)Plural))
#  define dngettext(Domain, Singular, Plural, Number) ((Number == 1) ? ((const char *)Singular) : ((const char *)Plural))
#endif

#define PURPLE_ORPHANS_GROUP_NAME				"__AdiumOrphansUE9FHUE7I"  //A group name no sane user would have

//Events we care about explicitly via signals
typedef enum
{
	PURPLE_BUDDY_NONE				= 0x00, /**< No events.                    */
	PURPLE_BUDDY_SIGNON				= 0x01, /**< The buddy signed on.          */
	PURPLE_BUDDY_SIGNOFF			= 0x02, /**< The buddy signed off.         */
	PURPLE_BUDDY_INFO_UPDATED		= 0x10, /**< The buddy's information (profile) changed.     */
	PURPLE_BUDDY_ICON				= 0x40, /**< The buddy's icon changed.     */
	PURPLE_BUDDY_MISCELLANEOUS		= 0x80, /**< The buddy's service-specific miscalleneous info changed.     */
	PURPLE_BUDDY_SIGNON_TIME		= 0x11, /**< The buddy's signon time changed.     */
	PURPLE_BUDDY_EVIL				= 0x12,  /**< The buddy's warning level changed.     */
	PURPLE_BUDDY_DIRECTIM_CONNECTED = 0x14, /**< Connected to the buddy via DirectIM.  */
	PURPLE_BUDDY_DIRECTIM_DISCONNECTED = 0x18, /**< Disconnected from the buddy via DirectIM.  */
	PURPLE_BUDDY_NAME				= 0x20 /**<Buddy name (UID) changed. */
	
} PurpleBuddyEvent;
