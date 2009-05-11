#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([gurls], [dev], [jdegges@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/download.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_CXX

# Checks for libraries with pkg-config
PKG_CHECK_MODULES(PACKAGE, "libcurl")
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

# Checks for librairies.
AC_HAVE_LIBRARY([curl])
AC_HAVE_LIBRARY([pthread])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([curl/curl.h pthread.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset regcomp strtoul])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT