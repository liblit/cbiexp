#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <vector>
#include "units.h"

using namespace std;

bool is_srun[NUM_RUNS + 1];
bool is_frun[NUM_RUNS + 1];

struct site_t {
    bit_vector* b[6];
    site_t() 
    { 
        for (int i = 0; i < 6; i++)
            b[i] = NULL;
    }
};

site_t* data[NUM_UNITS];

#define inc(r, u, c, p)                                  \
{                                                        \
    if (data[u][c].b[p]) (*(data[u][c].b[p]))[r] = true; \
}

inline void print_pred(int u, int c, int p)
{
    int i;

    if (data[u][c].b[p]) {
        char file[100];
        sprintf(file, "/moa/sc3/mhn/new/tru/%d_%d_%d.txt", u, c, p);
        FILE* fp = fopen(file, "w");
        assert(fp);
        fprintf(fp, "F: ");
        for (i = 1; i <= NUM_RUNS; i++)
            if (is_frun[i] && (*(data[u][c].b[p]))[i] == true)
                fprintf(fp, "%d ", i);
        fprintf(fp, "\nS: ");
        for (i = 1; i <= NUM_RUNS; i++)
            if (is_srun[i] && (*(data[u][c].b[p]))[i] == true)
                fprintf(fp, "%d ", i);
        fprintf(fp, "\n");
        fclose(fp);
    }
}

main(int argc, char** argv)
{
    int i, j, k, x, y, z;
    FILE *sfp = NULL, *ffp = NULL, *pfp = NULL;

    /************************************************************************
     ** process command line options
     ************************************************************************/

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            if (sfp) {
                printf("-s specified multiple times\n");
                return 1;
            }
            i++;
            if (!argv[i]) {
                printf("argument to -s missing\n");
                return 1;
            }
            sfp = fopen(argv[i], "r");
            if (!sfp) {
                printf("Cannot open file %s for reading\n", argv[i]);
                return 1;
            }
            continue;
        }
        if (strcmp(argv[i], "-f") == 0) {
            if (ffp) {
                printf("-f specified multiple times\n");
                return 1;
            }
            i++;
            if (!argv[i]) {
                printf("argument to -f missing\n");
                return 1;
            }
            ffp = fopen(argv[i], "r");
            if (!ffp) {
                printf("Cannot open file %s for reading\n", argv[i]);
                return 1;
            }
            continue;
        }
        if (strcmp(argv[i], "-p") == 0) {
            if (pfp) {
                printf("-p specified multiple times\n");
                return 1;
            }
            i++;
            if (!argv[i]) {
                printf("argument to -p missing\n");
                return 1;
            }
            pfp = fopen(argv[i], "r");
            if (!pfp) {
                printf("Cannot open file %s for reading\n", argv[i]);
                return 1;
            }
            continue;
        }
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: compute [-s runs_file] [-f runs_file] [-p pred_file]\n");
            return 0;
        }
        printf("Illegal option: %s\n", argv[i]);
        return 1;
    }

    /************************************************************************
     ** read -s specified file (if any)
     ************************************************************************/

    if (sfp) {
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
        fclose(sfp);
    }

    /************************************************************************
     ** read -f specified file (if any)
     ************************************************************************/

    if (ffp) {
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
        fclose(ffp);
    }

    /************************************************************************
     ** do sanity check (no run should be both successful and failing)
     ************************************************************************/

    int ns = 0, nf = 0;
    for (i = 1; i <= NUM_RUNS; i++) {
        if (is_srun[i] && is_frun[i]) {
            printf("Run %d is both successful and failing\n", i);
            return 1;
        }
        if (is_srun[i])
            ns++;
        else if (is_frun[i])
            nf++;
    }
    printf("#S runs: %d #F runs: %d\n", ns, nf);

    for (i = 0; i < NUM_UNITS; i++) {
        data[i] = new (site_t) [units[i].c];
        assert(data[i]);
    }

    /************************************************************************
     ** read -f specified file (if any)
     ************************************************************************/

    if (pfp) {
        while (1) {
            int u, c, p;
            fscanf(pfp, "%d %d %d", &u, &c, &p);
            if (feof(pfp))
                break;
            assert(u >= 0 && u < NUM_UNITS );
            assert(c >= 0 && c < units[u].c);
            assert(p >= 0 && p < 6);
            data[u][c].b[p] = new bit_vector(NUM_RUNS + 1);
            assert(data[u][c].b[p]);
        }
        fclose(pfp);
    }

    /************************************************************************
     ** main loop
     ************************************************************************/

    for (i = 1; i <= NUM_RUNS; i++) {
        if (!is_srun[i] && !is_frun[i])
            continue;

        char file[100];
        sprintf(file, "/moa/sc4/mhn/runs/%d.txt", i);
        FILE* fp = fopen(file, "r");
        assert(fp);

        printf("r %d\n", i);

        while (1) {
            fscanf(fp, "%d", &j);
            if (feof(fp))
                break;
            if (units[j].s[0] == 'S' || units[j].s[0] == 'R') {
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d %d", &x, &y, &z); 
                    if (x > 0) {
                        inc(i, j, k, 0);
                        inc(i, j, k, 3);
                        inc(i, j, k, 5);
                    }
                    if (y > 0) {
                        inc(i, j, k, 2);
                        inc(i, j, k, 1);
                        inc(i, j, k, 5);
                    }
                    if (z > 0) {
                        inc(i, j, k, 4);
                        inc(i, j, k, 1);
                        inc(i, j, k, 3);
                    }
                }
            }  else {
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d", &x, &y); 
                    if (x > 0) inc(i, j, k, 0);
                    if (y > 0) inc(i, j, k, 1);
                }
            } 
        }
        fclose(fp);
    }

    for (j = 0; j < NUM_UNITS; j++) {
        switch (units[j].s[0]) {
        case 'S':
        case 'R':
            for (k = 0; k < units[j].c; k++) {
                print_pred(j, k, 0);
                print_pred(j, k, 1);
                print_pred(j, k, 2);
                print_pred(j, k, 3);
                print_pred(j, k, 4);
                print_pred(j, k, 5);
            }
            break;
        case 'B':
            for (k = 0; k < units[j].c; k++) {
                print_pred(j, k, 0);
                print_pred(j, k, 1);
            }
            break;
        default:
            assert(0);
        }
    }

    return 0;
}

