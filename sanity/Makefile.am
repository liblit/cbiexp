AM_CFLAGS := -O9 -W -Wall -Werror
noinst_DATA := share/sanity.sites build/sanity

share/sanity.sites: build/sanity
	test -d $(@D) || mkdir $(@D)
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

build/sanity: src/sanity.c
	test -d $(@D) || mkdir $(@D)
	$(sampler_cc) --no-sample --sampler-scheme=branches -o $@ $<

force:
.PHONY: force

CLEANFILES := $(noinst_DATA)
clean-local:
	rm -rf build
