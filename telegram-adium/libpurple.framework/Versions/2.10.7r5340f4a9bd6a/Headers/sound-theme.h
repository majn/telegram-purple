/**
 * @file sound-theme.h  Purple Sound Theme Abstact Class API
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

#ifndef PURPLE_SOUND_THEME_H
#define PURPLE_SOUND_THEME_H

#include <glib.h>
#include <glib-object.h>
#include "theme.h"
#include "sound.h"

/**
 * extends PurpleTheme (theme.h)
 * A purple sound theme.
 * This is an object for Purple to represent a sound theme.
 *
 * PurpleSoundTheme is a PurpleTheme Object.
 */
typedef struct _PurpleSoundTheme        PurpleSoundTheme;
typedef struct _PurpleSoundThemeClass   PurpleSoundThemeClass;

#define PURPLE_TYPE_SOUND_THEME             (purple_sound_theme_get_type())
#define PURPLE_SOUND_THEME(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), PURPLE_TYPE_SOUND_THEME, PurpleSoundTheme))
#define PURPLE_SOUND_THEME_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), PURPLE_TYPE_SOUND_THEME, PurpleSoundThemeClass))
#define PURPLE_IS_SOUND_THEME(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), PURPLE_TYPE_SOUND_THEME))
#define PURPLE_IS_SOUND_THEME_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), PURPLE_TYPE_SOUND_THEME))
#define PURPLE_SOUND_THEME_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), PURPLE_TYPE_SOUND_THEME, PurpleSoundThemeClass))

struct _PurpleSoundTheme
{
	PurpleTheme parent;
	gpointer priv;
};

struct _PurpleSoundThemeClass
{
	PurpleThemeClass parent_class;
};

/**************************************************************************/
/** @name Purple Sound Theme API                                          */
/**************************************************************************/
G_BEGIN_DECLS

/**
 * GObject foo.
 * @internal.
 */
GType purple_sound_theme_get_type(void);

/**
 * Returns a copy of the filename for the sound event.
 *
 * @param theme The theme.
 * @param event The purple sound event to look up.
 *
 * @returns The filename of the sound event.
 */
const gchar *purple_sound_theme_get_file(PurpleSoundTheme *theme,
		const gchar *event);

/**
 * Returns a copy of the directory and filename for the sound event
 *
 * @param theme The theme.
 * @param event The purple sound event to look up
 *
 * @returns The directory + '/' + filename of the sound event.  This is
 *          a newly allocated string that should be freed with g_free.
 */
gchar *purple_sound_theme_get_file_full(PurpleSoundTheme *theme,
		const gchar *event);

/**
 * Sets the filename for a given sound event
 *
 * @param theme    The theme.
 * @param event    the purple sound event to look up
 * @param filename the name of the file to be used for the event
 */
void purple_sound_theme_set_file(PurpleSoundTheme *theme,
		const gchar *event,
		const gchar *filename);

G_END_DECLS
#endif /* PURPLE_SOUND_THEME_H */
