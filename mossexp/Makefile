BINDIR = /moa/sc1/cbi/bin
EXTRACT = $(BINDIR)/extract-section
BAD = mossbad
MOSS = moss $(BAD)

CC = gcc
CPPFLAGS = -DDEBUG=0 -DNDEBUG -Ddeletes=""
CFLAGS = -O3 -Wall

SCC = $(BINDIR)/sampler-cc
SCHEMES = branches returns scalar-pairs
SFLAGS = $(SCHEMES:%=--sampler-scheme=%)
SFLAGS += --no-sample
SFLAGS += --assign-across-pointer --assign-into-field --assign-into-index
SFLAGS += --compare-constants
SFLAGS += --rename-locals
#SFLAGS += -save-temps
#SFLAGS += -v -Q

-include site.mk


########################################################################


all: $(MOSS)

moss: moss.o lib/regions-norc.o
	$(CC) $(CFLAGS) moss.o lib/regions-norc.o -o moss

moss.o: moss.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c moss.c

mossbad: mossbad.o lib/regions-norc.o reporting.o
	$(SCC) $(SFLAGS) $(CFLAGS) $^ -o $@

mossbad.o: %.o: mossbad.c reporting.h
	$(SCC) $(SFLAGS) $(CFLAGS) $(CPPFLAGS) --exclude-file=$< -c $<

lib/regions-norc.o:
	cd lib; make regions-norc.o

reporting.o: reporting.c reporting.h

moss.c mossbad.c: %.c: %_includes.flex 
#	flex -8 -i -omoss.c moss_includes.flex
	flex -8 -f -o$@ $<

moss_includes.flex mossbad_includes.flex: %_includes.flex: %.flex
	cd languages; ./build
	scripts/myinclude <$< >$@

clean:
	rm -f $(MOSS) *.[iso] $(MOSS:=.c) $(MOSS:=_includes.flex) $(BAD:=.sites) $(BAD:=.cfg) $(BAD:=.inst.i) *~


########################################################################


ifdef INSTDIR

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

else

install:
	: $(error please use "make INSTDIR=<dir> ..." when installing)

endif
