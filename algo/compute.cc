#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "units.h"

bool is_srun[NUM_RUNS];
bool is_frun[NUM_RUNS];

struct site_t {
    int S[6], F[6];
    int os, of;
    site_t() 
    { 
        os = of = 0;
        for (int i = 0; i < 6; i++) {
            S[i] = 0;
            F[i] = 0;
        }
    }
};

site_t* data[NUM_UNITS];

#define inc(r, u, c, p)  \
{                                 \
    if (is_srun[r])               \
        data[u][c].S[p]++;        \
    else if (is_frun[r])          \
        data[u][c].F[p]++;        \
}

#define obs(r, u, c)  \
{                                 \
    if (is_srun[r])               \
        data[u][c].os++;          \
    else if (is_frun[r])          \
        data[u][c].of++;          \
}

void print_pred(FILE* fp, int u, int c, int p, const char* pred_kind, char* site_name)
{
    int s  = data[u][c].S[p];
    int f  = data[u][c].F[p];
    int os = data[u][c].os;
    int of = data[u][c].of;

    float cr = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = cr - co;

    if (in - 1.96 * sqrt(((cr * (1 - cr)) / (f + s)) + ((co * (1 - co))/(of + os))) > 0 && s + f >= 10) {
        fprintf(fp, "%.2f In, %.2f Cr, %.2f Co, %d S, %d F, %d OS, %d OF, ",
            in, cr, co, s, f, os, of);
        fprintf(fp, "<a href=\"r/%d_%d_%d.html\">", u, c, p);
        fprintf(fp, "%s %s", pred_kind, site_name);
        fprintf(fp, "</a><br>\n");
        printf("%d %d %d\n", u, c, p);
    }
}


main(int argc, char** argv)
{
    int i, j, k, x, y, z, w;
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
            printf("Usage: compute -s runs_file -f runs_file\n");
            return 0;
        }
        printf("Illegal option: %s\n", argv[i]);
        return 1;
    }

    FILE* sites_fp = fopen(O_SITES_FILE, "r");

    if (!sfp || !ffp || !sites_fp) {
        printf("Incorrect usage; try -h\n");
        return 1;
    }

    /************************************************************************
     ** read -s specified file 
     ************************************************************************/

    while (1) {
        fscanf(sfp, "%d", &i);
        if (feof(sfp))
            break;
        if (i < 0 || i >= NUM_RUNS) {
            printf("Illegal run %d in -s specified file\n", i);
            return 1;
        }
        is_srun[i] = true;
    }
    fclose(sfp);

    /************************************************************************
     ** read -f specified file 
     ************************************************************************/

    while (1) {
        fscanf(ffp, "%d", &i);
        if (feof(ffp))
            break;
        if (i < 0 || i >= NUM_RUNS) {
            printf("Illegal run %d in -f specified file\n", i);
            return 1;
        }
        is_frun[i] = true;
    }
    fclose(ffp);

    /************************************************************************
     ** do sanity check (no run should be both successful and failing)
     ************************************************************************/

    int ns = 0, nf = 0;
    for (i = 0; i < NUM_RUNS; i++) {
        if (is_srun[i] && is_frun[i]) {
            printf("Run %d is both successful and failing\n", i);
            return 1;
        }
        if (is_srun[i])
            ns++;
        else if (is_frun[i])
            nf++;
    }
    // printf("#S runs: %d #F runs: %d\n", ns, nf);

    /************************************************************************
     ** compute for each predicate, number of successful and failing runs in
     ** which it is (1) just observed, and (2) observed to be true.
     ************************************************************************/

    for (i = 0; i < NUM_UNITS; i++) {
        data[i] = new (site_t) [units[i].c];
        assert(data[i]);
    }


    for (i = 0; i < NUM_RUNS; i++) {
        if (!is_srun[i] && !is_frun[i])
            continue;

        // printf("r %d\n", i);

        char file[100];
        sprintf(file, O_REPORT_FILE, i);
        FILE* fp = fopen(file, "r");
        assert(fp);

        while (1) {
            fscanf(fp, "%d", &j);
            if (feof(fp))
                break;
            switch (units[j].s[0]) {
            case 'S':
            case 'R':
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d %d", &x, &y, &z); 
                    if (x + y + z > 0) {
                        obs(i, j, k);
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
                }
                break;
            case 'B':
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d", &x, &y); 
                    if (x + y > 0) {
                        obs(i, j, k);
                        if (x > 0) inc(i, j, k, 0);
                        if (y > 0) inc(i, j, k, 1);
                    }
                }
                break;
            case 'U':
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d %d %d", &x, &y, &z, &w);
                    if (x + y + z + w > 0) {
                        obs(i, j, k);
                        if (x > 0)
                            inc(i, j, k, 0);
                        if (y > 0)
                            inc(i, j, k, 1);
                        if (z > 0)
                            inc(i, j, k, 2);
                        if (w > 0)
                            inc(i, j, k, 3);
                    }
                }
                break;
            default:
                assert(0);
            }
        }
        fclose(fp);
    }

    FILE* out_s = fopen(O_S_HTML_FILE, "w");
    FILE* out_r = fopen(O_R_HTML_FILE, "w");
    FILE* out_b = fopen(O_B_HTML_FILE, "w");
    FILE* out_u = fopen(O_U_HTML_FILE, "w");

    assert(out_s && out_r && out_b && out_u);
    char site_name[10000];

    for (j = 0; j < NUM_UNITS; j++) {
        switch (units[j].s[0]) {
        case 'S':
            for (k = 0; k < units[j].c; k++) {
                fscanf(sites_fp, "%[^\n]s", site_name);
                fgetc (sites_fp);
                print_pred(out_s, j, k, 0, "LT" , site_name);
                print_pred(out_s, j, k, 1, "NLT", site_name);
                print_pred(out_s, j, k, 2, "EQ" , site_name);
                print_pred(out_s, j, k, 3, "NEQ", site_name);
                print_pred(out_s, j, k, 4, "GT" , site_name);
                print_pred(out_s, j, k, 5, "NGT", site_name);
            }
            break;
        case 'R':
            for (k = 0; k < units[j].c; k++) {
                fscanf(sites_fp, "%[^\n]s", site_name);
                fgetc (sites_fp);
                print_pred(out_r, j, k, 0, "LT" , site_name);
                print_pred(out_r, j, k, 1, "NLT", site_name);
                print_pred(out_r, j, k, 2, "EQ" , site_name);
                print_pred(out_r, j, k, 3, "NEQ", site_name);
                print_pred(out_r, j, k, 4, "GT" , site_name);
                print_pred(out_r, j, k, 5, "NGT", site_name);
            }
            break;
        case 'B':
            for (k = 0; k < units[j].c; k++) {
                fscanf(sites_fp, "%[^\n]s", site_name);
                fgetc (sites_fp);
                print_pred(out_b, j, k, 0, "F", site_name);
                print_pred(out_b, j, k, 1, "T", site_name);
            }
            break;
        case 'U':
            for (k = 0; k < units[j].c; k++) {
                fscanf(sites_fp, "%[^\n]s", site_name);
                fgetc (sites_fp);
                print_pred(out_u, j, k, 0, "(= 0)", site_name);
                print_pred(out_u, j, k, 1, "(= 1)", site_name);
                print_pred(out_u, j, k, 2, "(> 1)", site_name);
                print_pred(out_u, j, k, 3, "(inv)", site_name);
            }
            break;
        default:
            assert(0);
        }
    }

    fclose(out_s);
    fclose(out_r);
    fclose(out_b);
    fclose(out_u);
    fclose(sites_fp);
    return 0;
}

