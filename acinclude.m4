AC_DEFUN([CBI_CONFIG_APP],
[if test "$with_experiments" != no -a -e [$1/src]; then
  AC_CONFIG_FILES($2, $3)
fi])
