noinst_DATA := share/mossbad.sites src/moss src/mossbad

share/%.sites: src/%
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

src/moss: force
	$(MAKE) -C $(@D) $(@F)

src/mossbad: force
	$(MAKE) -C $(@D) $(@F) SCC="$(sampler_cc)"

force:
.PHONY: force

CLEANFILES := share/*.sites
clean-local:
	$(MAKE) -C src clean
