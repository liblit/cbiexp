AM_CFLAGS := -O9 -W -Wall -Werror
noinst_DATA := build/bc/bc sites implications
noinst_PROGRAMS := bin/fuzz

sites : share/bc.sites

implications : share/bc.implications

share/bc.sites: build/bc/bc
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

share/bc.implications: build/bc/bc
	-$(sampler_tooldir)/extract-section .debug_sampler_implications $< >$@.
	-mv $@. $@

build/bc/bc: build/Makefile force
	$(MAKE) -C build/lib
	$(MAKE) -C $(@D)

force:
.PHONY: force

build/Makefile: config.site
	[ -d $(@D) ] || mkdir $(@D)
	cd $(@D) && CONFIG_SITE=../config.site ../src/configure

CLEANFILES := $(noinst_DATA)
clean-local:
	rm -rf build
