## Process this file with automake to produce Makefile.in

TESTS = rijndael-check ccrypt-check.sh crypt3-check

check_PROGRAMS = rijndael-check crypt3-check
check_SCRIPTS = ccrypt-check.sh

EXTRA_DIST = ccrypt-check.sh testdata testdata.cpt testdata.ucpt

rijndael_check_SOURCES = rijndael-check.c rijndael-alg-ref.c rijndael-alg-ref.h boxes-ref.dat
rijndael_check_LDADD = ../src/rijndael.o ../src/tables.o
rijndael_check_DEPENDENCIES = ../src/rijndael.o ../src/tables.o

crypt3_check_DEPENDENCIES = ../src/unixcrypt3.o
crypt3_check_LDADD = @LIBS@ ../src/unixcrypt3.o

../src/rijndael.o ../src/tables.o:
	(cd ../src && $(MAKE) $(AM_MAKEFLAGS) rijndael.o tables.o)

../src/unixcrypt3.o:
	(cd ../src && $(MAKE) $(AM_MAKEFLAGS) unixcrypt3.o)

