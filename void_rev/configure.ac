#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ(2.57)
AC_INIT(src/server.cxx)
#AC_CONFIG_SRCDIR([src/EdgeLoadThread.cxx])
#AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(voidserver,0.0.1.1)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpq':
AC_CHECK_LIB([pq], [PQexec])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gethostbyname inet_ntoa memset select socket])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/commands/Makefile
                 src/commands/dock/Makefile
                 src/handles/Makefile
                 src/socket/Makefile
                 src/thread/Makefile])
AC_OUTPUT
