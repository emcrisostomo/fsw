#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([fsw], [1.1.0], [enrico.m.crisostomo@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror gnu])
AC_COPYRIGHT([2013 (C) Enrico M. Crisostomo])
AC_REVISION([$Revision: 1.1.0-1 $])
AC_CONFIG_SRCDIR([fsw.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIRS([m4])
AC_CANONICAL_HOST

# Configure language
AC_LANG(C++)

# Checks for programs.
AC_PROG_CXX([clang++ g++])

# Configure C++ compiler
AX_CXX_COMPILE_STDCXX_11
AX_CXXFLAGS_WARN_ALL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h getopt.h fcntl.h CoreServices/CoreServices.h sys/event.h])
AM_CONDITIONAL([USE_CORESERVICES], [test "x${ac_cv_header_CoreServices_CoreServices_h}" = "xyes"])

if test "x${ac_cv_header_sys_event_h}" != "xyes" && test "x${ac_cv_header_CoreServices_CoreServices_h}" != "xyes"
then
  AC_MSG_ERROR([Either sys/event.h or CoreServices/CoreServices.h must be available.])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS(
  [getopt_long],
  ,
  AC_MSG_ERROR([getopt_long function cannot be found.]) 
)

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
