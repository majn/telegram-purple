/**
 * @file pluginpref.h Plugin Preferences API
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
 *
 */
#ifndef _PURPLE_PLUGINPREF_H_
#define _PURPLE_PLUGINPREF_H_

typedef struct _PurplePluginPrefFrame		PurplePluginPrefFrame;
typedef struct _PurplePluginPref			PurplePluginPref;

/**
 * String format for preferences.
 */
typedef enum
{
	PURPLE_STRING_FORMAT_TYPE_NONE      = 0,          /**< The string is plain text. */
	PURPLE_STRING_FORMAT_TYPE_MULTILINE = 1 << 0,     /**< The string can have newlines. */
	PURPLE_STRING_FORMAT_TYPE_HTML      = 1 << 1      /**< The string can be in HTML. */
} PurpleStringFormatType;

typedef enum {
	PURPLE_PLUGIN_PREF_NONE,
	PURPLE_PLUGIN_PREF_CHOICE,
	PURPLE_PLUGIN_PREF_INFO,              /**< no-value label */
	PURPLE_PLUGIN_PREF_STRING_FORMAT      /**< The preference has a string value. */
} PurplePluginPrefType;

#include <glib.h>
#include "prefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Plugin Preference API                                           */
/**************************************************************************/
/*@{*/

/**
 * Create a new plugin preference frame
 *
 * @return a new PurplePluginPrefFrame
 */
PurplePluginPrefFrame *purple_plugin_pref_frame_new(void);

/**
 * Destroy a plugin preference frame
 *
 * @param frame The plugin frame to destroy
 */
void purple_plugin_pref_frame_destroy(PurplePluginPrefFrame *frame);

/**
 * Adds a plugin preference to a plugin preference frame
 *
 * @param frame The plugin frame to add the preference to
 * @param pref  The preference to add to the frame
 */
void purple_plugin_pref_frame_add(PurplePluginPrefFrame *frame, PurplePluginPref *pref);

/**
 * Get the plugin preferences from a plugin preference frame
 *
 * @param frame The plugin frame to get the plugin preferences from
 * @constreturn a GList of plugin preferences
 */
GList *purple_plugin_pref_frame_get_prefs(PurplePluginPrefFrame *frame);

/**
 * Create a new plugin preference
 *
 * @return a new PurplePluginPref
 */
PurplePluginPref *purple_plugin_pref_new(void);

/**
 * Create a new plugin preference with name
 *
 * @param name The name of the pref
 * @return a new PurplePluginPref
 */
PurplePluginPref *purple_plugin_pref_new_with_name(const char *name);

/**
 * Create a new plugin preference with label
 *
 * @param label The label to be displayed
 * @return a new PurplePluginPref
 */
PurplePluginPref *purple_plugin_pref_new_with_label(const char *label);

/**
 * Create a new plugin preference with name and label
 *
 * @param name  The name of the pref
 * @param label The label to be displayed
 * @return a new PurplePluginPref
 */
PurplePluginPref *purple_plugin_pref_new_with_name_and_label(const char *name, const char *label);

/**
 * Destroy a plugin preference
 *
 * @param pref The preference to destroy
 */
void purple_plugin_pref_destroy(PurplePluginPref *pref);

/**
 * Set a plugin pref name
 *
 * @param pref The plugin pref
 * @param name The name of the pref
 */
void purple_plugin_pref_set_name(PurplePluginPref *pref, const char *name);

/**
 * Get a plugin pref name
 *
 * @param pref The plugin pref
 * @return The name of the pref
 */
const char *purple_plugin_pref_get_name(PurplePluginPref *pref);

/**
 * Set a plugin pref label
 *
 * @param pref  The plugin pref
 * @param label The label for the plugin pref
 */
void purple_plugin_pref_set_label(PurplePluginPref *pref, const char *label);

/**
 * Get a plugin pref label
 *
 * @param pref The plugin pref
 * @return The label for the plugin pref
 */
const char *purple_plugin_pref_get_label(PurplePluginPref *pref);

/**
 * Set the bounds for an integer pref
 *
 * @param pref The plugin pref
 * @param min  The min value
 * @param max  The max value
 */
void purple_plugin_pref_set_bounds(PurplePluginPref *pref, int min, int max);

/**
 * Get the bounds for an integer pref
 *
 * @param pref The plugin pref
 * @param min  The min value
 * @param max  The max value
 */
void purple_plugin_pref_get_bounds(PurplePluginPref *pref, int *min, int *max);

/**
 * Set the type of a plugin pref
 *
 * @param pref The plugin pref
 * @param type The type
 */
void purple_plugin_pref_set_type(PurplePluginPref *pref, PurplePluginPrefType type);

/**
 * Get the type of a plugin pref
 *
 * @param pref The plugin pref
 * @return The type
 */
PurplePluginPrefType purple_plugin_pref_get_type(PurplePluginPref *pref);

/**
 * Set the choices for a choices plugin pref
 *
 * @param pref  The plugin pref
 * @param label The label for the choice
 * @param choice  A gpointer of the choice
 */
void purple_plugin_pref_add_choice(PurplePluginPref *pref, const char *label, gpointer choice);

/**
 * Get the choices for a choices plugin pref
 *
 * @param pref The plugin pref
 * @constreturn GList of the choices
 */
GList *purple_plugin_pref_get_choices(PurplePluginPref *pref);

/**
 * Set the max length for a string plugin pref
 *
 * @param pref       The plugin pref
 * @param max_length The max length of the string
 */
void purple_plugin_pref_set_max_length(PurplePluginPref *pref, unsigned int max_length);

/**
 * Get the max length for a string plugin pref
 *
 * @param pref The plugin pref
 * @return the max length
 */
unsigned int purple_plugin_pref_get_max_length(PurplePluginPref *pref);

/**
 * Sets the masking of a string plugin pref
 *
 * @param pref The plugin pref
 * @param mask The value to set
 */
void purple_plugin_pref_set_masked(PurplePluginPref *pref, gboolean mask);

/**
 * Gets the masking of a string plugin pref
 *
 * @param pref The plugin pref
 * @return The masking
 */
gboolean purple_plugin_pref_get_masked(PurplePluginPref *pref);

/**
 * Sets the format type for a formattable-string plugin pref. You need to set the
 * pref type to PURPLE_PLUGIN_PREF_STRING_FORMAT first before setting the format.
 *
 * @param pref	 The plugin pref
 * @param format The format of the string
 */
void purple_plugin_pref_set_format_type(PurplePluginPref *pref, PurpleStringFormatType format);

/**
 * Gets the format type of the formattable-string plugin pref.
 *
 * @param pref The plugin pref
 * @return The format of the pref
 */
PurpleStringFormatType purple_plugin_pref_get_format_type(PurplePluginPref *pref);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_PLUGINPREF_H_ */
