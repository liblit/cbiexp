noinst_DATA := install/bin/exif

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

force:
.PHONY: force

.DELETE_ON_ERROR:

clean-local:
	rm -rf install
	rm -rf build
