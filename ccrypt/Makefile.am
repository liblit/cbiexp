ccrypt := build/src/ccrypt

inputs := share/inputs
cleartext := $(shell find $(inputs) -type f -maxdepth 1 ! -name '*.cpt' ! -name .cvsignore)
cyphertext := $(cleartext:=.cpt)

noinst_DATA := share/ccrypt.sites $(ccrypt) $(cyphertext)

share/ccrypt.sites: $(ccrypt)
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

build/src/ccrypt: build/Makefile force
	$(MAKE) -C $(<D)

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
