/**
 * @file notify.h Notification API
 * @ingroup core
 * @see @ref notify-signals
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
#ifndef _PURPLE_NOTIFY_H_
#define _PURPLE_NOTIFY_H_

#include <stdlib.h>
#include <glib-object.h>
#include <glib.h>

typedef struct _PurpleNotifyUserInfoEntry	PurpleNotifyUserInfoEntry;
typedef struct _PurpleNotifyUserInfo	PurpleNotifyUserInfo;

#include "connection.h"

/**
 * Notification close callbacks.
 */
typedef void  (*PurpleNotifyCloseCallback) (gpointer user_data);


/**
 * Notification types.
 */
typedef enum
{
	PURPLE_NOTIFY_MESSAGE = 0,   /**< Message notification.         */
	PURPLE_NOTIFY_EMAIL,         /**< Single email notification.   */
	PURPLE_NOTIFY_EMAILS,        /**< Multiple email notification. */
	PURPLE_NOTIFY_FORMATTED,     /**< Formatted text.               */
	PURPLE_NOTIFY_SEARCHRESULTS, /**< Buddy search results.         */
	PURPLE_NOTIFY_USERINFO,      /**< Formatted userinfo text.      */
	PURPLE_NOTIFY_URI            /**< URI notification or display.  */

} PurpleNotifyType;


/**
 * Notification message types.
 */
typedef enum
{
	PURPLE_NOTIFY_MSG_ERROR   = 0, /**< Error notification.       */
	PURPLE_NOTIFY_MSG_WARNING,     /**< Warning notification.     */
	PURPLE_NOTIFY_MSG_INFO         /**< Information notification. */

} PurpleNotifyMsgType;


/**
 * The types of buttons
 */
typedef enum
{
	PURPLE_NOTIFY_BUTTON_LABELED = 0,  /**< special use, see _button_add_labeled */
	PURPLE_NOTIFY_BUTTON_CONTINUE = 1,
	PURPLE_NOTIFY_BUTTON_ADD,
	PURPLE_NOTIFY_BUTTON_INFO,
	PURPLE_NOTIFY_BUTTON_IM,
	PURPLE_NOTIFY_BUTTON_JOIN,
	PURPLE_NOTIFY_BUTTON_INVITE
} PurpleNotifySearchButtonType;


/**
 * Search results object.
 */
typedef struct
{
	GList *columns;        /**< List of the search column objects. */
	GList *rows;           /**< List of rows in the result. */
	GList *buttons;        /**< List of buttons to display. */

} PurpleNotifySearchResults;

/**
 * Types of PurpleNotifyUserInfoEntry objects
 */
typedef enum
{
	PURPLE_NOTIFY_USER_INFO_ENTRY_PAIR = 0,
	PURPLE_NOTIFY_USER_INFO_ENTRY_SECTION_BREAK,
	PURPLE_NOTIFY_USER_INFO_ENTRY_SECTION_HEADER
} PurpleNotifyUserInfoEntryType;

/**
 * Single column of a search result.
 */
typedef struct
{
	char *title; /**< Title of the column. */

} PurpleNotifySearchColumn;


/**
 * Callback for a button in a search result.
 *
 * @param c         the PurpleConnection passed to purple_notify_searchresults
 * @param row       the contents of the selected row
 * @param user_data User defined data.
 */
typedef void (*PurpleNotifySearchResultsCallback)(PurpleConnection *c, GList *row,
												gpointer user_data);


/**
 * Definition of a button.
 */
typedef struct
{
	PurpleNotifySearchButtonType type;
	PurpleNotifySearchResultsCallback callback; /**< Function to be called when clicked. */
	char *label;                              /**< only for PURPLE_NOTIFY_BUTTON_LABELED */
} PurpleNotifySearchButton;


/**
 * Notification UI operations.
 */
