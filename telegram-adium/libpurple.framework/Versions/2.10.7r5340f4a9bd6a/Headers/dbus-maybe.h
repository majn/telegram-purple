/* This file contains macros that wrap calls to the purple dbus module.
   These macros call the appropriate functions if the build includes
   dbus support and do nothing otherwise.  See "dbus-server.h" for
   documentation.  */

#ifndef _PURPLE_DBUS_MAYBE_H_
#define _PURPLE_DBUS_MAYBE_H_

#ifdef HAVE_DBUS

#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define DBUS_API_SUBJECT_TO_CHANGE
#endif

#include "dbus-server.h"

/* this provides a type check */
#define PURPLE_DBUS_REGISTER_POINTER(ptr, type) { \
    type *typed_ptr = ptr; \
    purple_dbus_register_pointer(typed_ptr, PURPLE_DBUS_TYPE(type));	\
}
#define PURPLE_DBUS_UNREGISTER_POINTER(ptr) purple_dbus_unregister_pointer(ptr)

#else  /* !HAVE_DBUS */

#define PURPLE_DBUS_REGISTER_POINTER(ptr, type) { \
    if (ptr) {} \
}

#define PURPLE_DBUS_UNREGISTER_POINTER(ptr)
#define DBUS_EXPORT

#endif	/* HAVE_DBUS */

#endif
