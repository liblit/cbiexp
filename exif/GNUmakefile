CC  := sampler-cc --sampler-scheme=scalar-pairs --sampler-scheme=branches --no-sample
# using the returns scheme masks the bug

pwd := $(shell pwd)

install/bin/exif: build/exif/config.status
	$(MAKE) -C build/exif install

build/exif/config.status: install/lib/libexif.so
	[ -d $(@D) ] || mkdir -p $(@D)
	cd $(@D) && CC='$(CC)' PKG_CONFIG_PATH=$(pwd)/install/lib/pkgconfig ../../src/exif-0.6.9/configure --prefix=$(pwd)/install

install/lib/libexif.so: build/libexif/config.status
	$(MAKE) -C build/libexif install

build/libexif/config.status:
	[ -d $(@D) ] || mkdir -p $(@D)
	cd $(@D) && CC='$(CC)' ../../src/libexif-0.6.10/configure --prefix=$(pwd)/install

force:
.PHONY: force

.DELETE_ON_ERROR:
