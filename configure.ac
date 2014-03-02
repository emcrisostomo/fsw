#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([fsw], [1.3.1], [enrico.m.crisostomo@gmail.com])
AC_COPYRIGHT([2013 (C) Enrico M. Crisostomo])
AC_REVISION([$Revision: 1.3.1-1 $])
AC_CONFIG_SRCDIR([fsw.cpp])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall -Werror gnu])

# Configure language
AC_LANG(C++)

# Checks for programs.
AC_PROG_CC([clang gcc])
AC_PROG_CXX([clang++ g++])

# Configure C++ compiler
AX_CXX_COMPILE_STDCXX_11(noext)
AX_CXXFLAGS_WARN_ALL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h getopt.h fcntl.h sys/event.h sys/time.h])
AC_CHECK_HEADERS([CoreServices/CoreServices.h])
AC_CHECK_HEADERS([unordered_map unordered_set])

AM_CONDITIONAL([USE_CORESERVICES], [test "x${ac_cv_header_CoreServices_CoreServices_h}" = "xyes"])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_MODE_T

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([getopt_long])
AC_CHECK_FUNCS(
  [realpath],
  ,
  AC_MSG_ERROR([The realpath function cannot be found.])
)
AC_CHECK_FUNCS(
  [modf],
  ,
  AC_MSG_ERROR([The modf function cannot be found.]) 
)
AC_CHECK_FUNCS([regcomp])

if test "x${ac_cv_header_CoreServices_CoreServices_h}" != "xyes"
then
  AC_CHECK_DECLS(
    [kqueue, kevent],
    ,
    AC_MSG_ERROR([kqueue support cannot be found.]),
    [
      AC_INCLUDES_DEFAULT
      [#include <sys/event.h>]
      [#include <sys/types.h>]
      [#include <sys/time.h>]
    ]
  )
fi

AC_OUTPUT
