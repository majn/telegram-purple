/**
 * @file buddyicon.h Buddy Icon API
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
#ifndef _PURPLE_BUDDYICON_H_
#define _PURPLE_BUDDYICON_H_

/** An opaque structure representing a buddy icon for a particular user on a
 *  particular #PurpleAccount.  Instances are reference-counted; use
 *  purple_buddy_icon_ref() and purple_buddy_icon_unref() to take and release
 *  references.
 */
typedef struct _PurpleBuddyIcon PurpleBuddyIcon;

#include "account.h"
#include "blist.h"
#include "imgstore.h"
#include "prpl.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************/
/** @name Buddy Icon API                                                  */
/**************************************************************************/
/*@{*/

/**
 * Creates a new buddy icon structure and populates it.
 *
 * If the buddy icon already exists, you'll get a reference to that structure,
 * which will have been updated with the data supplied.
 *
 * @param account   The account the user is on.
 * @param username  The username the icon belongs to.
 * @param icon_data The buddy icon data.
 * @param icon_len  The buddy icon length.
 * @param checksum  A protocol checksum from the prpl or @c NULL.
 *
 * @return The buddy icon structure, with a reference for the caller.
 */
PurpleBuddyIcon *purple_buddy_icon_new(PurpleAccount *account, const char *username,
                                       void *icon_data, size_t icon_len,
                                       const char *checksum);

/**
 * Increments the reference count on a buddy icon.
 *
 * @param icon The buddy icon.
 *
 * @return @a icon.
 */
PurpleBuddyIcon *purple_buddy_icon_ref(PurpleBuddyIcon *icon);

/**
 * Decrements the reference count on a buddy icon.
 *
 * If the reference count reaches 0, the icon will be destroyed.
 *
 * @param icon The buddy icon.
 *
 * @return @a icon, or @c NULL if the reference count reached 0.
 */
PurpleBuddyIcon *purple_buddy_icon_unref(PurpleBuddyIcon *icon);

/**
 * Updates every instance of this icon.
 *
 * @param icon The buddy icon.
 */
void purple_buddy_icon_update(PurpleBuddyIcon *icon);

/**
 * Sets the buddy icon's data.
 *
 * @param icon The buddy icon.
 * @param data The buddy icon data, which the buddy icon code
 *             takes ownership of and will free.
 * @param len  The length of the data in @a data.
 * @param checksum  A protocol checksum from the prpl or @c NULL.
 */
void
purple_buddy_icon_set_data(PurpleBuddyIcon *icon, guchar *data,
                           size_t len, const char *checksum);

/**
 * Returns the buddy icon's account.
 *
 * @param icon The buddy icon.
 *
 * @return The account.
 */
PurpleAccount *purple_buddy_icon_get_account(const PurpleBuddyIcon *icon);

/**
 * Returns the buddy icon's username.
 *
 * @param icon The buddy icon.
 *
 * @return The username.
 */
const char *purple_buddy_icon_get_username(const PurpleBuddyIcon *icon);

/**
 * Returns the buddy icon's checksum.
 *
 * This function is really only for prpl use.
 *
 * @param icon The buddy icon.
 *
 * @return The checksum.
 */
const char *purple_buddy_icon_get_checksum(const PurpleBuddyIcon *icon);

/**
 * Returns the buddy icon's data.
 *
 * @param icon The buddy icon.
 * @param len  If not @c NULL, the length of the icon data returned will be
 *             set in the location pointed to by this.
 *
 * @return A pointer to the icon data.
 */
gconstpointer purple_buddy_icon_get_data(const PurpleBuddyIcon *icon, size_t *len);

/**
 * Returns an extension corresponding to the buddy icon's file type.
 *
 * @param icon The buddy icon.
 *
 * @return The icon's extension, "icon" if unknown, or @c NULL if
 *         the image data has disappeared.
 */
const char *purple_buddy_icon_get_extension(const PurpleBuddyIcon *icon);

/**
 * Returns a full path to an icon.
 *
 * If the icon has data and the file exists in the cache, this will return
 * a full path to the cache file.
 *
 * In general, it is not appropriate to be poking in the icon cache
 * directly.  If you find yourself wanting to use this function, think
 * very long and hard about it, and then don't.
 *
 * @param icon The buddy icon
 *
 * @return A full path to the file, or @c NULL under various conditions.
 */
