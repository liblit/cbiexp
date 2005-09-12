########################################################################
#
#  basic project setup
#

AC_INIT(cbiexp, 0.1)
AC_CONFIG_AUX_DIR(config-aux)
AM_INIT_AUTOMAKE


########################################################################
#
#  compilation basics
#

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

old_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $GSL_CFLAGS"
AC_CHECK_HEADER(gsl/gsl_cdf.h, HAVE_GSL_CDF=1, HAVE_GSL_CDF=)
CFLAGS="$old_CFLAGS"
AM_CONDITIONAL(HAVE_GSL_CDF, test "$HAVE_GSL_CDF" = 1)
AC_SUBST(HAVE_GSL_CDF)


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


AC_CHECK_PROGS(HAVE_MEX, mex, [])
AM_CONDITIONAL(HAVE_MEX, test -n "$HAVE_MEX")


AC_PATH_PROG(PERL, perl)


########################################################################
#
#  instrumenting compiler
#

AC_ARG_WITH(sampler-build,
  AC_HELP_STRING([--with-sampler-build=DIR],
                 [use sampler tools from build tree DIR]))

AC_ARG_WITH(sampler-prefix,
  AC_HELP_STRING([--with-sampler-prefix=DIR],
                 [use sampler tools installed under DIR]))

if test -n "$with_sampler_build"; then
  AC_PATH_PROG(sampler_cc, sampler-cc-here, , $with_sampler_build/driver)
  sampler_tooldir="$with_sampler_build/tools"
elif test -n "$with_sampler_prefix"; then
  AC_PATH_PROG(sampler_cc, sampler-cc, , $with_sampler_prefix/bin)
  sampler_tooldir="$with_sampler_prefix/lib/sampler/tools"
else
  AC_PATH_PROG(sampler_cc, sampler-cc)
  sampler_tooldir="`cd \`dirname \"$sampler_cc\"\`/../lib/sampler/tools; pwd`"
fi

if ! test -x "$sampler_cc"; then
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
correlations/Makefile
sanity/Makefile
src/Makefile
src/corrective-ranking/Makefile
src/symmetric/Makefile
src/Score/Makefile
src/analysis-rules.mk
])

CBI_CONFIG_APP(bc)
CBI_CONFIG_APP(ccrypt, ccrypt/Makefile ccrypt/config.site ccrypt/bin/run-once, chmod +x ccrypt/bin/run-once)
CBI_CONFIG_APP(exif)
CBI_CONFIG_APP(mossexp, mossexp/Makefile)
CBI_CONFIG_APP(rhythmbox)

AC_OUTPUT
