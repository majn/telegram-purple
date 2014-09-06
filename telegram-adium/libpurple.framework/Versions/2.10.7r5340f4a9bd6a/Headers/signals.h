/**
 * @file signals.h Signal API
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
#ifndef _PURPLE_SIGNALS_H_
#define _PURPLE_SIGNALS_H_

#include <glib.h>
#include "value.h"

#define PURPLE_CALLBACK(func) ((PurpleCallback)func)

typedef void (*PurpleCallback)(void);
typedef void (*PurpleSignalMarshalFunc)(PurpleCallback cb, va_list args,
									  void *data, void **return_val);

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/** @name Signal API                                                      */
/**************************************************************************/
/*@{*/

/** The priority of a signal connected using purple_signal_connect().
 *
 *  @see purple_signal_connect_priority()
 */
#define PURPLE_SIGNAL_PRIORITY_DEFAULT     0
/** The largest signal priority; signals with this priority will be called
 *  <em>last</em>.  (This is highest as in numerical value, not as in order of
 *  importance.)
 *
 *  @see purple_signal_connect_priority().
 */
#define PURPLE_SIGNAL_PRIORITY_HIGHEST  9999
/** The smallest signal priority; signals with this priority will be called
 *  <em>first</em>.  (This is lowest as in numerical value, not as in order of
 *  importance.)
 *
 *  @see purple_signal_connect_priority().
 */
#define PURPLE_SIGNAL_PRIORITY_LOWEST  -9999

/**
 * Registers a signal in an instance.
 *
 * @param instance   The instance to register the signal for.
 * @param signal     The signal name.
 * @param marshal    The marshal function.
 * @param ret_value  The return value type, or NULL for no return value.
 * @param num_values The number of values to be passed to the callbacks.
 * @param ...        The values to pass to the callbacks.
 *
 * @return The signal ID local to that instance, or 0 if the signal
 *         couldn't be registered.
 *
 * @see PurpleValue
 */
gulong purple_signal_register(void *instance, const char *signal,
							PurpleSignalMarshalFunc marshal,
							PurpleValue *ret_value, int num_values, ...);

/**
 * Unregisters a signal in an instance.
 *
 * @param instance The instance to unregister the signal for.
 * @param signal   The signal name.
 */
void purple_signal_unregister(void *instance, const char *signal);

/**
 * Unregisters all signals in an instance.
 *
 * @param instance The instance to unregister the signal for.
 */
void purple_signals_unregister_by_instance(void *instance);

/**
 * Returns a list of value types used for a signal.
 *
 * @param instance   The instance the signal is registered to.
 * @param signal     The signal.
 * @param ret_value  The return value from the last signal handler.
 * @param num_values The returned number of values.
 * @param values     The returned list of values.
 */
void purple_signal_get_values(void *instance, const char *signal,
							PurpleValue **ret_value,
							int *num_values, PurpleValue ***values);

/**
 * Connects a signal handler to a signal for a particular object.
 *
 * Take care not to register a handler function twice. Purple will
 * not correct any mistakes for you in this area.
 *
 * @param instance The instance to connect to.
 * @param signal   The name of the signal to connect.
 * @param handle   The handle of the receiver.
 * @param func     The callback function.
 * @param data     The data to pass to the callback function.
 * @param priority The priority with which the handler should be called. Signal
 *                 handlers are called in ascending numerical order of @a
 *                 priority from #PURPLE_SIGNAL_PRIORITY_LOWEST to
 *                 #PURPLE_SIGNAL_PRIORITY_HIGHEST.
 *
 * @return The signal handler ID.
 *
 * @see purple_signal_disconnect()
 */
gulong purple_signal_connect_priority(void *instance, const char *signal,
	void *handle, PurpleCallback func, void *data, int priority);

/**
 * Connects a signal handler to a signal for a particular object.
 * (Its priority defaults to 0, aka #PURPLE_SIGNAL_PRIORITY_DEFAULT.)
 *
 * Take care not to register a handler function twice. Purple will
 * not correct any mistakes for you in this area.
 *
 * @param instance The instance to connect to.
 * @param signal   The name of the signal to connect.
 * @param handle   The handle of the receiver.
 * @param func     The callback function.
 * @param data     The data to pass to the callback function.
 *
 * @return The signal handler ID.
 *
 * @see purple_signal_disconnect()
 */
gulong purple_signal_connect(void *instance, const char *signal,
	void *handle, PurpleCallback func, void *data);

/**
 * Connects a signal handler to a signal for a particular object.
 *
 * The signal handler will take a va_args of arguments, instead of
 * individual arguments.
 *
 * Take care not to register a handler function twice. Purple will
 * not correct any mistakes for you in this area.
 *
 * @param instance The instance to connect to.
 * @param signal   The name of the signal to connect.
 * @param handle   The handle of the receiver.
 * @param func     The callback function.
 * @param data     The data to pass to the callback function.
 * @param priority The priority with which the handler should be called. Signal
 *                 handlers are called in ascending numerical order of @a
 *                 priority from #PURPLE_SIGNAL_PRIORITY_LOWEST to
 *                 #PURPLE_SIGNAL_PRIORITY_HIGHEST.
 *
 * @return The signal handler ID.
 *
 * @see purple_signal_disconnect()
 */