char *purple_buddy_icon_get_full_path(PurpleBuddyIcon *icon);

/*@}*/

/**************************************************************************/
/** @name Buddy Icon Subsystem API                                        */
/**************************************************************************/
/*@{*/

/**
 * Sets a buddy icon for a user.
 *
 * @param account   The account the user is on.
 * @param username  The username of the user.
 * @param icon_data The buddy icon data, which the buddy icon code
 *                  takes ownership of and will free.
 * @param icon_len  The length of the icon data.
 * @param checksum  A protocol checksum from the prpl or @c NULL.
 */
void
purple_buddy_icons_set_for_user(PurpleAccount *account, const char *username,
                                void *icon_data, size_t icon_len,
                                const char *checksum);

/**
 * Returns the checksum for the buddy icon of a specified buddy.
 *
 * This avoids loading the icon image data from the cache if it's
 * not already loaded for some other reason.
 *
 * @param buddy The buddy
 *
 * @return The checksum.
 */
const char *
purple_buddy_icons_get_checksum_for_user(PurpleBuddy *buddy);

/**
 * Returns the buddy icon information for a user.
 *
 * @param account  The account the user is on.
 * @param username The username of the user.
 *
 * @return The icon (with a reference for the caller) if found, or @c NULL if
 *         not found.
 */
PurpleBuddyIcon *
purple_buddy_icons_find(PurpleAccount *account, const char *username);

/**
 * Returns the buddy icon image for an account.
 *
 * The caller owns a reference to the image in the store, and must dereference
 * the image with purple_imgstore_unref() for it to be freed.
 *
 * This function deals with loading the icon from the cache, if
 * needed, so it should be called in any case where you want the
 * appropriate icon.
 *
 * @param account The account
 *
 * @return The account's buddy icon image.
 */
PurpleStoredImage *
purple_buddy_icons_find_account_icon(PurpleAccount *account);

/**
 * Sets a buddy icon for an account.
 *
 * This function will deal with saving a record of the icon,
 * caching the data, etc.
 *
 * @param account   The account for which to set a custom icon.
 * @param icon_data The image data of the icon, which the
 *                  buddy icon code will free.
 * @param icon_len  The length of the data in @a icon_data.
 *
 * @return The icon that was set.  The caller does NOT own
 *         a reference to this, and must call purple_imgstore_ref()
 *         if it wants one.
 */
PurpleStoredImage *
purple_buddy_icons_set_account_icon(PurpleAccount *account,
                                    guchar *icon_data, size_t icon_len);

/**
 * Returns the timestamp of when the icon was set.
 *
 * This is intended for use in protocols that require a timestamp for
 * buddy icon update reasons.
 *
 * @param account The account
 *
 * @return The time the icon was set, or 0 if an error occurred.
 */
time_t
purple_buddy_icons_get_account_icon_timestamp(PurpleAccount *account);

/**
 * Returns a boolean indicating if a given blist node has a custom buddy icon.
 *
 * @param node The blist node.
 *
 * @return A boolean indicating if @a node has a custom buddy icon.
 * @since 2.5.0
 */
gboolean
purple_buddy_icons_node_has_custom_icon(PurpleBlistNode *node);

/**
 * Returns the custom buddy icon image for a blist node.
 *
 * The caller owns a reference to the image in the store, and must dereference
 * the image with purple_imgstore_unref() for it to be freed.
 *
 * This function deals with loading the icon from the cache, if
 * needed, so it should be called in any case where you want the
 * appropriate icon.
 *
 * @param node The node.
 *
 * @return The custom buddy icon.
 * @since 2.5.0
 */
PurpleStoredImage *
purple_buddy_icons_node_find_custom_icon(PurpleBlistNode *node);

/**
 * Sets a custom buddy icon for a blist node.
 *
 * This function will deal with saving a record of the icon, caching the data,
 * etc.
 *
 * @param node      The blist node for which to set a custom icon.
 * @param icon_data The image data of the icon, which the buddy icon code will
 *                  free. Use NULL to unset the icon.
 * @param icon_len  The length of the data in @a icon_data.
 *
 * @return The icon that was set. The caller does NOT own a reference to this,
 *         and must call purple_imgstore_ref() if it wants one.
 * @since 2.5.0
 */
