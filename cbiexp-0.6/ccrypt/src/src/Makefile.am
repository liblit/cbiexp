## Process this file with automake to produce Makefile.in

# the following can be overridden by the user, but note that -O3 makes
# a big difference in terms of number-crunching performance.
CFLAGS = -O3

bin_PROGRAMS = ccrypt
noinst_PROGRAMS = maketables
BUILT_SOURCES = tables.c
CLEANFILES = tables.c
EXTRA_DIST = getopt.c getopt1.c getopt.h unixcrypt3.c unixcrypt3.h

ccrypt_SOURCES = main.c main.h traverse.c traverse.h xalloc.c xalloc.h \
  readkey.c readkey.h ccrypt.c ccrypt.h io.c io.h unixcrypt.c unixcrypt.h \
  rijndael.h rijndael.c tables.h tables.c
ccrypt_LDADD = @EXTRA_OBJS@
ccrypt_DEPENDENCIES = @EXTRA_OBJS@

maketables_SOURCES = maketables.c

tables.c: maketables
	./maketables > tables.c

dist-hook:
	rm -f $(distdir)/tables.c

install-exec-hook:
	[ "@NAME@" = ccrypt ] || \
	mv -f $(DESTDIR)$(bindir)/ccrypt $(DESTDIR)$(bindir)/@NAME@
	ln -f -s @NAME@ $(DESTDIR)$(bindir)/@NAMEENCRYPT@
	ln -f -s @NAME@ $(DESTDIR)$(bindir)/@NAMEDECRYPT@
	ln -f -s @NAME@ $(DESTDIR)$(bindir)/@NAMECAT@

uninstall-local:
	rm -f $(DESTDIR)$(bindir)/@NAME@
	rm -f $(DESTDIR)$(bindir)/@NAMEENCRYPT@
	rm -f $(DESTDIR)$(bindir)/@NAMEDECRYPT@
	rm -f $(DESTDIR)$(bindir)/@NAMECAT@


