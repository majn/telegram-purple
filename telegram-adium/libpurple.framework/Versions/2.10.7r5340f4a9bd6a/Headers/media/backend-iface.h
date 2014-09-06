/**
 * @file backend-iface.h Interface for media backends
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

#ifndef _MEDIA_BACKEND_IFACE_H_
#define _MEDIA_BACKEND_IFACE_H_

#include "codec.h"
#include "enum-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define PURPLE_TYPE_MEDIA_BACKEND		(purple_media_backend_get_type())
#define PURPLE_IS_MEDIA_BACKEND(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), PURPLE_TYPE_MEDIA_BACKEND))
#define PURPLE_MEDIA_BACKEND(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), PURPLE_TYPE_MEDIA_BACKEND, PurpleMediaBackend))
#define PURPLE_MEDIA_BACKEND_GET_INTERFACE(inst)(G_TYPE_INSTANCE_GET_INTERFACE((inst), PURPLE_TYPE_MEDIA_BACKEND, PurpleMediaBackendIface))

/** A placeholder to represent any media backend */
typedef struct _PurpleMediaBackend PurpleMediaBackend;
/** A structure to derive media backends from. */
typedef struct _PurpleMediaBackendIface PurpleMediaBackendIface;

struct _PurpleMediaBackendIface
{
	GTypeInterface parent_iface; /**< The parent iface class */

	/** Implementable functions called with purple_media_backend_* */
	gboolean (*add_stream) (PurpleMediaBackend *self,
		const gchar *sess_id, const gchar *who,
		PurpleMediaSessionType type, gboolean initiator,
		const gchar *transmitter,
		guint num_params, GParameter *params);
	void (*add_remote_candidates) (PurpleMediaBackend *self,
		const gchar *sess_id, const gchar *participant,
		GList *remote_candidates);
	gboolean (*codecs_ready) (PurpleMediaBackend *self,
		const gchar *sess_id);
	GList *(*get_codecs) (PurpleMediaBackend *self,
		const gchar *sess_id);
	GList *(*get_local_candidates) (PurpleMediaBackend *self,
		const gchar *sess_id, const gchar *participant);
	gboolean (*set_remote_codecs) (PurpleMediaBackend *self,
		const gchar *sess_id, const gchar *participant,
		GList *codecs);
	gboolean (*set_send_codec) (PurpleMediaBackend *self,
		const gchar *sess_id, PurpleMediaCodec *codec);
	void (*set_params) (PurpleMediaBackend *self,
		guint num_params, GParameter *params);
	const gchar **(*get_available_params) (void);
};

/**
 * Gets the media backend's GType.
 *
 * @return The media backend's GType.
 *
 * @since 2.7.0
 */
GType purple_media_backend_get_type(void);

/**
 * Creates and adds a stream to the media backend.
 *
 * @param self The backend to add the stream to.
 * @param sess_id The session id of the stream to add.
 * @param who The remote participant of the stream to add.
 * @param type The media type and direction of the stream to add.
 * @param initiator True if the local user initiated the stream.
 * @param transmitter The string id of the tranmsitter to use.
 * @param num_params The number of parameters in the param parameter.
 * @param params The additional parameters to pass when creating the stream.
 *
 * @return True if the stream was successfully created, othewise False.
 *
 * @since 2.7.0
 */
gboolean purple_media_backend_add_stream(PurpleMediaBackend *self,
		const gchar *sess_id, const gchar *who,
		PurpleMediaSessionType type, gboolean initiator,
		const gchar *transmitter,
		guint num_params, GParameter *params);

/**
 * Add remote candidates to a stream.
 *
 * @param self The backend the stream is in.
 * @param sess_id The session id associated with the stream.
 * @param participant The participant associated with the stream.
 * @param remote_candidates The list of remote candidates to add.
 *
 * @since 2.7.0
 */
void purple_media_backend_add_remote_candidates(PurpleMediaBackend *self,
		const gchar *sess_id, const gchar *participant,
		GList *remote_candidates);

/**
 * Get whether or not a session's codecs are ready.
 *
 * A codec is ready if all of the attributes and additional
 * parameters have been collected.
 *
 * @param self The media backend the session is in.
 * @param sess_id The session id of the session to check.
 *
 * @return True if the codecs are ready, otherwise False.
 *
 * @since 2.7.0
 */
gboolean purple_media_backend_codecs_ready(PurpleMediaBackend *self,
		const gchar *sess_id);

/**
 * Gets the codec intersection list for a session.
 *
 * The intersection list consists of all codecs that are compatible
 * between the local and remote software.
 *
 * @param self The media backend the session is in.
 * @param sess_id The session id of the session to use.
 *
 * @return The codec intersection list.
 *
 * @since 2.7.0
 */
GList *purple_media_backend_get_codecs(PurpleMediaBackend *self,
		const gchar *sess_id);

/**
 * Gets the list of local candidates for a stream.
 *
 * @param self The media backend the stream is in.
 * @param sess_id The session id associated with the stream.
 * @param particilant The participant associated with the stream.
 *
 * @return The list of local candidates.
 *
 * @since 2.7.0
 */
GList *purple_media_backend_get_local_candidates(PurpleMediaBackend *self,
		const gchar *sess_id, const gchar *participant);

/**
 * Sets the remote codecs on a stream.
 *
 * @param self The media backend the stream is in.
 * @param sess_id The session id the stream is associated with.
 * @param participant The participant the stream is associated with.
 * @param codecs The list of remote codecs to set.
 *
 * @return True if the remote codecs were set successfully, otherwise False.
 *
 * @since 2.7.0
 */
gboolean purple_media_backend_set_remote_codecs(PurpleMediaBackend *self,
		const gchar *sess_id, const gchar *participant,
		GList *codecs);

/**
 * Sets which codec format to send media content in for a session.
 *
 * @param self The media backend the session is in.
 * @param sess_id The session id of the session to set the codec for.
 * @param codec The codec to set.
 *
 * @return True if set successfully, otherwise False.
 *
 * @since 2.7.0
 */
gboolean purple_media_backend_set_send_codec(PurpleMediaBackend *self,
		const gchar *sess_id, PurpleMediaCodec *codec);

/**
 * Sets various optional parameters of the media backend.
 *
 * @param self The media backend to set the parameters on.
 * @param num_params The number of parameters to pass to backend
 * @param params Array of @c GParameter to pass to backend
 *
 * @since 2.8.0
 */
void purple_media_backend_set_params(PurpleMediaBackend *self,
		guint num_params, GParameter *params);

/**
 * Gets the list of optional parameters supported by the media backend.
 *
 * The list should NOT be freed.
 *
 * @param self The media backend
 *
 * @return NULL-terminated array of names of supported parameters.
 *
 * @since 2.8.0
 */
const gchar **purple_media_backend_get_available_params(PurpleMediaBackend *self);

G_END_DECLS

#endif /* _MEDIA_BACKEND_IFACE_H_ */