typedef struct
{
	void *(*notify_message)(PurpleNotifyMsgType type, const char *title,
	                        const char *primary, const char *secondary);

	void *(*notify_email)(PurpleConnection *gc,
	                      const char *subject, const char *from,
	                      const char *to, const char *url);

	void *(*notify_emails)(PurpleConnection *gc,
	                       size_t count, gboolean detailed,
	                       const char **subjects, const char **froms,
	                       const char **tos, const char **urls);

	void *(*notify_formatted)(const char *title, const char *primary,
	                          const char *secondary, const char *text);

	void *(*notify_searchresults)(PurpleConnection *gc, const char *title,
	                              const char *primary, const char *secondary,
	                              PurpleNotifySearchResults *results, gpointer user_data);

	void (*notify_searchresults_new_rows)(PurpleConnection *gc,
	                                      PurpleNotifySearchResults *results,
	                                      void *data);

	void *(*notify_userinfo)(PurpleConnection *gc, const char *who,
	                         PurpleNotifyUserInfo *user_info);

	void *(*notify_uri)(const char *uri);

	void (*close_notify)(PurpleNotifyType type, void *ui_handle);

	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
} PurpleNotifyUiOps;


#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************/
/** Search results notification API                                       */
/**************************************************************************/
/*@{*/

/**
 * Displays results from a buddy search.  This can be, for example,
 * a window with a list of all found buddies, where you are given the
 * option of adding buddies to your buddy list.
 *
 * @param gc        The PurpleConnection handle associated with the information.
 * @param title     The title of the message.  If this is NULL, the title
 *                  will be "Search Results."
 * @param primary   The main point of the message.
 * @param secondary The secondary information.
 * @param results   The PurpleNotifySearchResults instance.
 * @param cb        The callback to call when the user closes
 *                  the notification.
 * @param user_data The data to pass to the close callback and any other
 *                  callback associated with a button.
 *
 * @return A UI-specific handle.
 */
void *purple_notify_searchresults(PurpleConnection *gc, const char *title,
								const char *primary, const char *secondary,
								PurpleNotifySearchResults *results, PurpleNotifyCloseCallback cb,
								gpointer user_data);

/**
 * Frees a PurpleNotifySearchResults object.
 *
 * @param results The PurpleNotifySearchResults to free.
 */
void purple_notify_searchresults_free(PurpleNotifySearchResults *results);

/**
 * Replace old rows with the new. Reuse an existing window.
 *
 * @param gc        The PurpleConnection structure.
 * @param results   The PurpleNotifySearchResults structure.
 * @param data      Data returned by the purple_notify_searchresults().
 */
void purple_notify_searchresults_new_rows(PurpleConnection *gc,
										PurpleNotifySearchResults *results,
										void *data);


/**
 * Adds a stock button that will be displayed in the search results dialog.
 *
 * @param results The search results object.
 * @param type    Type of the button. (TODO: Only one button of a given type
 *                can be displayed.)
 * @param cb      Function that will be called on the click event.
 */
void purple_notify_searchresults_button_add(PurpleNotifySearchResults *results,
										  PurpleNotifySearchButtonType type,
										  PurpleNotifySearchResultsCallback cb);


/**
 * Adds a plain labelled button that will be displayed in the search results
 * dialog.
 *
 * @param results The search results object
 * @param label   The label to display
 * @param cb      Function that will be called on the click event
 */
void purple_notify_searchresults_button_add_labeled(PurpleNotifySearchResults *results,
                                                  const char *label,
                                                  PurpleNotifySearchResultsCallback cb);


/**
 * Returns a newly created search results object.
 *
 * @return The new search results object.
 */
PurpleNotifySearchResults *purple_notify_searchresults_new(void);

/**
 * Returns a newly created search result column object.
 *
 * @param title Title of the column. NOTE: Title will get g_strdup()ed.
 *
 * @return The new search column object.
 */
PurpleNotifySearchColumn *purple_notify_searchresults_column_new(const char *title);

/**
 * Adds a new column to the search result object.
 *
 * @param results The result object to which the column will be added.
 * @param column The column that will be added to the result object.
 */
void purple_notify_searchresults_column_add(PurpleNotifySearchResults *results,
										  PurpleNotifySearchColumn *column);

/**
 * Adds a new row of the results to the search results object.
 *
 * @param results The search results object.
 * @param row     The row of the results.
 */
void purple_notify_searchresults_row_add(PurpleNotifySearchResults *results,
									   GList *row);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_NOTIFY_C_)
/**
 * Returns a number of the rows in the search results object.
 *
 * @deprecated This function will be removed in Pidgin 3.0.0 unless
 *             there is sufficient demand to keep it.  Using this
 *             function encourages looping through the results
 *             inefficiently.  Instead of using this function you
 *             should iterate through the results using a loop
 *             similar to this:
 *                for (l = results->rows; l != NULL; l = l->next)
 *             If you really need to get the number of rows you
 *             can use g_list_length(results->rows).
 *
 * @param results The search results object.
 *
 * @return Number of the result rows.
 */
