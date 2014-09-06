/**
 * @file imgstore.h IM Image Store API
 * @ingroup core
 * @see @ref imgstore-signals
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
#ifndef _PURPLE_IMGSTORE_H_
#define _PURPLE_IMGSTORE_H_

#include <glib.h>

/**
 * A set of utility functions that provide a reference-counted immutable
 * wrapper around an image's data and filename.  These functions do not
 * cache any data to disk.
 */
typedef struct _PurpleStoredImage PurpleStoredImage;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create a new PurpleStoredImage.
 *
 * Despite the name of this function, the image is NOT added to the image
 * store and no ID is assigned.  If you need to reference the image by an
 * ID, use purple_imgstore_add_with_id() instead.
 *
 * The caller owns a reference to this image and must dereference it with
 * purple_imgstore_unref() for it to be freed.
 *
 * @param data      Pointer to the image data, which the imgstore will take
 *                  ownership of and free as appropriate.  If you want a
 *                  copy of the data, make it before calling this function.
 * @param size      Image data's size.
 * @param filename  Filename associated with image.  This is for your
 *                  convenience.  It could be the full path to the
 *                  image or, more commonly, the filename of the image
 *                  without any directory information.  It can also be
 *                  NULL, if you don't need to keep track of a filename.
 *
 * @return The stored image, or NULL if the image was not added (because of
 *         empty data or size).
 */
PurpleStoredImage *
purple_imgstore_add(gpointer data, size_t size, const char *filename);

/**
 * Create a PurpleStoredImage using purple_imgstore_add() by reading the
 * given filename from disk.
 *
 * The image is not added to the image store and no ID is assigned.  If you
 * need to reference the image by an ID, use purple_imgstore_add_with_id()
 * instead.
 *
 * The caller owns a reference to this image and must dereference it with
 * purple_imgstore_unref() for it to be freed.
 *
 * @param path  The path to the image.
 *
 * @return The stored image, or NULL if the image was not added (because of
 *         empty data or size).
 *
 * @since 2.5.0
 */
PurpleStoredImage *
purple_imgstore_new_from_file(const char *path);

/**
 * Create a PurpleStoredImage using purple_imgstore_add() and add the
 * image to the image store.  A unique ID will be assigned to the image.
 *
 * The caller owns a reference to the image and must dereference it with
 * purple_imgstore_unref() or purple_imgstore_unref_by_id() for it to be
 * freed.
 *
 * @param data      Pointer to the image data, which the imgstore will take
 *                  ownership of and free as appropriate.  If you want a
 *                  copy of the data, make it before calling this function.
 * @param size      Image data's size.
 * @param filename  Filename associated with image.  This is for your
 *                  convenience.  It could be the full path to the
 *                  image or, more commonly, the filename of the image
 *                  without any directory information.  It can also be
 *                  NULL, if you don't need to keep track of a filename.
 *
 * @return ID for the image.  This is a unique number that can be used
 *         within libpurple to reference the image.  0 is returned if the
 *         image was not added (because of empty data or size).
 */
int purple_imgstore_add_with_id(gpointer data, size_t size, const char *filename);

/**
 * Retrieve an image from the store. The caller does not own a
 * reference to the image.
 *
 * @param id The ID for the image.
 *
 * @return A pointer to the requested image, or NULL if it was not found.
 */
PurpleStoredImage *purple_imgstore_find_by_id(int id);

/**
 * Retrieves a pointer to the image's data.
 *
 * @param img The Image.
 *
 * @return A pointer to the data, which must not
 *         be freed or modified.
 */
gconstpointer purple_imgstore_get_data(PurpleStoredImage *img);

/**
 * Retrieves the length of the image's data.
 *
 * @param img The Image.
 *
 * @return The size of the data that the pointer returned by
 *         purple_imgstore_get_data points to.
 */
size_t purple_imgstore_get_size(PurpleStoredImage *img);

/**
 * Retrieves a pointer to the image's filename.
 *
 * @param img The image.
 *
 * @return A pointer to the filename, which must not
 *         be freed or modified.
 */
const char *purple_imgstore_get_filename(const PurpleStoredImage *img);

/**
 * Looks at the magic numbers of the image data (the first few bytes)
 * and returns an extension corresponding to the image's file type.
 *
 * @param img The image.
 *
 * @return The image's extension (for example "png") or "icon"
 *         if unknown.
 */
const char *purple_imgstore_get_extension(PurpleStoredImage *img);

/**
 * Increment the reference count.
 *
 * @param img The image.
 *
 * @return @a img
 */
PurpleStoredImage *
purple_imgstore_ref(PurpleStoredImage *img);

/**
 * Decrement the reference count.
 *
 * If the reference count reaches zero, the image will be freed.
 *
 * @param img The image.
 *
 * @return @a img or @c NULL if the reference count reached zero.
 */
PurpleStoredImage *
purple_imgstore_unref(PurpleStoredImage *img);

/**
 * Increment the reference count using an ID.
 *
 * This is a convience wrapper for purple_imgstore_find_by_id() and
 * purple_imgstore_ref(), so if you have a PurpleStoredImage, it'll
 * be more efficient to call purple_imgstore_ref() directly.
 *
 * @param id The ID for the image.
 */
void purple_imgstore_ref_by_id(int id);

/**
 * Decrement the reference count using an ID.
 *
 * This is a convience wrapper for purple_imgstore_find_by_id() and
 * purple_imgstore_unref(), so if you have a PurpleStoredImage, it'll
 * be more efficient to call purple_imgstore_unref() directly.
 *
 * @param id The ID for the image.
 */
void purple_imgstore_unref_by_id(int id);

/**
 * Returns the image store subsystem handle.
 *
 * @return The subsystem handle.
 */
void *purple_imgstore_get_handle(void);

/**
 * Initializes the image store subsystem.
 */
void purple_imgstore_init(void);

/**
 * Uninitializes the image store subsystem.
 */
void purple_imgstore_uninit(void);

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_IMGSTORE_H_ */
