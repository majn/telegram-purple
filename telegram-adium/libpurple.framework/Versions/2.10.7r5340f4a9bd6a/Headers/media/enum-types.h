/**
 * @file enum-types.h Enum types for Media API
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

#ifndef _PURPLE_MEDIA_ENUM_TYPES_H_
#define _PURPLE_MEDIA_ENUM_TYPES_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define PURPLE_TYPE_MEDIA_CANDIDATE_TYPE   (purple_media_candidate_type_get_type())
#define PURPLE_MEDIA_TYPE_CAPS	           (purple_media_caps_get_type())
#define PURPLE_MEDIA_TYPE_INFO_TYPE	   (purple_media_info_type_get_type())
#define PURPLE_TYPE_MEDIA_NETWORK_PROTOCOL (purple_media_network_protocol_get_type())
#define PURPLE_TYPE_MEDIA_SESSION_TYPE     (purple_media_session_type_get_type())
#define PURPLE_MEDIA_TYPE_STATE            (purple_media_state_changed_get_type())

/** Media candidate types */
typedef enum {
	PURPLE_MEDIA_CANDIDATE_TYPE_HOST,
	PURPLE_MEDIA_CANDIDATE_TYPE_SRFLX,
	PURPLE_MEDIA_CANDIDATE_TYPE_PRFLX,
	PURPLE_MEDIA_CANDIDATE_TYPE_RELAY,
	PURPLE_MEDIA_CANDIDATE_TYPE_MULTICAST
} PurpleMediaCandidateType;

/** Media caps */
typedef enum {
	PURPLE_MEDIA_CAPS_NONE = 0,
	PURPLE_MEDIA_CAPS_AUDIO = 1,
	PURPLE_MEDIA_CAPS_AUDIO_SINGLE_DIRECTION = 1 << 1,
	PURPLE_MEDIA_CAPS_VIDEO = 1 << 2,
	PURPLE_MEDIA_CAPS_VIDEO_SINGLE_DIRECTION = 1 << 3,
	PURPLE_MEDIA_CAPS_AUDIO_VIDEO = 1 << 4,
	PURPLE_MEDIA_CAPS_MODIFY_SESSION = 1 << 5,
	PURPLE_MEDIA_CAPS_CHANGE_DIRECTION = 1 << 6
} PurpleMediaCaps;

/** Media component types */
typedef enum {
	PURPLE_MEDIA_COMPONENT_NONE = 0,
	PURPLE_MEDIA_COMPONENT_RTP = 1,
	PURPLE_MEDIA_COMPONENT_RTCP = 2
} PurpleMediaComponentType;

/** Media info types */
typedef enum {
	PURPLE_MEDIA_INFO_HANGUP = 0,
	PURPLE_MEDIA_INFO_ACCEPT,
	PURPLE_MEDIA_INFO_REJECT,
	PURPLE_MEDIA_INFO_MUTE,
	PURPLE_MEDIA_INFO_UNMUTE,
	PURPLE_MEDIA_INFO_PAUSE,
	PURPLE_MEDIA_INFO_UNPAUSE,
	PURPLE_MEDIA_INFO_HOLD,
	PURPLE_MEDIA_INFO_UNHOLD
} PurpleMediaInfoType;

/** Media network protocols */
typedef enum {
	PURPLE_MEDIA_NETWORK_PROTOCOL_UDP,
	PURPLE_MEDIA_NETWORK_PROTOCOL_TCP
} PurpleMediaNetworkProtocol;

/** Media session types */
typedef enum {
	PURPLE_MEDIA_NONE	= 0,
	PURPLE_MEDIA_RECV_AUDIO = 1 << 0,
	PURPLE_MEDIA_SEND_AUDIO = 1 << 1,
	PURPLE_MEDIA_RECV_VIDEO = 1 << 2,
	PURPLE_MEDIA_SEND_VIDEO = 1 << 3,
	PURPLE_MEDIA_AUDIO = PURPLE_MEDIA_RECV_AUDIO | PURPLE_MEDIA_SEND_AUDIO,
	PURPLE_MEDIA_VIDEO = PURPLE_MEDIA_RECV_VIDEO | PURPLE_MEDIA_SEND_VIDEO
} PurpleMediaSessionType;

/** Media state-changed types */
typedef enum {
	PURPLE_MEDIA_STATE_NEW = 0,
	PURPLE_MEDIA_STATE_CONNECTED,
	PURPLE_MEDIA_STATE_END
} PurpleMediaState;

/**
 * Gets the media candidate type's GType
 *
 * @return The media candidate type's GType.
 *
 * @since 2.6.0
 */
GType purple_media_candidate_type_get_type(void);

/**
 * Gets the type of the media caps flags
 *
 * @return The media caps flags' GType
 *
 * @since 2.7.0
 */
GType purple_media_caps_get_type(void);

/**
 * Gets the type of the info type enum
 *
 * @return The info type enum's GType
 *
 * @since 2.6.0
 */
GType purple_media_info_type_get_type(void);

/**
 * Gets the media network protocol's GType
 *
 * @return The media network protocol's GType.
 *
 * @since 2.6.0
 */
GType purple_media_network_protocol_get_type(void);

/**
 * Gets the media session type's GType
 *
 * @return The media session type's GType.
 *
 * @since 2.6.0
 */
GType purple_media_session_type_get_type(void);

/**
 * Gets the type of the state-changed enum
 *
 * @return The state-changed enum's GType
 *
 * @since 2.6.0
 */
GType purple_media_state_changed_get_type(void);

G_END_DECLS

#endif  /* _PURPLE_MEDIA_ENUM_TYPES_ */