guint purple_notify_searchresults_get_rows_count(PurpleNotifySearchResults *results);
#endif

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_NOTIFY_C_)
/**
 * Returns a number of the columns in the search results object.
 *
 * @deprecated This function will be removed in Pidgin 3.0.0 unless
 *             there is sufficient demand to keep it.  Using this
 *             function encourages looping through the columns
 *             inefficiently.  Instead of using this function you
 *             should iterate through the columns using a loop
 *             similar to this:
 *                for (l = results->columns; l != NULL; l = l->next)
 *             If you really need to get the number of columns you
 *             can use g_list_length(results->columns).
 *
 * @param results The search results object.
 *
 * @return Number of the columns.
 */
guint purple_notify_searchresults_get_columns_count(PurpleNotifySearchResults *results);
#endif

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_NOTIFY_C_)
/**
 * Returns a row of the results from the search results object.
 *
 * @deprecated This function will be removed in Pidgin 3.0.0 unless
 *             there is sufficient demand to keep it.  Using this
 *             function encourages looping through the results
 *             inefficiently.  Instead of using this function you
 *             should iterate through the results using a loop
 *             similar to this:
 *                for (l = results->rows; l != NULL; l = l->next)
 *             If you really need to get the data for a particular
 *             row you can use g_list_nth_data(results->rows, row_id).
 *
 * @param results The search results object.
 * @param row_id  Index of the row to be returned.
 *
 * @return Row of the results.
 */
GList *purple_notify_searchresults_row_get(PurpleNotifySearchResults *results,
										 unsigned int row_id);
#endif

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_NOTIFY_C_)
/**
 * Returns a title of the search results object's column.
 *
 * @deprecated This function will be removed in Pidgin 3.0.0 unless
 *             there is sufficient demand to keep it.  Using this
 *             function encourages looping through the columns
 *             inefficiently.  Instead of using this function you
 *             should iterate through the name of a particular
 *             column you can use
 *             g_list_nth_data(results->columns, row_id).
 *
 * @param results   The search results object.
 * @param column_id Index of the column.
 *
 * @return Title of the column.
 */
char *purple_notify_searchresults_column_get_title(PurpleNotifySearchResults *results,
												 unsigned int column_id);
#endif

/*@}*/

/**************************************************************************/
/** @name Notification API                                                */
/**************************************************************************/
/*@{*/

/**
 * Displays a notification message to the user.
 *
 * @param handle    The plugin or connection handle.
 * @param type      The notification type.
 * @param title     The title of the message.
 * @param primary   The main point of the message.
 * @param secondary The secondary information.
 * @param cb        The callback to call when the user closes
 *                  the notification.
 * @param user_data The data to pass to the callback.
 *
 * @return A UI-specific handle.
 */
void *purple_notify_message(void *handle, PurpleNotifyMsgType type,
						  const char *title, const char *primary,
						  const char *secondary, PurpleNotifyCloseCallback cb,
						  gpointer user_data);

/**
 * Displays a single email notification to the user.
 *
 * @param handle    The plugin or connection handle.
 * @param subject   The subject of the email.
 * @param from      The from address.
 * @param to        The destination address.
 * @param url       The URL where the message can be read.
 * @param cb        The callback to call when the user closes
 *                  the notification.
 * @param user_data The data to pass to the callback.
 *
 * @return A UI-specific handle.
 */
void *purple_notify_email(void *handle, const char *subject,
						const char *from, const char *to,
						const char *url, PurpleNotifyCloseCallback cb,
						gpointer user_data);

/**
 * Displays a notification for multiple emails to the user.
 *
 * @param handle    The plugin or connection handle.
 * @param count     The number of emails.  '0' can be used to signify that
 *                  the user has no unread emails and the UI should remove
 *                  the mail notification.
 * @param detailed  @c TRUE if there is information for each email in the
 *                  arrays.
 * @param subjects  The array of subjects.
 * @param froms     The array of from addresses.
 * @param tos       The array of destination addresses.
 * @param urls      The URLs where the messages can be read.
 * @param cb        The callback to call when the user closes
 *                  the notification.
 * @param user_data The data to pass to the callback.
 *
 * @return A UI-specific handle.
 */
