#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include "classify_runs.h"
#include "scaffold.h"

using namespace std;

struct site_details_t {
    bit_vector *obs[6], *tru[6];
    site_details_t()
    {
        for (int i = 0; i < 6; i++)
            obs[i] = tru[i] = NULL;
    }
};

site_details_t* site_details[NUM_UNITS];

char* sruns_file = NULL;
char* fruns_file = NULL;
char* preds_file = NULL;
char* compact_report_path_fmt = NULL;
char* obs_file = NULL;
char* tru_file = NULL;

FILE* preds_fp = NULL;
FILE* obs_fp = NULL;
FILE* tru_fp = NULL;

inline void inc_obs(int r, int u, int c, int p) 
{                                                           
    if (site_details[u][c].obs[p]) (*(site_details[u][c].obs[p]))[r] = true; 
}

inline void inc_tru(int r, int u, int c, int p) 
{                                                            
    if (site_details[u][c].tru[p]) (*(site_details[u][c].tru[p]))[r] = true; 
}

void print_pred_details(int u, int c, int p)
{
    int r;

    if (site_details[u][c].obs[p]) {
        fprintf(obs_fp, "F: ");
        for (r = 0; r < num_runs; r++)
            if (is_frun[r] && (*(site_details[u][c].obs[p]))[r] == true)
                fprintf(obs_fp, "%d ", r);
        fprintf(obs_fp, "\nS: ");
        for (r = 0; r < num_runs; r++)
            if (is_srun[r] && (*(site_details[u][c].obs[p]))[r] == true)
                fprintf(obs_fp, "%d ", r);
        fprintf(obs_fp, "\n");
    }

    if (site_details[u][c].tru[p]) {
        fprintf(tru_fp, "F: ");
        for (r = 0; r < num_runs; r++)
            if (is_frun[r] && (*(site_details[u][c].tru[p]))[r] == true)
                fprintf(tru_fp, "%d ", r);
        fprintf(tru_fp, "\nS: ");
        for (r = 0; r < num_runs; r++)
            if (is_srun[r] && (*(site_details[u][c].tru[p]))[r] == true)
                fprintf(tru_fp, "%d ", r);
        fprintf(tru_fp, "\n");
    }

}

void print_site_details(int u, int c)
{
    int p;

    switch (units[u].s[0]) {
    case 'S':
    case 'R':
        for (p = 0; p < 6; p++)
            print_pred_details(u, c, p);
        break;
    case 'B':
        for (p = 0; p < 2; p++)
            print_pred_details(u, c, p);
        break;
    default:
        assert(0);
    }
}

void process_site(FILE* fp, int r, int u, int c)
{
   int x, y, z;

    switch (units[u].s[0]) {
    case 'S':
    case 'R':
        fscanf(fp, "%d %d %d\n", &x, &y, &z);
        if (x + y + z > 0) {
            inc_obs(r, u, c, 0);
            inc_obs(r, u, c, 1);
            inc_obs(r, u, c, 2);
            inc_obs(r, u, c, 3);
            inc_obs(r, u, c, 4);
            inc_obs(r, u, c, 5);
            if (x > 0)
                inc_tru(r, u, c, 0);
            if (y + z > 0)
                inc_tru(r, u, c, 1);
            if (y > 0) 
                inc_tru(r, u, c, 2);
            if (x + z > 0)
                inc_tru(r, u, c, 3);
            if (z > 0)
                inc_tru(r, u, c, 4);
            if (x + y > 0)
                inc_tru(r, u, c, 5);
        }
        break;
    case 'B':
        fscanf(fp, "%d %d\n", &x, &y);
        if (x + y > 0) {
            inc_obs(r, u, c, 0);
            inc_obs(r, u, c, 1);
            if (x > 0) inc_tru(r, u, c, 0);
            if (y > 0) inc_tru(r, u, c, 1);
        }
        break;
    default:
        assert(0);
    }
}

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-s")) {
            i++;
            sruns_file = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-f")) {
            i++;
            fruns_file = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-pa")) {
            i++;
            preds_file = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-cr")) {
            i++;
            compact_report_path_fmt = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-tru")) {
            i++;
            tru_file = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-obs")) {
            i++;
            obs_file = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-h")) {
            printf("Usage: compute-obs-tru -s <sruns-file> -f <fruns-file> -pa <preds-abbr-file> -cr <compact-report-path-fmt> -obs <obs-file> -tru <tru-file>\n");
            exit(0);
        }
        printf("Illegal option: %s\n", argv[i]);
        exit(1);
    }
    if (!sruns_file || !fruns_file || !preds_file || !compact_report_path_fmt || !obs_file || !tru_file) {
        printf("Incorrect usage; try -h\n");
        exit(1);
    }
}

int main(int argc, char** argv)
{
    int u, c, p;

    process_cmdline(argc, argv);

    classify_runs(sruns_file, fruns_file);

    obs_fp = fopen(obs_file, "w");
    assert(obs_fp);
    tru_fp = fopen(tru_file, "w");
    assert(tru_fp);

    for (u = 0; u < NUM_UNITS; u++) {
        site_details[u] = new (site_details_t) [units[u].c];
        assert(site_details[u]);
    }

    preds_fp = fopen(preds_file, "r");
    assert(preds_fp);
    while (1) {
        fscanf(preds_fp, "%d %d %d", &u, &c, &p);
        if (feof(preds_fp))
            break;
        assert(u >= 0 && u < NUM_UNITS );
        assert(c >= 0 && c < units[u].c);
        assert(p >= 0 && p < 6);
        site_details[u][c].obs[p] = new bit_vector(num_runs);
        assert(site_details[u][c].obs[p]);
        site_details[u][c].tru[p] = new bit_vector(num_runs);
        assert(site_details[u][c].tru[p]);
    }
    fclose(preds_fp);

    scaffold(compact_report_path_fmt, process_site);

    for (u = 0; u < NUM_UNITS; u++) 
        for (c = 0; c < units[u].c; c++) 
            print_site_details(u, c);

    fclose(obs_fp);
    fclose(tru_fp);
    return 0;
}

