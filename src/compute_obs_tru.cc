#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include "classify_runs.h"
#include "units.h"
#include "utils.h"

using namespace std;

struct site_info_t {
    bit_vector *obs[6], *tru[6];
    site_info_t()
    {
	for (int i = 0; i < 6; i++)
	    obs[i] = tru[i] = NULL;
    }
};

vector<vector<site_info_t> > site_info;

char* sruns_txt_file = NULL;
char* fruns_txt_file = NULL;
char* preds_txt_file = NULL;
char* compact_report_path_fmt = NULL;
char* obs_txt_file = NULL;
char* tru_txt_file = NULL;

FILE* pfp = NULL;
FILE* ofp = NULL;
FILE* tfp = NULL;

inline void inc_obs(int r, int u, int c, int p)
{
    if (site_info[u][c].obs[p]) (*(site_info[u][c].obs[p]))[r] = true;
}

inline void inc_tru(int r, int u, int c, int p)
{
    if (site_info[u][c].tru[p]) (*(site_info[u][c].tru[p]))[r] = true;
}

void print_pred_info(int u, int c, int p)
{
    int r;

    if (site_info[u][c].obs[p]) {
	fputs("F: ", ofp);
	for (r = 0; r < num_runs; r++)
	    if (is_frun[r] && (*(site_info[u][c].obs[p]))[r] == true)
		fprintf(ofp, "%d ", r);
	fputs("\nS: ", ofp);
	for (r = 0; r < num_runs; r++)
	    if (is_srun[r] && (*(site_info[u][c].obs[p]))[r] == true)
		fprintf(ofp, "%d ", r);
	fputc('\n', ofp);
    }

    if (site_info[u][c].tru[p]) {
	fputs("F: ", tfp);
	for (r = 0; r < num_runs; r++)
	    if (is_frun[r] && (*(site_info[u][c].tru[p]))[r] == true)
		fprintf(tfp, "%d ", r);
	fputs("\nS: ", tfp);
	for (r = 0; r < num_runs; r++)
	    if (is_srun[r] && (*(site_info[u][c].tru[p]))[r] == true)
		fprintf(tfp, "%d ", r);
	fputc('\n', tfp);
    }
}

void print_site_info(int u, int c)
{
    int p;

    switch (units[u].scheme_code) {
    case 'S':
    case 'R':
	for (p = 0; p < 6; p++)
	    print_pred_info(u, c, p);
	break;
    case 'B':
	for (p = 0; p < 2; p++)
	    print_pred_info(u, c, p);
	break;
    default:
	assert(0);
    }
}

int cur_run;

void process_s_site(int u, int c, int x, int y, int z)
{
	if (x + y + z > 0) {
		inc_obs(cur_run, u, c, 0);
		inc_obs(cur_run, u, c, 1);
		inc_obs(cur_run, u, c, 2);
		inc_obs(cur_run, u, c, 3);
		inc_obs(cur_run, u, c, 4);
		inc_obs(cur_run, u, c, 5);
		if (x > 0)
			inc_tru(cur_run, u, c, 0);
		if (y + z > 0)
			inc_tru(cur_run, u, c, 1);
		if (y > 0)
			inc_tru(cur_run, u, c, 2);
		if (x + z > 0)
			inc_tru(cur_run, u, c, 3);
		if (z > 0)
			inc_tru(cur_run, u, c, 4);
		if (x + y > 0)
			inc_tru(cur_run, u, c, 5);
	}
}

void process_b_site(int u, int c, int x, int y)
{
	if (x + y > 0) {
		inc_obs(cur_run, u, c, 0);
		inc_obs(cur_run, u, c, 1);
		if (x > 0) inc_tru(cur_run, u, c, 0);
		if (y > 0) inc_tru(cur_run, u, c, 1);
	}
}

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-s")) {
	    i++;
	    sruns_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-f")) {
	    i++;
	    fruns_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-p")) {
	    i++;
	    preds_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-cr")) {
	    i++;
	    compact_report_path_fmt = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-ot")) {
	    i++;
	    obs_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-tt")) {
	    i++;
	    tru_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: compute-obs-tru <options>\n"
                 "(r) -s  <sruns-txt-file>\n"
                 "(r) -f  <fruns-txt-file>\n"
                 "(r) -p  <preds-txt-file>\n"
                 "(r) -cr <compact-report-path-fmt>\n"
                 "(w) -ot <obs-txt-file>\n"
                 "(w) -tt <tru-txt-file>\n");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }
    if (!sruns_txt_file || 
        !fruns_txt_file ||
        !preds_txt_file ||
        !compact_report_path_fmt ||
        !obs_txt_file || 
        !tru_txt_file) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

int main(int argc, char** argv)
{
    int u, c, p;

    process_cmdline(argc, argv);

    classify_runs(sruns_txt_file, fruns_txt_file);

    ofp = fopen(obs_txt_file, "w");
    assert(ofp);
    tfp = fopen(tru_txt_file, "w");
    assert(tfp);

    site_info.resize(num_units);
    for (u = 0; u < num_units; u++) site_info[u].resize(units[u].num_sites);

    pfp = fopen(preds_txt_file, "r");
    assert(pfp);
    while (1) {
	fscanf(pfp, "%d %d %d", &u, &c, &p);
	if (feof(pfp))
	    break;
	assert(u >= 0 && u < num_units);
	assert(c >= 0 && c < units[u].num_sites);
	assert(p >= 0 && p < 6);
	site_info[u][c].obs[p] = new bit_vector(num_runs);
	assert(site_info[u][c].obs[p]);
	site_info[u][c].tru[p] = new bit_vector(num_runs);
	assert(site_info[u][c].tru[p]);
    }
    fclose(pfp);

    for (cur_run = 0; cur_run < num_runs; cur_run++) {
        if (!is_srun[cur_run] && !is_frun[cur_run])
            continue;

        printf("r %d\n", cur_run);
        char s[1000];
        sprintf(s, compact_report_path_fmt, cur_run);

        FILE* fp = fopen(s, "r");
        assert(fp);

        process_report(fp, process_s_site, process_s_site, process_b_site);

        fclose(fp);
    }

    for (u = 0; u < num_units; u++)
	for (c = 0; c < units[u].num_sites; c++)
	    print_site_info(u, c);

    fclose(ofp);
    fclose(tfp);
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