void *purple_notify_emails(void *handle, size_t count, gboolean detailed,
						 const char **subjects, const char **froms,
						 const char **tos, const char **urls,
						 PurpleNotifyCloseCallback cb, gpointer user_data);

/**
 * Displays a notification with formatted text.
 *
 * The text is essentially a stripped-down format of HTML, the same that
 * IMs may send.
 *
 * @param handle    The plugin or connection handle.
 * @param title     The title of the message.
 * @param primary   The main point of the message.
 * @param secondary The secondary information.
 * @param text      The formatted text.
 * @param cb        The callback to call when the user closes
 *                  the notification.
 * @param user_data The data to pass to the callback.
 *
 * @return A UI-specific handle.
 */
void *purple_notify_formatted(void *handle, const char *title,
							const char *primary, const char *secondary,
							const char *text, PurpleNotifyCloseCallback cb, gpointer user_data);

/**
 * Displays user information with formatted text, passing information giving
 * the connection and username from which the user information came.
 *
 * The text is essentially a stripped-down format of HTML, the same that
 * IMs may send.
 *
 * @param gc         The PurpleConnection handle associated with the information.
 * @param who        The username associated with the information.
 * @param user_info  The PurpleNotifyUserInfo which contains the information
 * @param cb         The callback to call when the user closes the notification.
 * @param user_data  The data to pass to the callback.
 *
 * @return  A UI-specific handle.
 */
void *purple_notify_userinfo(PurpleConnection *gc, const char *who,
						   PurpleNotifyUserInfo *user_info, PurpleNotifyCloseCallback cb,
						   gpointer user_data);

/**
 * Create a new PurpleNotifyUserInfo which is suitable for passing to
 * purple_notify_userinfo()
 *
 * @return  A new PurpleNotifyUserInfo, which the caller must destroy when done
 */
PurpleNotifyUserInfo *purple_notify_user_info_new(void);

/**
 * Destroy a PurpleNotifyUserInfo
 *
 * @param user_info  The PurpleNotifyUserInfo
 */
void purple_notify_user_info_destroy(PurpleNotifyUserInfo *user_info);

/**
 * Retrieve the array of PurpleNotifyUserInfoEntry objects from a
 * PurpleNotifyUserInfo
 *
 * This GList may be manipulated directly with normal GList functions such
 * as g_list_insert(). Only PurpleNotifyUserInfoEntry are allowed in the
 * list.  If a PurpleNotifyUserInfoEntry item is added to the list, it
 * should not be g_free()'d by the caller; PurpleNotifyUserInfo will g_free
 * it when destroyed.
 *
 * To remove a PurpleNotifyUserInfoEntry, use
 * purple_notify_user_info_remove_entry(). Do not use the GList directly.
 *
 * @param user_info  The PurpleNotifyUserInfo
 *
 * @constreturn A GList of PurpleNotifyUserInfoEntry objects
 */
GList *purple_notify_user_info_get_entries(PurpleNotifyUserInfo *user_info);

/**
 * Create a textual representation of a PurpleNotifyUserInfo, separating
 * entries with newline
 *
 * @param user_info  The PurpleNotifyUserInfo
 * @param newline    The separation character
 */
char *purple_notify_user_info_get_text_with_newline(PurpleNotifyUserInfo *user_info, const char *newline);

/**
 * Add a label/value pair to a PurpleNotifyUserInfo object.
 * PurpleNotifyUserInfo keeps track of the order in which pairs are added.
 *
 * @param user_info  The PurpleNotifyUserInfo
 * @param label      A label, which for example might be displayed by a
 *                   UI with a colon after it ("Status:"). Do not include
 *                   a colon.  If NULL, value will be displayed without a
 *                   label.
 * @param value      The value, which might be displayed by a UI after
 *                   the label.  This should be valid HTML.  If you want
 *                   to insert plaintext then use
 *                   purple_notify_user_info_add_pair_plaintext(), instead.
 *                   If this is NULL the label will still be displayed;
 *                   the UI should treat label as independent and not
 *                   include a colon if it would otherwise.
 */
/*
 * TODO: In 3.0.0 this function should be renamed to
 *       purple_notify_user_info_add_pair_html().  And optionally
 *       purple_notify_user_info_add_pair_plaintext() could be renamed to
 *       purple_notify_user_info_add_pair().
 */
