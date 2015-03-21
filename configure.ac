#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([telegram-purple], [1.0.0])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

m4_include([m4_ax_check_openssl.m4])
m4_include([m4_ax_check_zlib.m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

AC_CHECK_LIB([m], [sqrt])
AC_CHECK_LIB([rt], [clock_gettime])
AX_CHECK_OPENSSL(,[AC_MSG_ERROR([No openssl found])])
AX_CHECK_ZLIB(, [AC_MSG_ERROR([No zlib found])])

PKG_CHECK_MODULES([PURPLE], [purple], )

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strdup])

AC_SUBST(PURPLE_CFLAGS)
AC_SUBST(PURPLE_CPPFLAGS)
AC_SUBST(PURPLE_LIBS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
