// Usage: compute_obs_tru < preds_list in <u c p> format
// Creates files OBS_FILE and TRU_FILE

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include "def.h"
#include UNITS_HDR_FILE
#include "classify_runs.h"

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

FILE* obs_fp = NULL;
FILE* tru_fp = NULL;

#define inc_obs(r, u, c, p)                                  \
{                                                            \
    if (site_details[u][c].obs[p]) (*(site_details[u][c].obs[p]))[r] = true; \
}

#define inc_tru(r, u, c, p)                                  \
{                                                            \
    if (site_details[u][c].tru[p]) (*(site_details[u][c].tru[p]))[r] = true; \
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

int main()
{
    int i, j, k, x, y, z, w, u, c, p;

    classify_runs();

    obs_fp = fopen(OBS_FILE, "w"); assert(obs_fp);
    tru_fp = fopen(TRU_FILE, "w"); assert(tru_fp);

    for (u = 0; u < NUM_UNITS; u++) {
        site_details[u] = new (site_details_t) [units[u].c];
        assert(site_details[u]);
    }

    while (1) {
        fscanf(stdin, "%d %d %d", &u, &c, &p);
        if (feof(stdin))
            break;
        assert(u >= 0 && u < NUM_UNITS );
        assert(c >= 0 && c < units[u].c);
        assert(p >= 0 && p < 6);
        site_details[u][c].obs[p] = new bit_vector(num_runs);
        assert(site_details[u][c].obs[p]);
        site_details[u][c].tru[p] = new bit_vector(num_runs);
        assert(site_details[u][c].tru[p]);
    }

    for (i = 0; i < num_runs; i++) {
        if (!is_srun[i] && !is_frun[i])
            continue;
        printf("r %d\n", i);
        char file[1000];
        sprintf(file, O_REPORT_PATH_FMT, i);
        FILE* fp = fopen(file, "r");
        assert(fp);

        while (1) {
            fscanf(fp, "%d\n", &j);
            if (feof(fp))
                break;
            switch (units[j].s[0]) {
            case 'S':
            case 'R':
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d %d\n", &x, &y, &z);
                    if (x + y + z > 0) {
                        inc_obs(i, j, k, 0);
                        inc_obs(i, j, k, 1);
                        inc_obs(i, j, k, 2);
                        inc_obs(i, j, k, 3);
                        inc_obs(i, j, k, 4);
                        inc_obs(i, j, k, 5);
                    }
                    if (x > 0) {
                        inc_tru(i, j, k, 0);
                        inc_tru(i, j, k, 3);
                        inc_tru(i, j, k, 5);
                    }
                    if (y > 0) {
                        inc_tru(i, j, k, 2);
                        inc_tru(i, j, k, 1);
                        inc_tru(i, j, k, 5);
                    }
                    if (z > 0) {
                        inc_tru(i, j, k, 4);
                        inc_tru(i, j, k, 1);
                        inc_tru(i, j, k, 3);
                    }
                }
                break;
            case 'B':
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d\n", &x, &y);
                    if (x + y > 0) {
                        inc_obs(i, j, k, 0);
                        inc_obs(i, j, k, 1);
                    }
                    if (x > 0) inc_tru(i, j, k, 0);
                    if (y > 0) inc_tru(i, j, k, 1);
                }
                break;
            default:
                assert(0);
            }
        }
        fclose(fp);
    }

    for (u = 0; u < NUM_UNITS; u++) 
        for (c = 0; c < units[u].c; c++) 
            print_site_details(u, c);

    fclose(obs_fp);
    fclose(tru_fp);
    return 0;
}

