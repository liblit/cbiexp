########################################################################
#
#  basic project setup
#

AC_INIT(cbiexp, 0.0)
AC_CONFIG_AUX_DIR(config-aux)
AM_INIT_AUTOMAKE


########################################################################
#
#  compilation basics
#

new_cxx=/moa/sc2/gcc-3.3.3/bin/c++
if test -x "$new_cxx"; then
  : ${CXX:=$new_cxx}
fi

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_LEX

AM_PATH_GSL

AC_LANG(C++)
AC_REQUIRE_CPP

# old versions of g++ die on large site info arrays
if test "$GXX" = yes; then
  AC_MSG_CHECKING(for GNU C++ compiler version)
  AC_PREPROC_IFELSE([AC_LANG_PROGRAM([[
#include <ansidecl.h>
#if GCC_VERSION < 3003
#error
#endif]])],
    [AC_MSG_RESULT(3.3.x or later)],
    [AC_MSG_RESULT(earlier than 3.3.x)
    AC_MSG_WARN(this compiler version may not handle large numbers of sites)])
fi


########################################################################
#
#  instrumenting compiler
#

AC_ARG_VAR(SAMPLER_CC, [instrumenting C compiler command])
if test -z "$SAMPLER_CC"; then
  AC_CHECK_PROGS(SAMPLER_CC, sampler-cc)
else
  AC_MSG_CHECKING(sampler-cc)
  AC_MSG_RESULT($SAMPLER_CC)
fi


########################################################################
#
#  generate output files
#

AM_CONFIG_HEADER(config.h)

AC_CONFIG_FILES([
Makefile
correlations/Makefile
rhythmbox/config.site
src/Makefile
src/corrective-ranking/Makefile
src/Score/Makefile
src/compiler.cc
])

AC_OUTPUT
