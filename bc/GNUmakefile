CFLAGS := -O9 -W -Wall -Werror

prepare: bin/fuzz src/bc/bc share/bc.sites

share/bc.sites: src/bc/bc
	/usr/lib/sampler/tools/extract-section .debug_site_info $< >$@.
	mv $@. $@

src/bc/bc: src/Makefile force
	$(MAKE) -C src/lib
	$(MAKE) -C $(@D)

src/Makefile:
	cd $(@D) && CC='sampler-cc --sampler-scheme=scalar-pairs --no-sample' ./configure

force:
.PHONY: force

clean:
	rm -f bin/fuzz
	$(MAKE) -C src $@
