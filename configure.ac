#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([gurls], [dev], [jdegges@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
PKG_PROG_PKG_CONFIG([0.22])

# Checks for programs.
AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries with pkg-config
PKG_CHECK_MODULES([LIBCURL], [libcurl])
PKG_CHECK_MODULES([LOOMLIB], [loomlib])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([curl/curl.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([regcomp strtoul])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
