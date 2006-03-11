AM_CFLAGS := -O9 -W -Wall -Werror
noinst_DATA := share/sanity.sites build/sanity

EXTRA_DIST =					\
	analysis/GNUmakefile			\
	bin					\
	src

share/sanity.sites: build/sanity
	test -d $(@D) || mkdir $(@D)
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

build/sanity: src/sanity.c
	test -d $(@D) || mkdir $(@D)
	$(sampler_cc) --no-sample --sampler-scheme=branches --relative-paths -o $@ $<

force:
.PHONY: force

CLEANFILES := $(noinst_DATA)
clean-local:
	rm -rf build

check-local: data/stamp-labels
	test -d analysis || mkdir analysis
	$(MAKE) -C analysis -f ../$(srcdir)/analysis/GNUmakefile

data/stamp-labels: build/sanity
	$(top_srcdir)/bin/make-runs --bindir=$(srcdir)/bin 100

dist-hook:
	rm -r $(distdir)/*/CVS