PurpleStoredImage *
purple_buddy_icons_node_set_custom_icon(PurpleBlistNode *node,
                                        guchar *icon_data, size_t icon_len);

/**
 * Sets a custom buddy icon for a blist node.
 *
 * Convenience wrapper around purple_buddy_icons_node_set_custom_icon.
 * @see purple_buddy_icons_node_set_custom_icon()
 *
 * @param node      The blist node for which to set a custom icon.
 * @param filename  The path to the icon to set for the blist node. Use NULL
 *                  to unset the custom icon.
 *
 * @return The icon that was set. The caller does NOT own a reference to this,
 *         and must call purple_imgstore_ref() if it wants one.
 * @since 2.5.0
 */
PurpleStoredImage *
purple_buddy_icons_node_set_custom_icon_from_file(PurpleBlistNode *node,
                                                  const gchar *filename);

#if !(defined PURPLE_DISABLE_DEPRECATED) || (defined _PURPLE_BUDDYICON_C_)
/**
 * PurpleContact version of purple_buddy_icons_node_has_custom_icon.
 *
 * @copydoc purple_buddy_icons_node_has_custom_icon()
 *
 * @deprecated Use purple_buddy_icons_node_has_custom_icon instead.
 */
gboolean
purple_buddy_icons_has_custom_icon(PurpleContact *contact);

/**
 * PurpleContact version of purple_buddy_icons_node_find_custom_icon.
 *
 * @copydoc purple_buddy_icons_node_find_custom_icon()
 *
 * @deprecated Use purple_buddy_icons_node_find_custom_icon instead.
 */
PurpleStoredImage *
purple_buddy_icons_find_custom_icon(PurpleContact *contact);

/**
 * PurpleContact version of purple_buddy_icons_node_set_custom_icon.
 *
 * @copydoc purple_buddy_icons_node_set_custom_icon()
 *
 * @deprecated Use purple_buddy_icons_node_set_custom_icon instead.
 */
PurpleStoredImage *
purple_buddy_icons_set_custom_icon(PurpleContact *contact,
                                   guchar *icon_data, size_t icon_len);
#endif

/**
 * Sets whether or not buddy icon caching is enabled.
 *
 * @param caching TRUE of buddy icon caching should be enabled, or
 *                FALSE otherwise.
 */
void purple_buddy_icons_set_caching(gboolean caching);

/**
 * Returns whether or not buddy icon caching should be enabled.
 *
 * The default is TRUE, unless otherwise specified by
 * purple_buddy_icons_set_caching().
 *
 * @return TRUE if buddy icon caching is enabled, or FALSE otherwise.
 */
gboolean purple_buddy_icons_is_caching(void);

/**
 * Sets the directory used to store buddy icon cache files.
 *
 * @param cache_dir The directory to store buddy icon cache files to.
 */
void purple_buddy_icons_set_cache_dir(const char *cache_dir);

/**
 * Returns the directory used to store buddy icon cache files.
 *
 * The default directory is PURPLEDIR/icons, unless otherwise specified
 * by purple_buddy_icons_set_cache_dir().
 *
 * @return The directory to store buddy icon cache files to.
 */
const char *purple_buddy_icons_get_cache_dir(void);

/**
 * Returns the buddy icon subsystem handle.
 *
 * @return The subsystem handle.
 */
void *purple_buddy_icons_get_handle(void);

/**
 * Initializes the buddy icon subsystem.
 */
void purple_buddy_icons_init(void);

/**
 * Uninitializes the buddy icon subsystem.
 */
void purple_buddy_icons_uninit(void);

/*@}*/

/**************************************************************************/
/** @name Buddy Icon Helper API                                           */
/**************************************************************************/
/*@{*/

/**
 * Gets display size for a buddy icon
 */
void purple_buddy_icon_get_scale_size(PurpleBuddyIconSpec *spec, int *width, int *height);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_BUDDYICON_H_ */
