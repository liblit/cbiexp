#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include "def.h"

int num_runs = 0, num_sruns, num_fruns;
bool *is_srun, *is_frun;

void classify_runs()
{
    FILE *sfp, *ffp;
    int i;

    sfp = fopen(SRUNS_TXT_FILE, "r"); assert(sfp);
    while (1) {
        fscanf(sfp, "%d", &i);
        if (feof(sfp))
            break;
        if (i > num_runs)
            num_runs = i;
    }
    fclose(sfp);

    ffp = fopen(FRUNS_TXT_FILE, "r"); assert(ffp);
    while (1) {
        fscanf(ffp, "%d", &i);
        if (feof(ffp))
            break;
        if (i > num_runs)
            num_runs = i;
    }
    fclose(ffp);

    num_runs += 1;

    is_srun = new bool[num_runs];
    assert(is_srun);
    for (i = 0; i < num_runs; i++) is_srun[i] = false;
    is_frun = new bool[num_runs];
    assert(is_frun);
    for (i = 0; i < num_runs; i++) is_frun[i] = false;

    sfp = fopen(SRUNS_TXT_FILE, "r"); assert(sfp);
    while (1) {
        fscanf(sfp, "%d", &i);
        if (feof(sfp))
            break;
        is_srun[i] = 1;
        num_sruns++;
    }
    fclose(sfp);

    ffp = fopen(FRUNS_TXT_FILE, "r"); assert(ffp);
    while (1) {
        fscanf(ffp, "%d", &i);
        if (feof(ffp))
            break;
        if (is_srun[i]) {
            printf("Run %d is both successful and failing\n", i);
            exit(1);
        }
        is_frun[i] = 1;
        num_fruns++;
    }
    fclose(ffp);
}

