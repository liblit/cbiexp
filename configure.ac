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

AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_LEX

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
#  supporting packages
#


AM_PATH_GSL


AC_ARG_WITH(boost,
  AC_HELP_STRING([--with-boost=PREFIX],
                 [look for Boost in PREFIX (optional)]),
  BOOST_CPPFLAGS="-I$withval/include",
  BOOST_CPPFLAGS='')

old_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
AC_CHECK_HEADER(boost/version.hpp, ,
  AC_MSG_ERROR(cannot find Boost headers; please use "--with-boost=DIR"))
CPPFLAGS="$old_CPPFLAGS"

AC_SUBST(BOOST_CPPFLAGS)


########################################################################
#
#  instrumenting compiler
#

AC_ARG_WITH(sampler-build,
  AC_HELP_STRING([--with-sampler-build=DIR],
                 [use sampler tools from build tree DIR]))

if test -z "$with_sampler_build"; then
  AC_PATH_PROG(sampler_cc, sampler-cc)
  sampler_tooldir="`cd \`dirname $sampler_cc\`/../lib/sampler/tools; pwd`"
else
  AC_PATH_PROG(sampler_cc, sampler-cc-here, , $with_sampler_build/instrumentor/driver)
  sampler_tooldir="$with_sampler_build/tools"
fi

if test -z "$sampler_cc"; then
  AC_MSG_ERROR(cannot find sampler-cc; please use "--with-sampler-build=DIR")
fi

AC_MSG_CHECKING(sampler tools)
if test -x "$sampler_tooldir/extract-section"; then
  AC_MSG_RESULT($sampler_tooldir)
  AC_SUBST(sampler_tooldir)
else
  AC_MSG_ERROR(cannot find sampler tools in $sampler_tooldir; please use "--with-sampler-build=DIR")
fi


########################################################################
#
#  generate output files
#

AM_CONFIG_HEADER(config.h)

AC_CONFIG_FILES([
Makefile
bc/Makefile
bc/config.site
ccrypt/Makefile
ccrypt/config.site
correlations/Makefile
exif/Makefile
exif/config.site
mossexp/Makefile
rhythmbox/Makefile
rhythmbox/config.site
src/Makefile
src/corrective-ranking/Makefile
src/Score/Makefile
src/analysis-rules.mk
])

AC_OUTPUT
