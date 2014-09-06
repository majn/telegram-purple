/**
 * @file sound-theme-loader.h  Purple Sound Theme Loader Class API
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

#ifndef PURPLE_SOUND_THEME_LOADER_H
#define PURPLE_SOUND_THEME_LOADER_H

#include <glib.h>
#include <glib-object.h>
#include "theme-loader.h"

/**
 * A purple sound theme loader. extends PurpleThemeLoader (theme-loader.h)
 * This is a class designed to build sound themes
 *
 * PurpleSoundThemeLoader is a GObject.
 */
typedef struct _PurpleSoundThemeLoader        PurpleSoundThemeLoader;
typedef struct _PurpleSoundThemeLoaderClass   PurpleSoundThemeLoaderClass;

#define PURPLE_TYPE_SOUND_THEME_LOADER            (purple_sound_theme_loader_get_type())
#define PURPLE_SOUND_THEME_LOADER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), PURPLE_TYPE_SOUND_THEME_LOADER, PurpleSoundThemeLoader))
#define PURPLE_SOUND_THEME_LOADER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), PURPLE_TYPE_SOUND_THEME_LOADER, PurpleSoundThemeLoaderClass))
#define PURPLE_IS_SOUND_THEME_LOADER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), PURPLE_TYPE_SOUND_THEME_LOADER))
#define PURPLE_IS_SOUND_THEME_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PURPLE_TYPE_SOUND_THEME_LOADER))
#define PURPLE_SOUND_THEME_LOADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PURPLE_TYPE_SOUND_THEME_LOADER, PurpleSoundThemeLoaderClass))

struct _PurpleSoundThemeLoader
{
	PurpleThemeLoader parent;
};

struct _PurpleSoundThemeLoaderClass
{
	PurpleThemeLoaderClass parent_class;
};

/**************************************************************************/
/** @name Purple Theme-Loader API                                         */
/**************************************************************************/
G_BEGIN_DECLS

/**
 * GObject foo.
 * @internal.
 */
GType purple_sound_theme_loader_get_type(void);

G_END_DECLS
#endif /* PURPLE_SOUND_THEME_LOADER_H */
