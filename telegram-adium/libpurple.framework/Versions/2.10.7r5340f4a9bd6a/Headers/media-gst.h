/**
 * @file media-gst.h Media API
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

#ifndef _PURPLE_MEDIA_GST_H_
#define _PURPLE_MEDIA_GST_H_

#include "media.h"
#include "mediamanager.h"

#include <gst/gst.h>

G_BEGIN_DECLS

#define PURPLE_TYPE_MEDIA_ELEMENT_TYPE           (purple_media_element_type_get_type())
#define PURPLE_TYPE_MEDIA_ELEMENT_INFO           (purple_media_element_info_get_type())
#define PURPLE_MEDIA_ELEMENT_INFO(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), PURPLE_TYPE_MEDIA_ELEMENT_INFO, PurpleMediaElementInfo))
#define PURPLE_MEDIA_ELEMENT_INFO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), PURPLE_TYPE_MEDIA_ELEMENT_INFO, PurpleMediaElementInfo))
#define PURPLE_IS_MEDIA_ELEMENT_INFO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), PURPLE_TYPE_MEDIA_ELEMENT_INFO))
#define PURPLE_IS_MEDIA_ELEMENT_INFO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PURPLE_TYPE_MEDIA_ELEMENT_INFO))
#define PURPLE_MEDIA_ELEMENT_INFO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PURPLE_TYPE_MEDIA_ELEMENT_INFO, PurpleMediaElementInfo))

/** An opaque structure representing an audio/video source/sink. */
typedef struct _PurpleMediaElementInfo PurpleMediaElementInfo;
typedef struct _PurpleMediaElementInfoClass PurpleMediaElementInfoClass;
typedef GstElement *(*PurpleMediaElementCreateCallback)(PurpleMedia *media,
			const gchar *session_id, const gchar *participant);

typedef enum {
	PURPLE_MEDIA_ELEMENT_NONE = 0,			/** empty element */
	PURPLE_MEDIA_ELEMENT_AUDIO = 1,			/** supports audio */
	PURPLE_MEDIA_ELEMENT_VIDEO = 1 << 1,		/** supports video */
	PURPLE_MEDIA_ELEMENT_AUDIO_VIDEO = PURPLE_MEDIA_ELEMENT_AUDIO
			| PURPLE_MEDIA_ELEMENT_VIDEO, 	/** supports audio and video */

	PURPLE_MEDIA_ELEMENT_NO_SRCS = 0,		/** has no src pads */
	PURPLE_MEDIA_ELEMENT_ONE_SRC = 1 << 2,		/** has one src pad */
	PURPLE_MEDIA_ELEMENT_MULTI_SRC = 1 << 3, 	/** has multiple src pads */
	PURPLE_MEDIA_ELEMENT_REQUEST_SRC = 1 << 4, 	/** src pads must be requested */

	PURPLE_MEDIA_ELEMENT_NO_SINKS = 0,		/** has no sink pads */
	PURPLE_MEDIA_ELEMENT_ONE_SINK = 1 << 5, 	/** has one sink pad */
	PURPLE_MEDIA_ELEMENT_MULTI_SINK = 1 << 6, 	/** has multiple sink pads */
	PURPLE_MEDIA_ELEMENT_REQUEST_SINK = 1 << 7, 	/** sink pads must be requested */

	PURPLE_MEDIA_ELEMENT_UNIQUE = 1 << 8,		/** This element is unique and
							 only one instance of it should
							 be created at a time */

	PURPLE_MEDIA_ELEMENT_SRC = 1 << 9,		/** can be set as an active src */
	PURPLE_MEDIA_ELEMENT_SINK = 1 << 10,		/** can be set as an active sink */
} PurpleMediaElementType;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets the element type's GType.
 *
 * @return The element type's GType.
 *
 * @since 2.6.0
 */
GType purple_media_element_type_get_type(void);

/**
 * Gets the element info's GType.
 *
 * @return The element info's GType.
 *
 * @since 2.6.0
 */
GType purple_media_element_info_get_type(void);

/**
 * Gets the source from a session
 *
 * @param media The media object the session is in.
 * @param sess_id The session id of the session to get the source from.
 *
 * @return The source retrieved.
 *
 * @since 2.6.0
 */
GstElement *purple_media_get_src(PurpleMedia *media, const gchar *sess_id);

/**
 * Gets the tee from a given session/stream.
 *
 * @param media The instance to get the tee from.
 * @param session_id The id of the session to get the tee from.
 * @param participant Optionally, the participant of the stream to get the tee from.
 *
 * @return The GstTee element from the chosen session/stream.
 *
 * @since 2.6.0
 */
GstElement *purple_media_get_tee(PurpleMedia *media,
		const gchar *session_id, const gchar *participant);


/**
 * Gets the pipeline from the media manager.
 *
 * @param manager The media manager to get the pipeline from.
 *
 * @return The pipeline.
 *
 * @since 2.6.0
 */
GstElement *purple_media_manager_get_pipeline(PurpleMediaManager *manager);

/**
 * Returns a GStreamer source or sink for audio or video.
 *
 * @param manager The media manager to use to obtain the source/sink.
 * @param type The type of source/sink to get.
 * @param media The media call this element is requested for.
 * @param session_id The id of the session this element is requested for or NULL.
 * @param participant The remote user this element is requested for or NULL.
 *
 * @since 2.6.0
 */
GstElement *purple_media_manager_get_element(PurpleMediaManager *manager,
		PurpleMediaSessionType type, PurpleMedia *media,
		const gchar *session_id, const gchar *participant);

PurpleMediaElementInfo *purple_media_manager_get_element_info(
		PurpleMediaManager *manager, const gchar *name);
gboolean purple_media_manager_register_element(PurpleMediaManager *manager,
		PurpleMediaElementInfo *info);
gboolean purple_media_manager_unregister_element(PurpleMediaManager *manager,
		const gchar *name);
gboolean purple_media_manager_set_active_element(PurpleMediaManager *manager,
		PurpleMediaElementInfo *info);
PurpleMediaElementInfo *purple_media_manager_get_active_element(
		PurpleMediaManager *manager, PurpleMediaElementType type);

/**
 * Reduces media formats supported by the video source to given set.
 *
 * Useful to force negotiation of smaller picture resolution more suitable for
 * use with particular codec and communication protocol without rescaling.
 *
 * @param manager The media manager to set the media formats.
 * @param caps Set of allowed media formats.
 *
 * @since 2.8.0
 */
void purple_media_manager_set_video_caps(PurpleMediaManager *manager,
		GstCaps *caps);

/**
 * Returns current set of media formats limiting the output from video source.
 *
 * @param manager The media manager to get the media formats from.
 *
 * @return @c GstCaps limiting the video source's formats.
 *
 * @since 2.8.0
 */
GstCaps *purple_media_manager_get_video_caps(PurpleMediaManager *manager);

gchar *purple_media_element_info_get_id(PurpleMediaElementInfo *info);
gchar *purple_media_element_info_get_name(PurpleMediaElementInfo *info);
PurpleMediaElementType purple_media_element_info_get_element_type(
		PurpleMediaElementInfo *info);
GstElement *purple_media_element_info_call_create(
		PurpleMediaElementInfo *info, PurpleMedia *media,
		const gchar *session_id, const gchar *participant);

#ifdef __cplusplus
}
#endif

G_END_DECLS

#endif  /* _PURPLE_MEDIA_GST_H_ */