void purple_notify_user_info_add_pair(PurpleNotifyUserInfo *user_info, const char *label, const char *value);

/**
 * Like purple_notify_user_info_add_pair, but value should be plaintext
 * and will be escaped using g_markup_escape_text().
 */
void purple_notify_user_info_add_pair_plaintext(PurpleNotifyUserInfo *user_info, const char *label, const char *value);

/**
 * Prepend a label/value pair to a PurpleNotifyUserInfo object
 *
 * @param user_info  The PurpleNotifyUserInfo
 * @param label      A label, which for example might be displayed by a
 *                   UI with a colon after it ("Status:"). Do not include
 *                   a colon.  If NULL, value will be displayed without a
 *                   label.
 * @param value      The value, which might be displayed by a UI after
 *                   the label.  If NULL, label will still be displayed;
 *                   the UI should then treat label as independent and not
 *                   include a colon if it would otherwise.
 */
void purple_notify_user_info_prepend_pair(PurpleNotifyUserInfo *user_info, const char *label, const char *value);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_NOTIFY_C_)
/**
 * Remove a PurpleNotifyUserInfoEntry from a PurpleNotifyUserInfo object
 * without freeing the entry.
 *
 * @param user_info        The PurpleNotifyUserInfo
 * @param user_info_entry  The PurpleNotifyUserInfoEntry
 *
 * @deprecated Nothing is using this function and it should be removed
 *             in 3.0.0.  Or, if we decide we want to keep it in 3.0.0
 *             then we should make purple_notify_user_info_entry_destroy
 *             public so that entries can be free'd after they're removed.
 */
void purple_notify_user_info_remove_entry(PurpleNotifyUserInfo *user_info, PurpleNotifyUserInfoEntry *user_info_entry);
#endif

/**
 * Create a new PurpleNotifyUserInfoEntry
 *
 * If added to a PurpleNotifyUserInfo object, this should not be free()'d,
 * as PurpleNotifyUserInfo will do so when destroyed.
 * purple_notify_user_info_add_pair() and
 * purple_notify_user_info_prepend_pair() are convenience methods for
 * creating entries and adding them to a PurpleNotifyUserInfo.
 *
 * @param label  A label, which for example might be displayed by a UI
 *               with a colon after it ("Status:"). Do not include a
 *               colon.  If NULL, value will be displayed without a label.
 * @param value  The value, which might be displayed by a UI after the
 *               label.  If NULL, label will still be displayed; the UI
 *               should then treat label as independent and not include a
 *               colon if it would otherwise.
 *
 * @result A new PurpleNotifyUserInfoEntry
 */
PurpleNotifyUserInfoEntry *purple_notify_user_info_entry_new(const char *label, const char *value);

/**
 * Add a section break.  A UI might display this as a horizontal line.
 *
 * @param user_info  The PurpleNotifyUserInfo
 */
void purple_notify_user_info_add_section_break(PurpleNotifyUserInfo *user_info);

/**
 * Prepend a section break.  A UI might display this as a horizontal line.
 *
 * @param user_info  The PurpleNotifyUserInfo
 * @since 2.5.0
 */
void purple_notify_user_info_prepend_section_break(PurpleNotifyUserInfo *user_info);

/**
 * Add a section header.  A UI might display this in a different font
 * from other text.
 *
 * @param user_info  The PurpleNotifyUserInfo
 * @param label      The name of the section
 */
void purple_notify_user_info_add_section_header(PurpleNotifyUserInfo *user_info, const char *label);

/**
 * Prepend a section header.  A UI might display this in a different font
 * from other text.
 *
 * @param user_info  The PurpleNotifyUserInfo
 * @param label      The name of the section
 * @since 2.5.0
 */
void purple_notify_user_info_prepend_section_header(PurpleNotifyUserInfo *user_info, const char *label);

/**
 * Remove the last item which was added to a PurpleNotifyUserInfo. This
 * could be used to remove a section header which is not needed.
 */
void purple_notify_user_info_remove_last_item(PurpleNotifyUserInfo *user_info);

/**
 * Get the label for a PurpleNotifyUserInfoEntry
 *
 * @param user_info_entry  The PurpleNotifyUserInfoEntry
 *
 * @return  The label
 */
const gchar *purple_notify_user_info_entry_get_label(PurpleNotifyUserInfoEntry *user_info_entry);

