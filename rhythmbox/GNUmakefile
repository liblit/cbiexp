share/rhythmbox.sites: build/shell/rhythmbox
	/usr/lib/sampler/tools/extract-section .debug_site_info $< >$@

build/shell/rhythmbox: build/config.status force
	$(MAKE) -C $(<D)

build/config.status: config.site
	[ -d $(@D) ] || mkdir $(@D)
	cd $(@D) && CONFIG_SITE=../config.site ../src/configure

force:
.PHONY: force

.DELETE_ON_ERROR:
