#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "units.h"

#define MAX_RUNS 100
int num_runs_needed;

bool is_srun[NUM_RUNS + 1];
bool is_frun[NUM_RUNS + 1];

int runs[MAX_RUNS];
int num_runs_found = 0;

main(int argc, char** argv)
{
    int x, y, z, i, j, k;
    FILE* fp;
    char file[100];


    /************************************************************************
     ** process command line options
     ************************************************************************/

    if (argc != 6) {
        printf("usage: find_runs N f|s|a U C P\n");
        printf("N = Number of runs to find\n");
        printf("f|s|a = failed|successful|any runs");
        printf("U = unit index, C = site index with unit, P = pred index within site\n");
        return 1;
    }

    num_runs_needed = atoi(argv[1]);
    
    enum { S, F, A } r;

    switch (argv[2][0]) {
    case 's': r = S; break;
    case 'f': r = F; break;
    case 'a': r = A; break;
    default: assert(0);
    }

    int u = atoi(argv[3]), c = atoi(argv[4]), p = atoi(argv[5]);

    FILE* sfp = fopen("s.runs", "r");
    FILE* ffp = fopen("f.runs", "r");
    assert(sfp && ffp);

    while (1) {
        fscanf(sfp, "%d", &i);
        if (feof(sfp))
            break;
        if (i <= 0 || i > NUM_RUNS) {
            printf("Illegal run %d in -s specified file\n", i);
            return 1;
        }
        is_srun[i] = true;
    }

    while (1) {
        fscanf(ffp, "%d", &i);
        if (feof(ffp))
            break;
        if (i <= 0 || i > NUM_RUNS) {
            printf("Illegal run %d in -f specified file\n", i);
            return 1;
        }
        is_frun[i] = true;
    }

    fclose(sfp);
    fclose(ffp);

    printf("searching pred %c %d %d %d" in %s runs\n", units[u].s[0], u, c, p, argv[2]);

    for (i = 1; i <= NUM_RUNS; i++) {
        if (num_runs_found >= num_runs_needed)
            break;
        if (!is_frun[i] && !is_srun[i])
            continue;
        if (is_srun[i] && r == F)
            continue;
        if (is_frun[i] && r == S)
            continue;

        sprintf(file, "/moa/sc4/mhn/runs/%d.txt", i);
        fp = fopen(file, "r");
        assert(fp);

        for (j = 0; j < NUM_UNITS; j++) {
            fscanf(fp, "%d", &j);
            if (feof(fp))
                break;
            if (units[j].s[0] == 'S' || units[j].s[0] == 'R') {
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d %d", &x, &y, &z);
                    if (u == j && c == k && x + y + z > 0 && ((p == 0 && x     > 0) ||
                                                              (p == 1 && y + z > 0) ||
                                                              (p == 2 && y     > 0) ||
                                                              (p == 3 && x + z > 0) ||
                                                              (p == 4 && z     > 0) ||
                                                              (p == 5 && x + y > 0))) {
                        runs[num_runs_found] = i;
                        num_runs_found++;
                        goto OUT;
                    }
                }
            } else {
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d", &x, &y);
                    if (u == j && c == k && x + y > 0 && ((p == 0 && x > 0) || (p == 1 && y > 0))) {
                        runs[num_runs_found] = i;
                        num_runs_found++;
                        goto OUT;
                    }
                }
            }
        }
OUT:
        fclose(fp);
    }

    sprintf(file, "/moa/sc3/mhn/r/%d_%d_%d.html", u, c, p);
    fp = fopen(file, "w");
    assert(fp);
    fprintf(fp, "<html>\n<body>\n");
    for (i = 0; i < num_runs_found; i++)
        fprintf(fp, "[<a href=\"../s/%d\">%d</a>] ", runs[i], runs[i]);
    fprintf(fp, "\n</html>\n</body>\n");
    fclose(fp);

    return 0;
}

