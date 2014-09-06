/**
 * @file version.h Purple Versioning
 *
 * purple
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
#ifndef _PURPLE_VERSION_H_
#define _PURPLE_VERSION_H_

/** The major version of the running libpurple. */
#define PURPLE_MAJOR_VERSION (2)
/** The minor version of the running libpurple. */
#define PURPLE_MINOR_VERSION (10)
/** The micro version of the running libpurple. */
#define PURPLE_MICRO_VERSION (7)

#define PURPLE_VERSION_CHECK(x,y,z) ((x) == PURPLE_MAJOR_VERSION && \
									 ((y) < PURPLE_MINOR_VERSION || \
									  ((y) == PURPLE_MINOR_VERSION && (z) <= PURPLE_MICRO_VERSION)))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Checks that the libpurple version is compatible with the requested
 * version
 *
 * @param required_major: the required major version.
 * @param required_minor: the required minor version.
 * @param required_micro: the required micro version.
 *
 * @return NULL if the versions are compatible, or a string describing
 *         the version mismatch if not compatible.
 */
const char *purple_version_check(guint required_major, guint required_minor, guint required_micro);

/**
 * The major version of the running libpurple.  Contrast with
 * #PURPLE_MAJOR_VERSION, which expands at compile time to the major version of
 * libpurple being compiled against.
 *
 * @since 2.4.0
 */
extern const guint purple_major_version;

/**
 * The minor version of the running libpurple.  Contrast with
 * #PURPLE_MINOR_VERSION, which expands at compile time to the minor version of
 * libpurple being compiled against.
 *
 * @since 2.4.0
 */
extern const guint purple_minor_version;

/**
 *
 * The micro version of the running libpurple.  Contrast with
 * #PURPLE_MICRO_VERSION, which expands at compile time to the micro version of
 * libpurple being compiled against.
 *
 * @since 2.4.0
 */
extern const guint purple_micro_version;

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_VERSION_H_ */