gulong purple_signal_connect_priority_vargs(void *instance, const char *signal,
	void *handle, PurpleCallback func, void *data, int priority);

/**
 * Connects a signal handler to a signal for a particular object.
 * (Its priority defaults to 0, aka #PURPLE_SIGNAL_PRIORITY_DEFAULT.)
 *
 * The signal handler will take a va_args of arguments, instead of
 * individual arguments.
 *
 * Take care not to register a handler function twice. Purple will
 * not correct any mistakes for you in this area.
 *
 * @param instance The instance to connect to.
 * @param signal   The name of the signal to connect.
 * @param handle   The handle of the receiver.
 * @param func     The callback function.
 * @param data     The data to pass to the callback function.
 *
 * @return The signal handler ID.
 *
 * @see purple_signal_disconnect()
 */
gulong purple_signal_connect_vargs(void *instance, const char *signal,
	void *handle, PurpleCallback func, void *data);

/**
 * Disconnects a signal handler from a signal on an object.
 *
 * @param instance The instance to disconnect from.
 * @param signal   The name of the signal to disconnect.
 * @param handle   The handle of the receiver.
 * @param func     The registered function to disconnect.
 *
 * @see purple_signal_connect()
 */
void purple_signal_disconnect(void *instance, const char *signal,
							void *handle, PurpleCallback func);

/**
 * Removes all callbacks associated with a receiver handle.
 *
 * @param handle The receiver handle.
 */
void purple_signals_disconnect_by_handle(void *handle);

/**
 * Emits a signal.
 *
 * @param instance The instance emitting the signal.
 * @param signal   The signal being emitted.
 *
 * @see purple_signal_connect()
 * @see purple_signal_disconnect()
 */
void purple_signal_emit(void *instance, const char *signal, ...);

/**
 * Emits a signal, using a va_list of arguments.
 *
 * @param instance The instance emitting the signal.
 * @param signal   The signal being emitted.
 * @param args     The arguments list.
 *
 * @see purple_signal_connect()
 * @see purple_signal_disconnect()
 */
void purple_signal_emit_vargs(void *instance, const char *signal, va_list args);

/**
 * Emits a signal and returns the first non-NULL return value.
 *
 * Further signal handlers are NOT called after a handler returns
 * something other than NULL.
 *
 * @param instance The instance emitting the signal.
 * @param signal   The signal being emitted.
 *
 * @return The first non-NULL return value
 */
void *purple_signal_emit_return_1(void *instance, const char *signal, ...);

/**
 * Emits a signal and returns the first non-NULL return value.
 *
 * Further signal handlers are NOT called after a handler returns
 * something other than NULL.
 *
 * @param instance The instance emitting the signal.
 * @param signal   The signal being emitted.
 * @param args     The arguments list.
 *
 * @return The first non-NULL return value
 */
void *purple_signal_emit_vargs_return_1(void *instance, const char *signal,
									  va_list args);

/**
 * Initializes the signals subsystem.
 */
void purple_signals_init(void);

/**
 * Uninitializes the signals subsystem.
 */
void purple_signals_uninit(void);

/*@}*/

/**************************************************************************/
/** @name Marshal Functions                                               */
/**************************************************************************/
/*@{*/

void purple_marshal_VOID(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__INT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__INT_INT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_UINT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_INT_INT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_INT_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_POINTER_UINT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_POINTER_UINT_UINT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_POINTER_POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_POINTER_POINTER_POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_POINTER_POINTER_UINT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_POINTER_POINTER_POINTER_UINT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_VOID__POINTER_POINTER_POINTER_UINT_UINT(
		PurpleCallback cb, va_list args, void *data, void **return_val);

void purple_marshal_INT__INT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_INT__INT_INT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_INT__POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_INT__POINTER_POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_INT__POINTER_POINTER_POINTER_POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);

void purple_marshal_BOOLEAN__POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_BOOLEAN__POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_BOOLEAN__POINTER_BOOLEAN(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_BOOLEAN__POINTER_POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_BOOLEAN__POINTER_POINTER_UINT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_BOOLEAN__POINTER_POINTER_POINTER_UINT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_BOOLEAN__POINTER_POINTER_POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_BOOLEAN__POINTER_POINTER_POINTER_POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_BOOLEAN__POINTER_POINTER_POINTER_POINTER_UINT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_BOOLEAN__POINTER_POINTER_POINTER_POINTER_POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);

void purple_marshal_BOOLEAN__INT_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);

void purple_marshal_POINTER__POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_POINTER__POINTER_INT(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_POINTER__POINTER_INT64(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_POINTER__POINTER_INT_BOOLEAN(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_POINTER__POINTER_INT64_BOOLEAN(
		PurpleCallback cb, va_list args, void *data, void **return_val);
void purple_marshal_POINTER__POINTER_POINTER(
		PurpleCallback cb, va_list args, void *data, void **return_val);
/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_SIGNALS_H_ */
