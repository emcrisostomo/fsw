#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([fsw], [1.0.5], [enrico.m.crisostomo@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror gnu])
AC_COPYRIGHT([2013 (C) Enrico M. Crisostomo])
AC_REVISION([$Revision: 1.0.5-1 $])
AC_CONFIG_SRCDIR([fsw.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIRS([m4])
AC_CANONICAL_HOST

case "${host_os}" in
  darwin1[[0-3]].*)
    ;;
  *)
    AC_MSG_ERROR([Unsupported OS: ${host} - ${host_os}])
    ;;
esac
    
# Configure language
AC_LANG(C++)

# Configure C++ compiler
AX_CXX_COMPILE_STDCXX_11
AX_CXXFLAGS_WARN_ALL

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h getopt.h CoreServices/CoreServices.h])

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

AC_OUTPUT
