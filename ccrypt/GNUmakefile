program := src/src/ccrypt
sites := share/$(notdir $(program)).sites

inputs := share/inputs
cleartext := $(shell find $(inputs) -type f -maxdepth 1 ! -name '*.cpt' ! -name .cvsignore)
cyphertext := $(cleartext:=.cpt)

prepare: $(sites) $(program) $(cleartext) $(cyphertext)

$(sites): $(program)
	/usr/lib/sampler/tools/extract-section .debug_site_info $< >$@.
	mv $@. $@

$(program): src/Makefile force
	$(MAKE) -C $(<D)

$(cyphertext): %.cpt: % $(program)
	$(program) -e -K good <$< >$@.
	mv $@. $@

src/Makefile:
	cd $(@D) && CC='sampler-cc --sampler-scheme=returns --no-sample' ./configure

force:
.PHONY: force

clean:
	rm -f $(inputs)/*.cpt
	$(MAKE) -C src $@
