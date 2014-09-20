AC_PREREQ([2.68])
AC_INIT([telegram-purple], [0.2])

# Checks for programs.
AC_PROG_CC

# BSD locations for headers and libraries from packages, Linux locations for self-compiled stuff.
CPPFLAGS="$CPPFLAGS -I/usr/local/include"
LDFLAGS="$LDFLAGS -L/usr/local/lib"

# Checks for libraries.
AC_CHECK_LIB([m], [sqrt])
AC_SEARCH_LIBS([clock_gettime], [rt])
AC_SEARCH_LIBS([backtrace], [execinfo])
AC_CHECK_LIB([z], [inflate])
AC_CHECK_LIB([crypto], [AES_set_encrypt_key])

EXTRA_LIBS=""

#check for custom prog name
AC_ARG_WITH(progname,[--with-progname=<name>],
  [ 
  if test "x$with_progname" = "xno" ; then
    AC_MSG_RESULT([default])
  elif test "x$with_progname" = "xyes" ; then
    AC_MSG_RESULT([default])
  else
    AC_MSG_RESULT(["$with_progname"])
    AC_DEFINE_UNQUOTED([PROG_NAME], ["$with_progname"], [Use custom prog name])
  fi
  ],[
    AC_MSG_RESULT([default])
  ])
    
# Checks for header files.
AC_CHECK_HEADERS([execinfo.h fcntl.h malloc.h netdb.h stdlib.h string.h unistd.h arpa/inet.h mach/mach.h netinet/in.h sys/file.h sys/socket.h termios.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([alarm endpwent memset memmove mkdir select socket strdup strndup uname])

AC_SUBST(EXTRA_LIBS)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

