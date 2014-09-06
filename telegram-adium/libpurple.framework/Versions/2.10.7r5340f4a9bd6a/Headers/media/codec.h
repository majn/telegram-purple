/**
 * @file codec.h Codec for Media API
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

#ifndef _PURPLE_MEDIA_CODEC_H_
#define _PURPLE_MEDIA_CODEC_H_

#include "enum-types.h"

/** An opaque structure representing an audio or video codec. */
typedef struct _PurpleMediaCodec PurpleMediaCodec;

#include "../util.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define PURPLE_TYPE_MEDIA_CODEC            (purple_media_codec_get_type())
#define PURPLE_IS_MEDIA_CODEC(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), PURPLE_TYPE_MEDIA_CODEC))
#define PURPLE_IS_MEDIA_CODEC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PURPLE_TYPE_MEDIA_CODEC))
#define PURPLE_MEDIA_CODEC(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), PURPLE_TYPE_MEDIA_CODEC, PurpleMediaCodec))
#define PURPLE_MEDIA_CODEC_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), PURPLE_TYPE_MEDIA_CODEC, PurpleMediaCodec))
#define PURPLE_MEDIA_CODEC_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PURPLE_TYPE_MEDIA_CODEC, PurpleMediaCodec))


/**
 * Gets the type of the media codec structure.
 *
 * @return The media codec's GType
 *
 * @since 2.6.0
 */
GType purple_media_codec_get_type(void);

/**
 * Creates a new PurpleMediaCodec instance.
 *
 * @param id Codec identifier.
 * @param encoding_name Name of the media type this encodes.
 * @param media_type PurpleMediaSessionType of this codec.
 * @param clock_rate The clock rate this codec encodes at, if applicable.
 *
 * @return The newly created PurpleMediaCodec.
 *
 * @since 2.6.0
 */
PurpleMediaCodec *purple_media_codec_new(int id, const char *encoding_name,
		PurpleMediaSessionType media_type, guint clock_rate);

/**
 * Gets the codec id.
 *
 * @param The codec to get the id from.
 *
 * @return The codec id.
 *
 * @since 2.6.0
 */
guint purple_media_codec_get_id(PurpleMediaCodec *codec);

/**
 * Gets the encoding name.
 *
 * @param The codec to get the encoding name from.
 *
 * @return The encoding name.
 *
 * @since 2.6.0
 */
gchar *purple_media_codec_get_encoding_name(PurpleMediaCodec *codec);

/**
 * Gets the clock rate.
 *
 * @param The codec to get the clock rate from.
 *
 * @return The clock rate.
 *
 * @since 2.6.0
 */
guint purple_media_codec_get_clock_rate(PurpleMediaCodec *codec);

/**
 * Gets the number of channels.
 *
 * @param The codec to get the number of channels from.
 *
 * @return The number of channels.
 *
 * @since 2.6.0
 */
guint purple_media_codec_get_channels(PurpleMediaCodec *codec);

/**
 * Gets a list of the optional parameters.
 *
 * The list consists of PurpleKeyValuePair's.
 *
 * @param The codec to get the optional parameters from.
 *
 * @return The list of optional parameters. The list is owned by the codec and
 *         should not be freed.
 *
 * @since 2.6.0
 */
GList *purple_media_codec_get_optional_parameters(PurpleMediaCodec *codec);

/**
 * Adds an optional parameter to the codec.
 *
 * @param codec The codec to add the parameter to.
 * @param name The name of the parameter to add.
 * @param value The value of the parameter to add.
 *
 * @since 2.6.0
 */
void purple_media_codec_add_optional_parameter(PurpleMediaCodec *codec,
		const gchar *name, const gchar *value);

/**
 * Removes an optional parameter from the codec.
 *
 * @param codec The codec to remove the parameter from.
 * @param param A pointer to the parameter to remove.
 *
 * @since 2.6.0
 */
void purple_media_codec_remove_optional_parameter(PurpleMediaCodec *codec,
		PurpleKeyValuePair *param);

/**
 * Gets an optional parameter based on the values given.
 *
 * @param codec The codec to find the parameter in.
 * @param name The name of the parameter to search for.
 * @param value The value to search for or NULL.
 *
 * @return The value found or NULL.
 *
 * @since 2.6.0
 */
PurpleKeyValuePair *purple_media_codec_get_optional_parameter(
		PurpleMediaCodec *codec, const gchar *name,
		const gchar *value);

/**
 * Copies a PurpleMediaCodec object.
 *
 * @param codec The codec to copy.
 *
 * @return The copy of the codec.
 *
 * @since 2.7.0
 */
PurpleMediaCodec *purple_media_codec_copy(PurpleMediaCodec *codec);

/**
 * Copies a GList of PurpleMediaCodec and its contents.
 *
 * @param codecs The list of codecs to be copied.
 *
 * @return The copy of the GList.
 *
 * @since 2.6.0
 */
GList *purple_media_codec_list_copy(GList *codecs);

/**
 * Frees a GList of PurpleMediaCodec and its contents.
 *
 * @param codecs The list of codecs to be freed.
 *
 * @since 2.6.0
 */
void purple_media_codec_list_free(GList *codecs);

/**
 * Creates a string representation of the codec.
 *
 * @param codec The codec to create the string of.
 *
 * @return The new string representation.
 *
 * @since 2.6.0
 */
gchar *purple_media_codec_to_string(const PurpleMediaCodec *codec);

G_END_DECLS

#endif  /* _PURPLE_MEDIA_CODEC_H_ */

