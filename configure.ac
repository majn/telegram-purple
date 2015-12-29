#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([telegram-purple], [1.2.3])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

m4_include([m4_ax_check_zlib.m4])
m4_include([m4_ax_pkg_config.m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

AC_CHECK_LIB([rt], [clock_gettime])
AC_CHECK_LIB([gcrypt], [gcry_mpi_snatch], [], [
  echo "no libgcrypt >= 1.60 found"
  exit -1
])
AX_CHECK_ZLIB(, [AC_MSG_ERROR([No zlib found])])

PKG_CHECK_MODULES([PURPLE], [purple])

AC_MSG_CHECKING([for libwebp])
AC_ARG_ENABLE([libwebp],
  AS_HELP_STRING([--disable-libwebp], [Disable libwebp, stickers won't be displayed in the chat]))
  AS_IF([test "x$enable_libwebp" != "xno"], [
    AC_MSG_RESULT([enabled])
    AC_CHECK_LIB([webp], [WebPDecodeRGBA], [], [AC_MSG_ERROR([no libwebp found, try --disable-libwebp, but stickers won't be displayed in the chat])])
  ])

AC_SUBST([COPY_ICONS], [yes])
AC_ARG_ENABLE([icons],
  AS_HELP_STRING([--disable-icons], [Don't copy the protocol icons for Pidgin. Only useful if you're using Finch of telepathy-haze. (Adium users shouldn't be using this build system at all.)]))
  AS_IF([test "x$enable_icons" == "xno"], [
    AC_WARN([Icons are deactivated. Pidgin needs icons. Finch and telepathy-haze don't.])
    AC_SUBST([COPY_ICONS], [no])
  ])

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

AC_CHECK_PROG(GETTEXT_CHECK,msgfmt,yes)
if test x"$GETTEXT_CHECK" != x"yes" ; then
    AC_MSG_ERROR([msgfmt not found please install gettext])
fi

# Define the domain name for the translated words 
GETTEXT_PACKAGE=$PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)  
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Define the gettext package to be used])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
