# SYNOPSIS
#
#   AX_CXX_HAVE_HASH_CLASS()
#   AX_CXX_HAVE_FUNCTION_CLASS()
#   AX_CXX_HAVE_REFERENCE_WRAPPER_CLASS()
#   AX_CXX_HAVE_BIT_AND_CLASS()
#   AX_CXX_HAVE_BIT_OR_CLASS()
#   AX_CXX_HAVE_BIT_XOR_CLASS()
#   AX_CXX_HAVE_BAD_FUNCTION_CALL_CLASS()
#   AX_CXX_HAVE_IS_BIND_EXPRESSION_CLASS()
#   AX_CXX_HAVE_IS_PLACEHOLDER_CLASS()
#   AX_CXX_HAVE_BIND_FUNCTION()
#   AX_CXX_HAVE_CREF_FUNCTION()
#   AX_CXX_HAVE_MEM_FN_FUNCTION()
#   AX_CXX_HAVE_REF_FUNCTION()
#
# DESCRIPTION
#
#   These macros check if the following template classes and functions, added in
#   C++11, are defined in the <functional> header.
#     * std::hash class.
#     * std::function class.
#     * std::reference_wrapper class.
#     * std::bit_and class.
#     * std::bit_or class.
#     * std::bit_xor class.
#     * std::bad_function_call class.
#     * std::is_bind_expression class.
#     * std::is_placeholder class.
#     * std::bind function.
#     * std::cref function.
#     * std::mem_fn function.
#     * std::ref function.
#
#   If they are, define the ac_cv_cxx_have_[name]_[class|function] environment
#   variable to "yes" and define HAVE_FUNCTIONAL_[NAME]_[CLASS|FUNCTION].
#
#   NOTE: These macros depend on AX_CXX_NAMESPACES.
#
# LICENSE
#
#   Copyright (c) 2014 Enrico M. Crisostomo <enrico.m.crisostomo@gmail.com>
#
#   Copying and distribution of this file, with or without modification,
#   are permitted in any medium without royalty provided the copyright
#   notice and this notice are preserved.  This file is offered as-is,
#   without any warranty.

#serial 1

AC_DEFUN([AX_CXX_HAVE_HASH_CLASS],
  [AC_CACHE_CHECK(
    [for std::hash in functional],
    ac_cv_cxx_have_hash_class,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>
           #include <string>]
          [using std::hash;]
        ],
        []
        )],
        [ac_cv_cxx_have_hash_class=yes],
        [ac_cv_cxx_have_hash_class=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_hash_class" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_HASH_CLASS,1,[Define if functional defines the std::hash class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_FUNCTION_CLASS],
  [AC_CACHE_CHECK(
    [for std::function in functional],
    ac_cv_cxx_have_function_class,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::function;]
        ],
        []
        )],
        [ac_cv_cxx_have_function_class=yes],
        [ac_cv_cxx_have_function_class=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_function_class" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_FUNCTION_CLASS,1,[Define if functional defines the std::function class.])
    fi
  ])
  
