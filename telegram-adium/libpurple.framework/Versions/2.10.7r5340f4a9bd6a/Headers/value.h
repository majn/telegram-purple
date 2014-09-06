/**
 * @file value.h Value wrapper API
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
#ifndef _PURPLE_VALUE_H_
#define _PURPLE_VALUE_H_

#include <glib.h>

/**
 * Specific value types.
 */
typedef enum
{
	PURPLE_TYPE_UNKNOWN = 0,  /**< Unknown type.                     */
	PURPLE_TYPE_SUBTYPE,      /**< Subtype.                          */
	PURPLE_TYPE_CHAR,         /**< Character.                        */
	PURPLE_TYPE_UCHAR,        /**< Unsigned character.               */
	PURPLE_TYPE_BOOLEAN,      /**< Boolean.                          */
	PURPLE_TYPE_SHORT,        /**< Short integer.                    */
	PURPLE_TYPE_USHORT,       /**< Unsigned short integer.           */
	PURPLE_TYPE_INT,          /**< Integer.                          */
	PURPLE_TYPE_UINT,         /**< Unsigned integer.                 */
	PURPLE_TYPE_LONG,         /**< Long integer.                     */
	PURPLE_TYPE_ULONG,        /**< Unsigned long integer.            */
	PURPLE_TYPE_INT64,        /**< 64-bit integer.                   */
	PURPLE_TYPE_UINT64,       /**< 64-bit unsigned integer.          */
	PURPLE_TYPE_STRING,       /**< String.                           */
	PURPLE_TYPE_OBJECT,       /**< Object pointer.                   */
	PURPLE_TYPE_POINTER,      /**< Generic pointer.                  */
	PURPLE_TYPE_ENUM,         /**< Enum.                             */
	PURPLE_TYPE_BOXED         /**< Boxed pointer with specific type. */

} PurpleType;


/**
 * Purple-specific subtype values.
 */
typedef enum
{
	PURPLE_SUBTYPE_UNKNOWN = 0,
	PURPLE_SUBTYPE_ACCOUNT,
	PURPLE_SUBTYPE_BLIST,
	PURPLE_SUBTYPE_BLIST_BUDDY,
	PURPLE_SUBTYPE_BLIST_GROUP,
	PURPLE_SUBTYPE_BLIST_CHAT,
	PURPLE_SUBTYPE_BUDDY_ICON,
	PURPLE_SUBTYPE_CONNECTION,
	PURPLE_SUBTYPE_CONVERSATION,
	PURPLE_SUBTYPE_PLUGIN,
	PURPLE_SUBTYPE_BLIST_NODE,
	PURPLE_SUBTYPE_CIPHER,
	PURPLE_SUBTYPE_STATUS,
	PURPLE_SUBTYPE_LOG,
	PURPLE_SUBTYPE_XFER,
	PURPLE_SUBTYPE_SAVEDSTATUS,
	PURPLE_SUBTYPE_XMLNODE,
	PURPLE_SUBTYPE_USERINFO,
	PURPLE_SUBTYPE_STORED_IMAGE,
	PURPLE_SUBTYPE_CERTIFICATEPOOL,
	PURPLE_SUBTYPE_CHATBUDDY
} PurpleSubType;

/**
 * A wrapper for a type, subtype, and specific type of value.
 */
typedef struct
{
	PurpleType type;
	unsigned short flags;

	union
	{
		char char_data;
		unsigned char uchar_data;
		gboolean boolean_data;
		short short_data;
		unsigned short ushort_data;
		int int_data;
		unsigned int uint_data;
		long long_data;
		unsigned long ulong_data;
		gint64 int64_data;
		guint64 uint64_data;
		char *string_data;
		void *object_data;
		void *pointer_data;
		int enum_data;
		void *boxed_data;

	} data;

	union
	{
		unsigned int subtype;
		char *specific_type;

	} u;

} PurpleValue;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a new PurpleValue.
 *
 * This function takes a type and, depending on that type, a sub-type
 * or specific type.
 *
 * If @a type is PURPLE_TYPE_BOXED, the next parameter must be a
 * string representing the specific type.
 *
 * If @a type is PURPLE_TYPE_SUBTYPE, the next parameter must be a
 * integer or enum representing the sub-type.
 *
 * If the subtype or specific type is not set when required, random
 * errors may occur. You have been warned.
 *
 * @param type The type.
 *
 * @return The new value.
 */
