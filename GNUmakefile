CFLAGS = -O3 
CC = g++

all: bin/process_run_labels bin/map_sites bin/format bin/gen_views bin/print_1_cluster 

bin/process_run_labels: src/process_run_labels.cc src/def.h
	$(CC) $(CFLAGS) src/process_run_labels.cc -Isrc -o $@

bin/map_sites: src/map_sites.cc src/def.h
	$(CC) $(CFLAGS) src/map_sites.cc -Isrc -o $@

bin/format: src/format.cc
	$(CC) $(CFLAGS) src/format.cc -o $@

bin/gen_views: src/gen_views.cc 
	$(CC) $(CFLAGS) src/gen_views.cc -o $@

bin/print_1_cluster: src/print_1_cluster.cc src/def.h 
	$(CC) $(CFLAGS) src/print_1_cluster.cc -Isrc -o $@

clean:
	rm -f bin/process_run_labels bin/map_sites bin/format bin/gen_views bin/print_1_cluster