AC_DEFUN([AX_CXX_HAVE_REFERENCE_WRAPPER_CLASS],
  [AC_CACHE_CHECK(
    [for std::reference_wrapper in functional],
    ac_cv_cxx_have_reference_wrapper_class,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::reference_wrapper;]
        ],
        []
        )],
        [ac_cv_cxx_have_reference_wrapper_class=yes],
        [ac_cv_cxx_have_reference_wrapper_class=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_reference_wrapper_class" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_REFERENCE_WRAPPER_CLASS,1,[Define if functional defines the std::reference_wrapper class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_BIT_AND_CLASS],
  [AC_CACHE_CHECK(
    [for std::bit_and in functional],
    ac_cv_cxx_have_bit_and_class,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::bit_and;]
        ],
        []
        )],
        [ac_cv_cxx_have_bit_and_class=yes],
        [ac_cv_cxx_have_bit_and_class=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_bit_and_class" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_BIT_AND_CLASS,1,[Define if functional defines the std::bit_and class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_BIT_OR_CLASS],
  [AC_CACHE_CHECK(
    [for std::bit_or in functional],
    ac_cv_cxx_have_bit_or_class,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::bit_or;]
        ],
        []
        )],
        [ac_cv_cxx_have_bit_or_class=yes],
        [ac_cv_cxx_have_bit_or_class=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_bit_or_class" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_BIT_OR_CLASS,1,[Define if functional defines the std::bit_or class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_BIT_XOR_CLASS],
  [AC_CACHE_CHECK(
    [for std::bit_xor in functional],
    ac_cv_cxx_have_bit_xor_class,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::bit_xor;]
        ],
        []
        )],
        [ac_cv_cxx_have_bit_xor_class=yes],
        [ac_cv_cxx_have_bit_xor_class=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_bit_xor_class" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_BIT_XOR_CLASS,1,[Define if functional defines the std::bit_xor class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_BAD_FUNCTION_CALL_CLASS],
  [AC_CACHE_CHECK(
    [for std::bad_function_call in functional],
    ac_cv_cxx_have_bad_function_call_class,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::bad_function_call;]
        ],
        []
        )],
        [ac_cv_cxx_have_bad_function_call_class=yes],
        [ac_cv_cxx_have_bad_function_call_class=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_bad_function_call_class" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_BAD_FUNCTION_CALL_CLASS,1,[Define if functional defines the std::bad_function_call class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_IS_BIND_EXPRESSION_CLASS],
  [AC_CACHE_CHECK(
    [for std::is_bind_expression in functional],
    ac_cv_cxx_have_is_bind_expression_class,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::is_bind_expression;]
        ],
        []
        )],
        [ac_cv_cxx_have_is_bind_expression_class=yes],
        [ac_cv_cxx_have_is_bind_expression_class=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_is_bind_expression_class" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_IS_BIND_EXPRESSION_CLASS,1,[Define if functional defines the std::is_bind_expression class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_IS_PLACEHOLDER_CLASS],
  [AC_CACHE_CHECK(
    [for std::is_placeholder in functional],
    ac_cv_cxx_have_is_placeholder_class,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::is_placeholder;]
        ],
        []
        )],
        [ac_cv_cxx_have_is_placeholder_class=yes],
        [ac_cv_cxx_have_is_placeholder_class=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_is_placeholder_class" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_IS_PLACEHOLDER_CLASS,1,[Define if functional defines the std::is_placeholder class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_BIND_FUNCTION],
  [AC_CACHE_CHECK(
    [for std::bind in functional],
    ac_cv_cxx_have_bind_function,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>
           #ifdef HAVE_NAMESPACES
             using namespace std;
           #endif]
           [int fn(int x, int y) { return x + y; }]
        ],
        [
          [auto bound_fn = bind(fn,1,2);]
          [bound_fn();]
        ]
        )],
        [ac_cv_cxx_have_bind_function=yes],
        [ac_cv_cxx_have_bind_function=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_bind_function" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_BIND_FUNCTION,1,[Define if functional defines the std::bind class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_CREF_FUNCTION],
  [AC_CACHE_CHECK(
    [for std::cref in functional],
    ac_cv_cxx_have_cref_function,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>
           #ifdef HAVE_NAMESPACES
             using namespace std;
           #endif]
        ],
        [
          [int x(1);]
          [auto crefd = cref(x);]
        ]
        )],
        [ac_cv_cxx_have_cref_function=yes],
        [ac_cv_cxx_have_cref_function=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_cref_function" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_CREF_FUNCTION,1,[Define if functional defines the std::cref class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_MEM_FN_FUNCTION],
  [AC_CACHE_CHECK(
    [for std::mem_fn in functional],
    ac_cv_cxx_have_mem_fn_function,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>
           #ifdef HAVE_NAMESPACES
             using namespace std;
           #endif]
          [struct st { int x; int fn() { return x; } };]
        ],
        [
          [st one {1};]
          [auto mem_fned = mem_fn(&st::fn);]
          [mem_fned(one);]
        ]
        )],
        [ac_cv_cxx_have_mem_fn_function=yes],
        [ac_cv_cxx_have_mem_fn_function=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_mem_fn_function" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_MEM_FN_FUNCTION,1,[Define if functional defines the std::mem_fn class.])
    fi
  ])

AC_DEFUN([AX_CXX_HAVE_REF_FUNCTION],
  [AC_CACHE_CHECK(
    [for std::ref in functional],
    ac_cv_cxx_have_ref_function,
    [AC_REQUIRE([AX_CXX_NAMESPACES])
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>
           #ifdef HAVE_NAMESPACES
             using namespace std;
           #endif]
        ],
        [
          [int one(1);]
          [auto refed = ref(one);]
          [++refed;]
        ]
        )],
        [ac_cv_cxx_have_ref_function=yes],
        [ac_cv_cxx_have_ref_function=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ac_cv_cxx_have_ref_function" = "xyes"
    then
      AC_DEFINE(HAVE_FUNCTIONAL_REF_FUNCTION,1,[Define if functional defines the std::ref class.])
    fi
  ])