/**
 * Set the label for a PurpleNotifyUserInfoEntry
 *
 * @param user_info_entry  The PurpleNotifyUserInfoEntry
 * @param label            The label
 */
void purple_notify_user_info_entry_set_label(PurpleNotifyUserInfoEntry *user_info_entry, const char *label);

/**
 * Get the value for a PurpleNotifyUserInfoEntry
 *
 * @param user_info_entry  The PurpleNotifyUserInfoEntry
 *
 * @result  The value
 */
const gchar *purple_notify_user_info_entry_get_value(PurpleNotifyUserInfoEntry *user_info_entry);

/**
 * Set the value for a PurpleNotifyUserInfoEntry
 *
 * @param user_info_entry  The PurpleNotifyUserInfoEntry
 * @param value            The value
 */
void purple_notify_user_info_entry_set_value(PurpleNotifyUserInfoEntry *user_info_entry, const char *value);


/**
 * Get the type of a PurpleNotifyUserInfoEntry
 *
 * @param user_info_entry  The PurpleNotifyUserInfoEntry
 *
 * @return  The PurpleNotifyUserInfoEntryType
 */
PurpleNotifyUserInfoEntryType purple_notify_user_info_entry_get_type(PurpleNotifyUserInfoEntry *user_info_entry);

/**
 * Set the type of a PurpleNotifyUserInfoEntry
 *
 * @param user_info_entry  The PurpleNotifyUserInfoEntry
 * @param type             The PurpleNotifyUserInfoEntryType
 */
void purple_notify_user_info_entry_set_type(PurpleNotifyUserInfoEntry *user_info_entry,
										  PurpleNotifyUserInfoEntryType type);

/**
 * Opens a URI or somehow presents it to the user.
 *
 * @param handle The plugin or connection handle.
 * @param uri    The URI to display or go to.
 *
 * @return A UI-specific handle, if any. This may only be presented if
 *         the UI code displays a dialog instead of a webpage, or something
 *         similar.
 */
void *purple_notify_uri(void *handle, const char *uri);

/**
 * Closes a notification.
 *
 * This should be used only by the UI operation functions and part of the
 * core.
 *
 * @param type      The notification type.
 * @param ui_handle The notification UI handle.
 */
void purple_notify_close(PurpleNotifyType type, void *ui_handle);

/**
 * Closes all notifications registered with the specified handle.
 *
 * @param handle The handle.
 */
void purple_notify_close_with_handle(void *handle);

/**
 * A wrapper for purple_notify_message that displays an information message.
 */
#define purple_notify_info(handle, title, primary, secondary) \
	purple_notify_message((handle), PURPLE_NOTIFY_MSG_INFO, (title), \
						(primary), (secondary), NULL, NULL)

/**
 * A wrapper for purple_notify_message that displays a warning message.
 */
#define purple_notify_warning(handle, title, primary, secondary) \
	purple_notify_message((handle), PURPLE_NOTIFY_MSG_WARNING, (title), \
						(primary), (secondary), NULL, NULL)

/**
 * A wrapper for purple_notify_message that displays an error message.
 */
#define purple_notify_error(handle, title, primary, secondary) \
	purple_notify_message((handle), PURPLE_NOTIFY_MSG_ERROR, (title), \
						(primary), (secondary), NULL, NULL)

/*@}*/

/**************************************************************************/
/** @name UI Registration Functions                                       */
/**************************************************************************/
/*@{*/

/**
 * Sets the UI operations structure to be used when displaying a
 * notification.
 *
 * @param ops The UI operations structure.
 */
void purple_notify_set_ui_ops(PurpleNotifyUiOps *ops);

/**
 * Returns the UI operations structure to be used when displaying a
 * notification.
 *
 * @return The UI operations structure.
 */
PurpleNotifyUiOps *purple_notify_get_ui_ops(void);

/*@}*/

/**************************************************************************/
/** @name Notify Subsystem                                         */
/**************************************************************************/
/*@{*/

/**
 * Returns the notify subsystem handle.
 *
 * @return The notify subsystem handle.
 */
void *purple_notify_get_handle(void);

/**
 * Initializes the notify subsystem.
 */
void purple_notify_init(void);

/**
 * Uninitializes the notify subsystem.
 */
void purple_notify_uninit(void);

/*@}*/


#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_NOTIFY_H_ */
