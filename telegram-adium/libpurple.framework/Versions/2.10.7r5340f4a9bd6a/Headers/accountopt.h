/**
 * @file accountopt.h Account Options API
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
#ifndef _PURPLE_ACCOUNTOPT_H_
#define _PURPLE_ACCOUNTOPT_H_

#include "prefs.h"

/**
 * An option for an account.
 *
 * This is set by protocol plugins, and appears in the account settings
 * dialogs.
 */
typedef struct
{
	PurplePrefType type;      /**< The type of value.                     */

	char *text;             /**< The text that will appear to the user. */
	char *pref_name;        /**< The name of the associated preference. */

	union
	{
		gboolean boolean;   /**< The default boolean value.             */
		int integer;        /**< The default integer value.             */
		char *string;       /**< The default string value.              */
		GList *list;        /**< The default list value.                */

	} default_value;

	gboolean masked;        /**< Whether the value entered should be
	                         *   obscured from view (for passwords and
	                         *   similar options)
	                         */
} PurpleAccountOption;

/**
 * A username split.
 *
 * This is used by some protocols to separate the fields of the username
 * into more human-readable components.
 */
typedef struct
{
	char *text;             /**< The text that will appear to the user. */
	char *default_value;    /**< The default value.                     */
	char  field_sep;        /**< The field separator.                   */
	gboolean reverse;       /**< TRUE if the separator should be found
							  starting a the end of the string, FALSE
							  otherwise                                 */

} PurpleAccountUserSplit;

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Account Option API                                              */
/**************************************************************************/
/*@{*/

/**
 * Creates a new account option.  If you know what @a type will be in advance,
 * consider using purple_account_option_bool_new(),
 * purple_account_option_int_new(), purple_account_option_string_new() or
 * purple_account_option_list_new() (as appropriate) instead.
 *
 * @param type      The type of option.
 * @param text      The text of the option.
 * @param pref_name The account preference name for the option.
 *
 * @return The account option.
 */
PurpleAccountOption *purple_account_option_new(PurplePrefType type,
	const char *text, const char *pref_name);

/**
 * Creates a new boolean account option.
 *
 * @param text          The text of the option.
 * @param pref_name     The account preference name for the option.
 * @param default_value The default value.
 *
 * @return The account option.
 */
PurpleAccountOption *purple_account_option_bool_new(const char *text,
	const char *pref_name, gboolean default_value);

/**
 * Creates a new integer account option.
 *
 * @param text          The text of the option.
 * @param pref_name     The account preference name for the option.
 * @param default_value The default value.
 *
 * @return The account option.
 */
PurpleAccountOption *purple_account_option_int_new(const char *text,
	const char *pref_name, int default_value);

/**
 * Creates a new string account option.
 *
 * @param text          The text of the option.
 * @param pref_name     The account preference name for the option.
 * @param default_value The default value.
 *
 * @return The account option.
 */
PurpleAccountOption *purple_account_option_string_new(const char *text,
	const char *pref_name, const char *default_value);

/**
 * Creates a new list account option.
 *
 * The list passed will be owned by the account option, and the
 * strings inside will be freed automatically.
 *
 * The list is a list of #PurpleKeyValuePair items. The key is the label that
 * should be displayed to the user, and the <tt>(const char *)</tt> value is
 * the internal ID that should be passed to purple_account_set_string() to
 * choose that value.
 *
 * @param text      The text of the option.
 * @param pref_name The account preference name for the option.
 * @param list      The key, value list.
 *
 * @return The account option.
 */
PurpleAccountOption *purple_account_option_list_new(const char *text,
	const char *pref_name, GList *list);

/**
 * Destroys an account option.
 *
 * @param option The option to destroy.
 */
void purple_account_option_destroy(PurpleAccountOption *option);

/**
 * Sets the default boolean value for an account option.
 *
 * @param option The account option.
 * @param value  The default boolean value.
 */
void purple_account_option_set_default_bool(PurpleAccountOption *option,
										  gboolean value);

/**
 * Sets the default integer value for an account option.
 *
 * @param option The account option.
 * @param value  The default integer value.
 */
void purple_account_option_set_default_int(PurpleAccountOption *option,
										 int value);

/**
 * Sets the default string value for an account option.
 *
 * @param option The account option.
 * @param value  The default string value.
 */
void purple_account_option_set_default_string(PurpleAccountOption *option,
											const char *value);

/**
 * Sets the masking for an account option. Setting this to %TRUE acts
 * as a hint to the UI that the option's value should be obscured from
 * view, like a password.
 *
 * @param option The account option.
 * @param masked The masking.
 */
