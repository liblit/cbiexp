// Usage: process_run_labels <num_runs>
// Creates files SRUNS_TXT_FILE and FRUNS_TXT_FILE

#include <cstdio>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include "def.h"

int
main(int argc, char** argv)
{
    assert(argc == 2);
    int num_runs = atoi(argv[1]);

    FILE* sfp = fopen(SRUNS_TXT_FILE, "w"); assert(sfp);
    FILE* ffp = fopen(FRUNS_TXT_FILE, "w"); assert(ffp);

    for (int i = 0; i < num_runs; i++) {
        char s[100];
        sprintf(s, LABEL_PATH_FMT, i);
        FILE* fp = fopen(s, "r");
	if (!fp) {
	    fprintf(stderr, "cannot read %s: %s\n", s, strerror(errno));
	    return 1;
	}
        fscanf(fp, "%s", s);
        if (!strcmp(s, "success")) {
            fprintf(sfp, "%d\n", i);
	    fclose(fp);
            continue;
        }
        if (!strcmp(s, "failure")) {
            fprintf(ffp, "%d\n", i);
	    fclose(fp);
            continue;
        }
        assert(!strcmp(s, "ignore"));
        fclose(fp);
    }

    fclose(sfp);
    fclose(ffp);
    return 0;
}

