AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I /usr/share/aclocal

SUBDIRS = src

if HAVE_MEX
SUBDIRS += correlations
endif

DIST_SUBDIRS = correlations sanity src

EXTRA_DIST =					\
	bin/Check.pm				\
	bin/make-runs				\
	bin/MakeRuns.pm				\
	bin/RunsRunner.pm			\
	bin/TaskRunner.pm

check-local:
	$(MAKE) -C sanity check

dist-hook:
	rm -rf $(distdir)/bc $(distdir)/ccrypt $(distdir)/exif $(distdir)/mossexp $(distdir)/rhythmbox
