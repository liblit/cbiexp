INSTDIR = /moa/sc1/cbi
CC = gcc
SCC = /usr/bin/sampler-cc scalar-pairs --no-sample
SCC2 = /usr/bin/sampler-cc branches --no-sample
CPPFLAGS = -DDEBUG=0 -DNDEBUG -Ddeletes=""
CFLAGS = -O3 -Wall
EXTRACT = /usr/lib/sampler/tools/extract-section
BAD = mossbad mossbad2
MOSS = moss $(BAD)

all: $(MOSS)

lib/regions-norc.o:
	cd lib; make regions-norc.o

moss: moss.o lib/regions-norc.o
	$(CC) $(CFLAGS) moss.o lib/regions-norc.o -o moss

moss.o: moss.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c moss.c

moss.c: moss_includes.flex 
#	flex -8 -i -omoss.c moss_includes.flex
	flex -8 -f -omoss.c moss_includes.flex

moss_includes.flex: moss.flex
	cd languages; ./build
	scripts/myinclude < moss.flex > moss_includes.flex

mossbad: mossbad.o lib/regions-norc.o reporting.o
	$(SCC) $(CFLAGS) $^ -o $@

mossbad.o: mossbad.c reporting.h
	$(SCC) $(CFLAGS) $(CPPFLAGS) --exclude-file=mossbad.c -c mossbad.c

mossbad2: mossbad2.o lib/regions-norc.o reporting.o
	$(SCC2) $(CFLAGS) $^ -o $@

mossbad2.o: mossbad.c reporting.h
	cp mossbad.c mossbad2.c
	$(SCC2) $(CFLAGS) $(CPPFLAGS) --exclude-file=mossbad2.c -c mossbad2.c

mossbad.c: mossbad_includes.flex 
	flex -8 -f -omossbad.c mossbad_includes.flex

mossbad_includes.flex: mossbad.flex
	cd languages; ./build
	scripts/myinclude < mossbad.flex > mossbad_includes.flex

reporting.o: reporting.c reporting.h

clean: 
	rm -f moss.c moss mossbad.c mossbad2.c mossbad_includes.flex mossbad.o mossbad2.o mossbad mossbad2 moss_includes.flex moss.o *~

BIN = $(MOSS) clusterdata.pl watchdog.pl make-runs

install: install-bin install-share
install-bin: $(BIN:%=$(INSTDIR)/bin/%)
install-share: install-cfg install-sites
install-cfg: $(BAD:%=$(INSTDIR)/share/%.cfg)
install-sites: $(BAD:%=$(INSTDIR)/share/%.sites)

$(BIN:%=$(INSTDIR)/bin/%): $(INSTDIR)/bin/%: %
	[ -d $(INSTDIR)/bin ] || mkdir -p $(INSTDIR)/bin
	cp $< $@

$(MOSS:%=$(INSTDIR)/share/%.cfg): $(INSTDIR)/share/%.cfg: $(INSTDIR)/bin/%
	[ -d $(@D) ] || mkdir -p $(@D)
	$(EXTRACT) .debug_sampler_cfg $< >$@.tmp
	mv $@.tmp $@

$(MOSS:%=$(INSTDIR)/share/%.sites): $(INSTDIR)/share/%.sites: $(INSTDIR)/bin/%
	[ -d $(@D) ] || mkdir -p $(@D)
	$(EXTRACT) .debug_site_info $< >$@.tmp
	mv $@.tmp $@