void
purple_account_option_set_masked(PurpleAccountOption *option, gboolean masked);

/**
 * Sets the list values for an account option.
 *
 * The list passed will be owned by the account option, and the
 * strings inside will be freed automatically.
 *
 * The list is in key, value pairs. The key is the ID stored and used
 * internally, and the value is the label displayed.
 *
 * @param option The account option.
 * @param values The default list value.
 */
void purple_account_option_set_list(PurpleAccountOption *option, GList *values);

/**
 * Adds an item to a list account option.
 *
 * @param option The account option.
 * @param key    The key.
 * @param value  The value.
 */
void purple_account_option_add_list_item(PurpleAccountOption *option,
									   const char *key, const char *value);

/**
 * Returns the specified account option's type.
 *
 * @param option The account option.
 *
 * @return The account option's type.
 */
PurplePrefType purple_account_option_get_type(const PurpleAccountOption *option);

/**
 * Returns the text for an account option.
 *
 * @param option The account option.
 *
 * @return The account option's text.
 */
const char *purple_account_option_get_text(const PurpleAccountOption *option);

/**
 * Returns the name of an account option.  This corresponds to the @c pref_name
 * parameter supplied to purple_account_option_new() or one of the
 * type-specific constructors.
 *
 * @param option The account option.
 *
 * @return The option's name.
 */
const char *purple_account_option_get_setting(const PurpleAccountOption *option);

/**
 * Returns the default boolean value for an account option.
 *
 * @param option The account option.
 *
 * @return The default boolean value.
 */
gboolean purple_account_option_get_default_bool(const PurpleAccountOption *option);

/**
 * Returns the default integer value for an account option.
 *
 * @param option The account option.
 *
 * @return The default integer value.
 */
int purple_account_option_get_default_int(const PurpleAccountOption *option);

/**
 * Returns the default string value for an account option.
 *
 * @param option The account option.
 *
 * @return The default string value.
 */
const char *purple_account_option_get_default_string(
	const PurpleAccountOption *option);

/**
 * Returns the default string value for a list account option.
 *
 * @param option The account option.
 *
 * @return The default list string value.
 */
const char *purple_account_option_get_default_list_value(
	const PurpleAccountOption *option);

/**
 * Returns whether an option's value should be masked from view, like a
 * password.  If so, the UI might display each character of the option
 * as a '*' (for example).
 *
 * @param option The account option.
 *
 * @return %TRUE if the option's value should be obscured.
 */
gboolean
purple_account_option_get_masked(const PurpleAccountOption *option);

/**
 * Returns the list values for an account option.
 *
 * @param option The account option.
 *
 * @constreturn A list of #PurpleKeyValuePair, mapping the human-readable
 *              description of the value to the <tt>(const char *)</tt> that
 *              should be passed to purple_account_set_string() to set the
 *              option.
 */
GList *purple_account_option_get_list(const PurpleAccountOption *option);

/*@}*/


/**************************************************************************/
/** @name Account User Split API                                          */
/**************************************************************************/
/*@{*/

/**
 * Creates a new account username split.
 *
 * @param text          The text of the option.
 * @param default_value The default value.
 * @param sep           The field separator.
 *
 * @return The new user split.
 */
PurpleAccountUserSplit *purple_account_user_split_new(const char *text,
												  const char *default_value,
												  char sep);

/**
 * Destroys an account username split.
 *
 * @param split The split to destroy.
 */
void purple_account_user_split_destroy(PurpleAccountUserSplit *split);

/**
 * Returns the text for an account username split.
 *
 * @param split The account username split.
 *
 * @return The account username split's text.
 */
const char *purple_account_user_split_get_text(const PurpleAccountUserSplit *split);

/**
 * Returns the default string value for an account split.
 *
 * @param split The account username split.
 *
 * @return The default string.
 */
const char *purple_account_user_split_get_default_value(
		const PurpleAccountUserSplit *split);

/**
 * Returns the field separator for an account split.
 *
 * @param split The account username split.
 *
 * @return The field separator.
 */
char purple_account_user_split_get_separator(const PurpleAccountUserSplit *split);

/**
 * Returns the 'reverse' value for an account split.
 *
 * @param split The account username split.
 *
 * @return The 'reverse' value.
 */
gboolean purple_account_user_split_get_reverse(const PurpleAccountUserSplit *split);

/**
 * Sets the 'reverse' value for an account split.
 *
 * @param split   The account username split.
 * @param reverse The 'reverse' value
 */
void purple_account_user_split_set_reverse(PurpleAccountUserSplit *split, gboolean reverse);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_ACCOUNTOPT_H_ */
