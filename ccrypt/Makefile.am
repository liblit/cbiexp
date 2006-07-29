ccrypt := build/src/ccrypt

inputs := share/inputs
cleartext := $(shell find $(inputs) -type f -maxdepth 1 ! -name '*.cpt' ! -name .cvsignore)
cyphertext := $(cleartext:=.cpt)

noinst_DATA := $(ccrypt) $(cyphertext) sites implications

sites : share/ccrypt.sites

implications : share/ccrypt.implications

share/ccrypt.sites: $(ccrypt)
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

share/ccrypt.implications: $(ccrypt)
	-$(sampler_tooldir)/extract-section .debug_sampler_implications $< >$@.
	-mv $@. $@

$(ccrypt): build/Makefile force
	$(MAKE) -C $(<D) CFLAGS='-g -O3'

$(cyphertext): %.cpt: % $(ccrypt)
	build/src/ccrypt -e -K good <$< >$@.
	mv $@. $@

build/Makefile: config.site
	[ -d $(@D) ] || mkdir $(@D)
	cd $(@D) && CONFIG_SITE=../config.site ../src/configure

force:
.PHONY: force

CLEANFILES := $(inputs)/*.cpt
clean-local:
	rm -rf build
