INSTDIR = /moa/sc1/cbi/bin
CC = gcc
SCC = /usr/bin/sampler-cc scalar-pairs --no-sample
SCC2 = /usr/bin/sampler-cc branches --no-sample
CPPFLAGS = -DDEBUG=0 -DNDEBUG -Ddeletes=""
CFLAGS = -O3 -Wall

all: moss mossbad mossbad2

moss: moss.o lib/regions-norc.o
	$(CC) $(CFLAGS) moss.o lib/regions-norc.o -o moss

moss.o: moss.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c moss.c

moss.c: moss_includes.flex 
#	flex -8 -i -omoss.c moss_includes.flex
	flex -8 -f -omoss.c moss_includes.flex

moss_includes.flex: moss.flex
	cd languages; ./build
	../scripts/myinclude < moss.flex > moss_includes.flex

mossbad: mossbad.o lib/regions-norc.o
	$(SCC) $(CFLAGS) mossbad.o lib/regions-norc.o -o mossbad

mossbad.o: mossbad.c
	$(SCC) $(CFLAGS) $(CPPFLAGS) --exclude-file=mossbad.c -c mossbad.c

mossbad2: mossbad2.o lib/regions-norc.o
	$(SCC2) $(CFLAGS) mossbad2.o lib/regions-norc.o -o mossbad2

mossbad2.o: mossbad.c
	cp mossbad.c mossbad2.c
	$(SCC2) $(CFLAGS) $(CPPFLAGS) --exclude-file=mossbad2.c -c mossbad2.c

mossbad.c: mossbad_includes.flex 
	flex -8 -f -omossbad.c mossbad_includes.flex

mossbad_includes.flex: mossbad.flex
	cd languages; ./build
	../scripts/myinclude < mossbad.flex > mossbad_includes.flex

clean: 
	rm -f moss.c moss mossbad.c mossbad2.c mossbad_includes.flex mossbad.o mossbad2.o mossbad mossbad2 moss_includes.flex moss.o *~

install:
	cp moss $(INSTDIR)
	cp mossbad $(INSTDIR)
	cp mossbad2 $(INSTDIR)
	cp clusterdata.pl $(INSTDIR)
	cp watchdog.pl $(INSTDIR)
	chmod a+x $(INSTDIR)/*