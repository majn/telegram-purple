/**
 * @file desktopitem.h Functions for managing .desktop files
 * @ingroup core
 */

/* Purple is the legal property of its developers, whose names are too numerous
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

/*
 * The following code has been adapted from gnome-desktop-item.[ch],
 * as found on gnome-desktop-2.8.1.
 *
 *   Copyright (C) 2004 by Alceste Scalas <alceste.scalas@gmx.net>.
 *
 * Original copyright notice:
 *
 * Copyright (C) 1999, 2000 Red Hat Inc.
 * Copyright (C) 2001 Sid Vicious
 * All rights reserved.
 *
 * This file is part of the Gnome Library.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02111-1301, USA.
 */

#ifndef _PURPLE_DESKTOP_ITEM_H_
#define _PURPLE_DESKTOP_ITEM_H_

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef enum {
	PURPLE_DESKTOP_ITEM_TYPE_NULL = 0 /* This means its NULL, that is, not
					  * set */,
	PURPLE_DESKTOP_ITEM_TYPE_OTHER /* This means it's not one of the below
					 strings types, and you must get the
					 Type attribute. */,

	/* These are the standard compliant types: */
	PURPLE_DESKTOP_ITEM_TYPE_APPLICATION,
	PURPLE_DESKTOP_ITEM_TYPE_LINK,
	PURPLE_DESKTOP_ITEM_TYPE_FSDEVICE,
	PURPLE_DESKTOP_ITEM_TYPE_MIME_TYPE,
	PURPLE_DESKTOP_ITEM_TYPE_DIRECTORY,
	PURPLE_DESKTOP_ITEM_TYPE_SERVICE,
	PURPLE_DESKTOP_ITEM_TYPE_SERVICE_TYPE
} PurpleDesktopItemType;

typedef struct _PurpleDesktopItem PurpleDesktopItem;

#define PURPLE_TYPE_DESKTOP_ITEM         (purple_desktop_item_get_type ())
GType purple_desktop_item_get_type       (void);

/* standard */
#define PURPLE_DESKTOP_ITEM_ENCODING	"Encoding" /* string */
#define PURPLE_DESKTOP_ITEM_VERSION	"Version"  /* numeric */
#define PURPLE_DESKTOP_ITEM_NAME		"Name" /* localestring */
#define PURPLE_DESKTOP_ITEM_GENERIC_NAME	"GenericName" /* localestring */
#define PURPLE_DESKTOP_ITEM_TYPE		"Type" /* string */
#define PURPLE_DESKTOP_ITEM_FILE_PATTERN "FilePattern" /* regexp(s) */
#define PURPLE_DESKTOP_ITEM_TRY_EXEC	"TryExec" /* string */
#define PURPLE_DESKTOP_ITEM_NO_DISPLAY	"NoDisplay" /* boolean */
#define PURPLE_DESKTOP_ITEM_COMMENT	"Comment" /* localestring */
#define PURPLE_DESKTOP_ITEM_EXEC		"Exec" /* string */
#define PURPLE_DESKTOP_ITEM_ACTIONS	"Actions" /* strings */
#define PURPLE_DESKTOP_ITEM_ICON		"Icon" /* string */
#define PURPLE_DESKTOP_ITEM_MINI_ICON	"MiniIcon" /* string */
#define PURPLE_DESKTOP_ITEM_HIDDEN	"Hidden" /* boolean */
#define PURPLE_DESKTOP_ITEM_PATH		"Path" /* string */
#define PURPLE_DESKTOP_ITEM_TERMINAL	"Terminal" /* boolean */
#define PURPLE_DESKTOP_ITEM_TERMINAL_OPTIONS "TerminalOptions" /* string */
#define PURPLE_DESKTOP_ITEM_SWALLOW_TITLE "SwallowTitle" /* string */
#define PURPLE_DESKTOP_ITEM_SWALLOW_EXEC	"SwallowExec" /* string */
#define PURPLE_DESKTOP_ITEM_MIME_TYPE	"MimeType" /* regexp(s) */
#define PURPLE_DESKTOP_ITEM_PATTERNS	"Patterns" /* regexp(s) */
#define PURPLE_DESKTOP_ITEM_DEFAULT_APP	"DefaultApp" /* string */
#define PURPLE_DESKTOP_ITEM_DEV		"Dev" /* string */
#define PURPLE_DESKTOP_ITEM_FS_TYPE	"FSType" /* string */
#define PURPLE_DESKTOP_ITEM_MOUNT_POINT	"MountPoint" /* string */
#define PURPLE_DESKTOP_ITEM_READ_ONLY	"ReadOnly" /* boolean */
#define PURPLE_DESKTOP_ITEM_UNMOUNT_ICON "UnmountIcon" /* string */
#define PURPLE_DESKTOP_ITEM_SORT_ORDER	"SortOrder" /* strings */
#define PURPLE_DESKTOP_ITEM_URL		"URL" /* string */
#define PURPLE_DESKTOP_ITEM_DOC_PATH	"X-GNOME-DocPath" /* string */

/**
 * This function loads 'filename' and turns it into a PurpleDesktopItem.
 *
 * @param filename The filename or directory path to load the PurpleDesktopItem from
 *
 * @return The newly loaded item, or NULL on error.
 */
PurpleDesktopItem *purple_desktop_item_new_from_file (const char *filename);

/**
 * Gets the type attribute (the 'Type' field) of the item.  This should
 * usually be 'Application' for an application, but it can be 'Directory'
 * for a directory description.  There are other types available as well.
 * The type usually indicates how the desktop item should be handeled and
 * how the 'Exec' field should be handeled.
 *
 * @param item A desktop item
 *
 * @return The type of the specified 'item'. The returned memory
 * remains owned by the PurpleDesktopItem and should not be freed.
 */
PurpleDesktopItemType purple_desktop_item_get_entry_type (const PurpleDesktopItem *item);

/**
 * Gets the value of an attribute of the item, as a string.
 *
 * @param item A desktop item
 * @param attr The attribute to look for
 *
 * @return The value of the specified item attribute.
 */
const char *purple_desktop_item_get_string (const PurpleDesktopItem *item,
					  const char *attr);

/**
 * Creates a copy of a PurpleDesktopItem.  The new copy has a refcount of 1.
 * Note: Section stack is NOT copied.
 *
 * @param item The item to be copied
 *
 * @return The new copy
 */
PurpleDesktopItem *purple_desktop_item_copy (const PurpleDesktopItem *item);

/**
 * Decreases the reference count of the specified item, and destroys
 * the item if there are no more references left.
 *
 * @param item A desktop item
 */
void purple_desktop_item_unref (PurpleDesktopItem *item);

G_END_DECLS

#endif /* _PURPLE_DESKTOP_ITEM_H_ */
