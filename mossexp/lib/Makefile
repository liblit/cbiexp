# Generated automatically from Makefile.in by configure.
prefix = ~/moss/lib
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib/rc
includedir = $(libdir)
objdir = $(libdir)/i386-redhat-linux

INSTALL = /usr/bin/install -c
install_data = ${INSTALL} -m 644
install_program = ${INSTALL}

CC=gcc
CFLAGS = -g -O2 -g -Wall -Ddeletes="" -DNMEMDEBUG -DNDEBUG -O9

all: regions-std.o regions-std-stats.o regions-pairs.o regions-pairs-stats.o regions-norc.o

regions-std.o: regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -o regions-std.o regions.c

regions-std-stats.o: regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -DREGIONPROFILE -o regions-std-stats.o regions.c

regions-pairs.o: regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -DRCPAIRS -o regions-pairs.o regions.c

regions-pairs-stats.o: regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -DRCPAIRS -DREGIONPROFILE -o regions-pairs-stats.o regions.c

regions-norc.o: regions.c pages.c alloc.c regions.h
	gcc -c $(CFLAGS) -DNORC -o regions-norc.o regions.c

install: all
	$(INSTALL) -d $(includedir) $(objdir)
	$(install_data) regions.h $(includedir)
	$(install_data) regions-norc.o $(objdir)
	$(install_data) regions-pairs.o $(objdir)
	$(install_data) regions-pairs-stats.o $(objdir)
	$(install_data) regions-std.o $(objdir)
	$(install_data) regions-std-stats.o $(objdir)

clean:
	rm -f *.o
