AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I /usr/share/aclocal

SUBDIRS = src

if HAVE_MEX
SUBDIRS += correlations
endif

DIST_SUBDIRS = correlations src

EXTRA_DIST =					\
	bin/Check.pm				\
	bin/make-runs				\
	bin/MakeRuns.pm				\
	bin/RunsRunner.pm			\
	bin/TaskRunner.pm			\
	doc

check-local:
	$(MAKE) -C check

dist-hook:
	rm -rf $(distdir)/bc $(distdir)/ccrypt $(distdir)/exif $(distdir)/mossexp $(distdir)/rhythmbox
	rm -rf $(distdir)/doc/.svn

distcheck: dist
	gunzip -c $(distdir).tar.gz | $(am__untar)
	cd $(distdir) && ./configure && make && make check
	$(am__remove_distdir)
