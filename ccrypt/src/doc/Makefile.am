EXTRA_DIST = ccrypt.1.in man2html.sed cypfaq01.txt

htmldir = $(prefix)/doc/ccrypt

DISTCLEANFILES = ccrypt.ps ccrypt.html

man_MANS = ccrypt.1
html_DATA = ccrypt.html

ps: ccrypt.ps
html: ccrypt.html

ccrypt.ps: ccrypt.1
	groff -man ccrypt.1 > ccrypt.ps

ccrypt.html: ccrypt.1 $(srcdir)/man2html.sed
	cat ccrypt.1 | sed -f $(srcdir)/man2html.sed > ccrypt.html

install-data-hook:
	[ "@NAME@" = ccrypt ] || \
	mv -f $(DESTDIR)$(man1dir)/ccrypt.1 $(DESTDIR)$(man1dir)/@NAME@.1
	ln -f -s @NAME@.1 $(DESTDIR)$(man1dir)/@NAMEENCRYPT@.1
	ln -f -s @NAME@.1 $(DESTDIR)$(man1dir)/@NAMEDECRYPT@.1
	ln -f -s @NAME@.1 $(DESTDIR)$(man1dir)/@NAMECAT@.1

uninstall-local:
	rm -f $(DESTDIR)$(man1dir)/@NAME@.1
	rm -f $(DESTDIR)$(man1dir)/@NAMEENCRYPT@.1
	rm -f $(DESTDIR)$(man1dir)/@NAMEDECRYPT@.1
	rm -f $(DESTDIR)$(man1dir)/@NAMECAT@.1

