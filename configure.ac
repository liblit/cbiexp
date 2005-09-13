########################################################################
#
#  basic project setup
#

AC_INIT(cbiexp, 0.2)
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
                 [look for Boost in PREFIX]),
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
  sampler_debug="$with_sampler_build/launcher"
elif test -n "$with_sampler_prefix"; then
  AC_PATH_PROG(sampler_cc, sampler-cc, , $with_sampler_prefix/bin)
  sampler_tooldir="$with_sampler_prefix/lib/sampler/tools"
  sampler_debug="$with_sampler_prefix/share/sampler"
else
  AC_PATH_PROG(sampler_cc, sampler-cc)
  if test -n "$sampler_cc"; then
    sampler_tooldir="`cd \`dirname \"$sampler_cc\"\`/../lib/sampler/tools; pwd`"
    sampler_debug="`cd \`dirname \"$sampler_cc\"\`/../share/sampler; pwd`"
  fi
fi

if ! test -x "$sampler_cc"; then
  with_experiments=no
fi

AC_MSG_CHECKING(sampler tools)
if test -x "$sampler_tooldir/extract-section"; then
  AC_MSG_RESULT($sampler_tooldir)
else
  AC_MSG_RESULT(no)
  with_experiments=no
fi
AC_SUBST(sampler_tooldir)

sampler_debug="$sampler_debug/print-debug-info"
AC_SUBST(sampler_debug)


########################################################################
#
#  generate output files
#

AM_CONFIG_HEADER(config.h)

AC_CONFIG_FILES([
Makefile
correlations/Makefile
src/Makefile
src/corrective-ranking/Makefile
src/symmetric/Makefile
src/Score/Makefile
src/analysis-rules.mk
])

CBI_CONFIG_APP(bc, [bc/bin/run-once bc/Makefile bc/config.site], [chmod +x bc/bin/run-once])
CBI_CONFIG_APP(ccrypt, [ccrypt/bin/run-once ccrypt/Makefile ccrypt/config.site], [chmod +x ccrypt/bin/run-once])
CBI_CONFIG_APP(exif, [exif/bin/run-once exif/Makefile exif/config.site], [chmod +x exif/bin/run-once])
CBI_CONFIG_APP(mossexp, [mossexp/bin/run-once mossexp/Makefile], [chmod +x mossexp/bin/run-once])
CBI_CONFIG_APP(rhythmbox, [rhythmbox/bin/run-client rhythmbox/Makefile rhythmbox/config.site], [chmod +x rhythmbox/bin/run-client])
CBI_CONFIG_APP(sanity, [sanity/bin/run-once sanity/Makefile], [chmod +x sanity/bin/run-once])

AC_OUTPUT
