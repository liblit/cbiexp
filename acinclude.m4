AC_DEFUN([CBI_CONFIG_APP],
[test -e [$1/src] && AC_CONFIG_FILES([$1/bin/$2 $1/Makefile $1/config.site], [chmod +x $1/bin/$2])])
