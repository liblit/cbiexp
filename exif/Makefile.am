noinst_DATA := install/bin/exif sites implications 

pwd := $(shell pwd)

install/bin/exif: build/exif/config.status
	$(MAKE) -C build/exif install

build/exif/config.status: install/lib/libexif.so config.site
	[ -d $(@D) ] || mkdir -p $(@D)
	cd $(@D) && CONFIG_SITE=../../config.site PKG_CONFIG_PATH=$(pwd)/install/lib/pkgconfig ../../src/exif-0.6.9/configure --prefix=$(pwd)/install

install/lib/libexif.so: build/libexif/config.status
	$(MAKE) -C build/libexif install

build/libexif/config.status: config.site
	[ -d $(@D) ] || mkdir -p $(@D)
	cd $(@D) && CONFIG_SITE=../../config.site ../../src/libexif-0.6.10/configure --prefix=$(pwd)/install

sites : share/exif.sites share/libexif.so.sites

implications : share/exif.implications share/libexif.so.implications

share/exif.sites : install/bin/exif
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

share/libexif.so.sites : install/lib/libexif.so
	$(sampler_tooldir)/extract-section .debug_site_info $< >$@.
	mv $@. $@

share/exif.implications : install/bin/exif
	-$(sampler_tooldir)/extract-section .debug_sampler_implications $< >$@.
	-mv $@. $@

share/libexif.so.implications : install/lib/libexif.so
	-$(sampler_tooldir)/extract-section .debug_sampler_implications $< >$@.
	-mv $@. $@

force:
.PHONY: force

.DELETE_ON_ERROR:

CLEANFILES := share/*.sites share/*.implications
clean-local:
	rm -rf install
	rm -rf build
