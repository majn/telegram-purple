# This file is part of telegram-purple
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
#
# Copyright Matthias Jentsch, Vitaly Valtman, Christopher Althaus, Markus Endres 2014-2015

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([telegram-purple], [1.4.2])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

m4_include([m4_ax_check_openssl.m4])
m4_include([m4_ax_check_zlib.m4])
m4_include([m4_ax_pkg_config.m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

AC_TRY_COMPILE([], [
#ifdef NDEBUG
#error "NDEBUG detected"
#endif], [],
[AC_MSG_ERROR([The macro NDEBUG cannot work.  All writes and reads are wrapped in asserts.
For more info, see https://github.com/majn/telegram-purple/issues/429])])

AC_CHECK_LIB([rt], [clock_gettime])

AC_ARG_ENABLE([gcrypt],
  AS_HELP_STRING([--disable-gcrypt], [Disable gcrypt, use OpenSSL instead. DO NOT DISTRIBUTE BINARIES!]))

AS_IF([test "x$enable_gcrypt" != "xno"], [
  # Even though tgl/configure will do this too, still call our own libgcrypt check to assure that -lgcrypt is added to our makefile
  AC_CHECK_LIB([gcrypt], [gcry_mpi_snatch], [], [
    AC_MSG_ERROR([no libgcrypt >= 1.60 found.
    If its not available try --disable-gcrypt to link against openssl, but in that case
    YOU MAY NOT REDISTRIBUTE THE BINARY due to licensing restrictions.])
  ])
  AC_SUBST([CRYPTO_FLAG], [--disable-openssl])
], [
  AX_CHECK_OPENSSL([], [No openssl found.])
])


AX_CHECK_ZLIB(, [AC_MSG_ERROR([No zlib found])])

PKG_CHECK_MODULES([PURPLE], [purple])

AC_MSG_CHECKING([for libwebp])
AC_ARG_ENABLE([libwebp],
  AS_HELP_STRING([--disable-libwebp], [Disable libwebp, stickers won't be displayed in the chat]))

AS_IF([test "x$enable_libwebp" != "xno"], [
    AC_MSG_RESULT([enabled])
    AC_CHECK_LIB([webp], [WebPDecodeRGBA], [], [
        AC_SEARCH_LIBS([WebPDecodeRGBA], [webp-7], [], [AC_MSG_ERROR([no libwebp found, try --disable-libwebp, but stickers won't be displayed in the chat])])
        # If we get here, then it's one of the special-case scenarios.
        # So we *do* want to set 'HAVE_LIBWEBP' to 1.  But because the primary check
        # failed, it would now go to "undefined".
        # Therefore, override it:
        AC_DEFINE(HAVE_LIBWEBP, 1)
    ])
  ])

AC_MSG_CHECKING([for libpng])
AC_ARG_ENABLE([libpng],
  AS_HELP_STRING([--disable-libpng], [Disable libpng, stickers won't be converted]))

AS_IF([test "x$enable_libpng" != "xno"], [
    AC_MSG_RESULT([enabled])
    AC_CHECK_LIB([png], [png_write_png], [], [AC_MSG_ERROR([no libpng found, try --disable-libpng])])
  ])

AC_ARG_ENABLE([icons],
  AS_HELP_STRING([--disable-icons], [Unsupported option. Use 'make noicon_install' instead.]))
AS_IF([test "x$enable_icons" == "xno"], [
  AC_MSG_ERROR([--disable-icons is now obsolete. Please use 'make noicon_install' for the same effect.])
])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([malloc memset strdup])

AC_SUBST(PURPLE_CFLAGS)
AC_SUBST(PURPLE_CPPFLAGS)
AC_SUBST(PURPLE_LIBS)

AC_ARG_ENABLE([translation],
  AS_HELP_STRING([--disable-translation], [Disable gettext translation]))
  AS_IF([test "x$enable_translation" != "xno"], [
    AC_CHECK_PROG(CHECK_GETTEXT, msgfmt, "yes", "no", $PATH)
    if test x"$CHECK_GETTEXT" != x"yes" ; then
      AC_MSG_ERROR([no gettext found, use --disable-translation if you dont need it.])
    fi
    AC_SUBST(MSGFMT_PATH, $(which msgfmt))
  ])

# Define the domain name for the translated words 
GETTEXT_PACKAGE=$PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)  
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Define the gettext package to be used])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
