AM_CFLAGS := -O9 -W -Wall -Werror
noinst_DATA := share/bc.sites
noinst_PROGRAMS := bin/fuzz build/bc/bc

share/bc.sites: build/bc/bc
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

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
