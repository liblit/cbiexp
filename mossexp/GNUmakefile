prepare: src/moss src/mossbad share/mossbad.sites

share/%.sites: src/%
	/usr/lib/sampler/tools/extract-section .debug_site_info $< >$@.
	mv $@. $@

src/%: force
	$(MAKE) -C $(@D) $(@F)

force:
.PHONY: force

clean:
	rm -f share/*.sites
	$(MAKE) -C src $@

.PHONY: clean