PurpleValue *purple_value_new(PurpleType type, ...);

/**
 * Creates a new outgoing PurpleValue.  If a value is an "outgoing" value
 * it means the value can be modified by plugins and scripts.
 *
 * This function takes a type and, depending on that type, a sub-type
 * or specific type.
 *
 * If @a type is PURPLE_TYPE_BOXED, the next parameter must be a
 * string representing the specific type.
 *
 * If @a type is PURPLE_TYPE_SUBTYPE, the next parameter must be a
 * integer or enum representing the sub-type.
 *
 * If the sub-type or specific type is not set when required, random
 * errors may occur. You have been warned.
 *
 * @param type The type.
 *
 * @return The new value.
 */
PurpleValue *purple_value_new_outgoing(PurpleType type, ...);

/**
 * Destroys a PurpleValue.
 *
 * @param value The value to destroy.
 */
void purple_value_destroy(PurpleValue *value);

/**
 * Duplicated a PurpleValue.
 *
 * @param value The value to duplicate.
 *
 * @return The duplicate value.
 */
PurpleValue *purple_value_dup(const PurpleValue *value);

/**
 * Returns a value's type.
 *
 * @param value The value whose type you want.
 *
 * @return The value's type.
 */
PurpleType purple_value_get_type(const PurpleValue *value);

/**
 * Returns a value's subtype.
 *
 * If the value's type is not PURPLE_TYPE_SUBTYPE, this will return 0.
 * Subtypes should never have a subtype of 0.
 *
 * @param value The value whose subtype you want.
 *
 * @return The value's subtype, or 0 if @a type is not PURPLE_TYPE_SUBTYPE.
 */
unsigned int purple_value_get_subtype(const PurpleValue *value);

/**
 * Returns a value's specific type.
 *
 * If the value's type is not PURPLE_TYPE_BOXED, this will return @c NULL.
 *
 * @param value The value whose specific type you want.
 *
 * @return The value's specific type, or @a NULL if not PURPLE_TYPE_BOXED.
 */
const char *purple_value_get_specific_type(const PurpleValue *value);

/**
 * Returns whether or not the value is an outgoing value.
 *
 * @param value The value.
 *
 * @return TRUE if the value is outgoing, or FALSE otherwise.
 */
gboolean purple_value_is_outgoing(const PurpleValue *value);

/**
 * Sets the value's character data.
 *
 * @param value The value.
 * @param data The character data.
 */
void purple_value_set_char(PurpleValue *value, char data);

/**
 * Sets the value's unsigned character data.
 *
 * @param value The value.
 * @param data The unsigned character data.
 */
void purple_value_set_uchar(PurpleValue *value, unsigned char data);

/**
 * Sets the value's boolean data.
 *
 * @param value The value.
 * @param data The boolean data.
 */
void purple_value_set_boolean(PurpleValue *value, gboolean data);

/**
 * Sets the value's short integer data.
 *
 * @param value The value.
 * @param data The short integer data.
 */
void purple_value_set_short(PurpleValue *value, short data);

/**
 * Sets the value's unsigned short integer data.
 *
 * @param value The value.
 * @param data The unsigned short integer data.
 */
void purple_value_set_ushort(PurpleValue *value, unsigned short data);

/**
 * Sets the value's integer data.
 *
 * @param value The value.
 * @param data The integer data.
 */
void purple_value_set_int(PurpleValue *value, int data);

/**
 * Sets the value's unsigned integer data.
 *
 * @param value The value.
 * @param data The unsigned integer data.
 */
void purple_value_set_uint(PurpleValue *value, unsigned int data);

/**
 * Sets the value's long integer data.
 *
 * @param value The value.
 * @param data The long integer data.
 */
void purple_value_set_long(PurpleValue *value, long data);

/**
 * Sets the value's unsigned long integer data.
 *
 * @param value The value.
 * @param data The unsigned long integer data.
 */
void purple_value_set_ulong(PurpleValue *value, unsigned long data);

