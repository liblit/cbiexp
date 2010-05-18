noinst_DATA := src/moss src/mossbad sites implications

sites : share/mossbad.sites

implications : share/mossbad.implications

share/%.sites: src/%
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

share/%.sites: src/%
	-$(sampler_tooldir)/extract-section .debug_sampler_implications $< >$@.
	-mv $@. $@

src/moss: force
	$(MAKE) -C $(@D) $(@F)

src/mossbad: force
	$(MAKE) -C $(@D) $(@F) SCC="$(sampler_cc)"

force:
.PHONY: force

CLEANFILES := share/*.sites share/*.implications
clean-local:
	$(MAKE) -C src clean
