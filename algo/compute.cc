#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "units.h"
#include "sites.h"

bool is_srun[NUM_RUNS + 1];
bool is_frun[NUM_RUNS + 1];

struct site_t {
    int S[6], F[6];
    int os, of;
    site_t() { os = of = 0;
               S[0] = S[1] = S[2] = S[3] = S[4] = S[5] = 0;
               F[0] = F[1] = F[2] = F[3] = F[4] = F[5] = 0; }
};

site_t* data[NUM_UNITS];

inline void inc(int r, int u, int c, int p)
{ 
    if (is_srun[r]) 
        data[u][c].S[p]++; 
    else if (is_frun[r])
        data[u][c].F[p]++; 
}

inline void obs(int r, int u, int c) 
{ 
    if (is_srun[r]) 
        data[u][c].os++;
    else if (is_frun[r]) 
        data[u][c].of++; 
}

inline void print_pred(FILE* fp, int u, int c, int p, const char* pred_kind, const char* site_name)
{
    int s  = data[u][c].S[p];
    int f  = data[u][c].F[p];
    int os = data[u][c].os;
    int of = data[u][c].of;

    float crash   = ((float)  f) / ( s +  f);
    float context = ((float) of) / (os + of);
    fprintf(fp, "%.2f In %.2f Cr %.2f Co %d S %d F %d OS %d OF ",
        crash - context, crash, context, s, f, os, of);
    fprintf(fp, "<a href=\"r/%d_%d_%d.html\">", u, c, p);
    fprintf(fp, "%s %s", pred_kind, site_name);
    fprintf(fp, "</a><br>\n");
}

main(int argc, char** argv)
{
    int i, j, k, x, y, z;
    FILE *sfp = NULL, *ffp = NULL;

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
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: compute [-s runs_file] [-f runs_file]\n");
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

    /************************************************************************
     ** compute for each predicate, number of successful and failing runs in
     ** which it is (1) just observed, and (2) observed to be true.
     ************************************************************************/

    for (i = 0; i < NUM_UNITS; i++) {
        data[i] = new (site_t) [units[i].c];
        assert(data[i]);
    }

    for (i = 1; i <= NUM_RUNS; i++) {
        if (!is_srun[i] && !is_frun[i])
            continue;
        char file[100];
        sprintf(file, "/moa/sc4/mhn/runs/%d.txt", i);
        FILE* fp = fopen(file, "r");
        assert(fp);

        // printf("r %d\n", i);

        while (1) {
            fscanf(fp, "%d", &j);
            if (feof(fp))
                break;
            if (units[j].s[0] == 'S' || units[j].s[0] == 'R') {
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d %d", &x, &y, &z); 
                    if (x + y + z > 0) 
                        obs(i, j, k);
                    if (x     > 0) inc(i, j, k, 0);
                    if (y + z > 0) inc(i, j, k, 1);
                    if (y     > 0) inc(i, j, k, 2);
                    if (x + z > 0) inc(i, j, k, 3);
                    if (z     > 0) inc(i, j, k, 4);
                    if (x + y > 0) inc(i, j, k, 5);
                }
            }  else {
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d", &x, &y); 
                    if (x + y > 0)
                        obs(i, j, k);
                    if (x > 0) inc(i, j, k, 0);
                    if (y > 0) inc(i, j, k, 1);
                }
            } 
        }
        fclose(fp);
    }

    FILE* out_s = fopen("S.html", "w");
    FILE* out_r = fopen("R.html", "w");
    FILE* out_b = fopen("B.html", "w");
    assert(out_s && out_r && out_b);

    int s = 0;

    for (j = 0; j < NUM_UNITS; j++) {
        switch (units[j].s[0]) {
        case 'S':
            for (k = 0; k < units[j].c; k++) {
                print_pred(out_s, j, k, 0, "LT" , sites[s]);
                print_pred(out_s, j, k, 1, "NLT", sites[s]);
                print_pred(out_s, j, k, 2, "EQ" , sites[s]);
                print_pred(out_s, j, k, 3, "NEQ", sites[s]);
                print_pred(out_s, j, k, 4, "GT" , sites[s]);
                print_pred(out_s, j, k, 5, "NGT", sites[s]);
                s++;
            }
            break;
        case 'R':
            for (k = 0; k < units[j].c; k++) {
                print_pred(out_r, j, k, 0, "LT" , sites[s]);
                print_pred(out_r, j, k, 1, "NLT", sites[s]);
                print_pred(out_r, j, k, 2, "EQ" , sites[s]);
                print_pred(out_r, j, k, 3, "NEQ", sites[s]);
                print_pred(out_r, j, k, 4, "GT" , sites[s]);
                print_pred(out_r, j, k, 5, "NGT", sites[s]);
                s++;
            }
            break;
        case 'B':
            for (k = 0; k < units[j].c; k++) {
                print_pred(out_b, j, k, 0, "F", sites[s]);
                print_pred(out_b, j, k, 1, "T", sites[s]);
                s++;
            }
            break;
        default:
            assert(0);
        }
    }

    fclose(out_s);
    fclose(out_r);
    fclose(out_b);

    return 0;
}

