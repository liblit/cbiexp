AC_DEFUN([CBI_CONFIG_APP],
[if test "$with_experiments" != no -a -e [$1/src]; then
  AC_CONFIG_FILES([$1/bin/$2 $1/Makefile $1/config.site], [chmod +x $1/bin/$2])
fi])