/**
 * Sets the value's 64-bit integer data.
 *
 * @param value The value.
 * @param data The 64-bit integer data.
 */
void purple_value_set_int64(PurpleValue *value, gint64 data);

/**
 * Sets the value's unsigned 64-bit integer data.
 *
 * @param value The value.
 * @param data The unsigned 64-bit integer data.
 */
void purple_value_set_uint64(PurpleValue *value, guint64 data);

/**
 * Sets the value's string data.
 *
 * @param value The value.
 * @param data The string data.
 */
void purple_value_set_string(PurpleValue *value, const char *data);

/**
 * Sets the value's object data.
 *
 * @param value The value.
 * @param data The object data.
 */
void purple_value_set_object(PurpleValue *value, void *data);

/**
 * Sets the value's pointer data.
 *
 * @param value The value.
 * @param data The pointer data.
 */
void purple_value_set_pointer(PurpleValue *value, void *data);

/**
 * Sets the value's enum data.
 *
 * @param value The value.
 * @param data The enum data.
 */
void purple_value_set_enum(PurpleValue *value, int data);

/**
 * Sets the value's boxed data.
 *
 * @param value The value.
 * @param data The boxed data.
 */
void purple_value_set_boxed(PurpleValue *value, void *data);

/**
 * Returns the value's character data.
 *
 * @param value The value.
 *
 * @return The character data.
 */
char purple_value_get_char(const PurpleValue *value);

/**
 * Returns the value's unsigned character data.
 *
 * @param value The value.
 *
 * @return The unsigned character data.
 */
unsigned char purple_value_get_uchar(const PurpleValue *value);

/**
 * Returns the value's boolean data.
 *
 * @param value The value.
 *
 * @return The boolean data.
 */
gboolean purple_value_get_boolean(const PurpleValue *value);

/**
 * Returns the value's short integer data.
 *
 * @param value The value.
 *
 * @return The short integer data.
 */
short purple_value_get_short(const PurpleValue *value);

/**
 * Returns the value's unsigned short integer data.
 *
 * @param value The value.
 *
 * @return The unsigned short integer data.
 */
unsigned short purple_value_get_ushort(const PurpleValue *value);

/**
 * Returns the value's integer data.
 *
 * @param value The value.
 *
 * @return The integer data.
 */
int purple_value_get_int(const PurpleValue *value);

/**
 * Returns the value's unsigned integer data.
 *
 * @param value The value.
 *
 * @return The unsigned integer data.
 */
unsigned int purple_value_get_uint(const PurpleValue *value);

/**
 * Returns the value's long integer data.
 *
 * @param value The value.
 *
 * @return The long integer data.
 */
long purple_value_get_long(const PurpleValue *value);

/**
 * Returns the value's unsigned long integer data.
 *
 * @param value The value.
 *
 * @return The unsigned long integer data.
 */
unsigned long purple_value_get_ulong(const PurpleValue *value);

/**
 * Returns the value's 64-bit integer data.
 *
 * @param value The value.
 *
 * @return The 64-bit integer data.
 */
gint64 purple_value_get_int64(const PurpleValue *value);

/**
 * Returns the value's unsigned 64-bit integer data.
 *
 * @param value The value.
 *
 * @return The unsigned 64-bit integer data.
 */
guint64 purple_value_get_uint64(const PurpleValue *value);

/**
 * Returns the value's string data.
 *
 * @param value The value.
 *
 * @return The string data.
 */
const char *purple_value_get_string(const PurpleValue *value);

/**
 * Returns the value's object data.
 *
 * @param value The value.
 *
 * @return The object data.
 */
void *purple_value_get_object(const PurpleValue *value);

/**
 * Returns the value's pointer data.
 *
 * @param value The value.
 *
 * @return The pointer data.
 */
void *purple_value_get_pointer(const PurpleValue *value);

/**
 * Returns the value's enum data.
 *
 * @param value The value.
 *
 * @return The enum data.
 */
int purple_value_get_enum(const PurpleValue *value);

/**
 * Returns the value's boxed data.
 *
 * @param value The value.
 *
 * @return The boxed data.
 */
void *purple_value_get_boxed(const PurpleValue *value);

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_VALUE_H_ */
