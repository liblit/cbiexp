CFLAGS = -O3 
CC = g++

all: process_run_labels map_sites format gen_views

process_run_labels: src/process_run_labels.cc src/def.h
	$(CC) $(CFLAGS) src/$@.cc -Isrc -o bin/$@

map_sites: src/map_sites.cc src/def.h
	$(CC) $(CFLAGS) src/$@.cc -Isrc -o bin/$@

format: src/format.cc
	$(CC) $(CFLAGS) src/$@.cc -o bin/$@

gen_views: src/gen_views.cc 
	$(CC) $(CFLAGS) src/$@.cc -o bin/$@

clean:
	rm -f bin/process_run_labels bin/map_sites bin/format bin/gen_views

