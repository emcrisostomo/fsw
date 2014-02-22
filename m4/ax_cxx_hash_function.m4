# SYNOPSIS
#
#   AX_CXX_HAVE_HASH_FUNCTION()
#
# DESCRIPTION
#
#   Check if the template function std::hash, added in C++11, is defined in the
#   <functional> header.  If it is, define the ac_cv_cxx_have_hash_function
#   environment variable to "yes" and define HAVE_FUNCTIONAL_HASH.
#
#   NOTE: These macros depend on AX_CXX_NAMESPACES.
#
# LICENSE
#
#   Copyright (c) 2014 Enrico M. Crisostomo <enrico.m.crisostomo@gmail.com>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 1

AC_DEFUN([AX_CXX_HAVE_HASH_FUNCTION],
  [AC_CACHE_CHECK(
    [for std::hash in functional],
    ac_cv_cxx_have_hash_function,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
    AC_LANG_PUSH([C++])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [
      [#include <functional>
       #include <string>
       #ifdef HAVE_NAMESPACES
        using namespace std;
       #endif]
    ],
    [
      [hash<string> t;]
    ]
    )],
    [ac_cv_cxx_have_hash_function=yes],
    [ac_cv_cxx_have_hash_function=no]
  )
  AC_LANG_POP([C++])])
  if test x"$ac_cv_cxx_have_hash_function" = "xyes"
  then
    AC_DEFINE(HAVE_FUNCTIONAL_HASH,1,[Define if functional defines std::hash.])
   fi
 ])