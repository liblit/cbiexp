AC_DEFUN([CBI_CONFIG_APP],
[test -e [$1/Makefile] && AC_CONFIG_FILES(ifelse([$2], [], [$1/Makefile $1/config.site], [$2]))])
