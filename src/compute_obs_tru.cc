#include <argp.h>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include "CompactReport.h"
#include "PredStats.h"
#include "Progress.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "classify_runs.h"
#include "fopen.h"
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

char* obs_txt_file = "obs.txt";
char* tru_txt_file = "tru.txt";

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
    case 'G':
	for (p = 0; p < 4; p++)
	    print_pred_info(u, c, p);
	break;
    default:
	assert(0);
    }
}

int cur_run;

class Reader : public ReportReader
{
public:
    void branchesSite(    unsigned unitIndex, unsigned siteIndex, unsigned, unsigned) const;
    void gObjectUnrefSite(unsigned unitIndex, unsigned siteIndex, unsigned, unsigned, unsigned, unsigned) const;
    void returnsSite(     unsigned unitIndex, unsigned siteIndex, unsigned, unsigned, unsigned) const;
    void scalarPairsSite( unsigned unitIndex, unsigned siteIndex, unsigned, unsigned, unsigned) const;

private:
    void tripleSite( unsigned unitIndex, unsigned siteIndex, unsigned, unsigned, unsigned) const;
};


void Reader::tripleSite(unsigned u, unsigned c, unsigned x, unsigned y, unsigned z) const
{
    assert(x || y || z);
    inc_obs(cur_run, u, c, 0);
    inc_obs(cur_run, u, c, 1);
    inc_obs(cur_run, u, c, 2);
    inc_obs(cur_run, u, c, 3);
    inc_obs(cur_run, u, c, 4);
    inc_obs(cur_run, u, c, 5);
    if (x)
	inc_tru(cur_run, u, c, 0);
    if (y || z)
	inc_tru(cur_run, u, c, 1);
    if (y)
	inc_tru(cur_run, u, c, 2);
    if (x || z)
	inc_tru(cur_run, u, c, 3);
    if (z)
	inc_tru(cur_run, u, c, 4);
    if (x || y)
	inc_tru(cur_run, u, c, 5);
}


void Reader::scalarPairsSite(unsigned u, unsigned c, unsigned x, unsigned y, unsigned z) const
{
    tripleSite(u, c, x, y, z);
}


void Reader::returnsSite(unsigned u, unsigned c, unsigned x, unsigned y, unsigned z) const
{
    tripleSite(u, c, x, y, z);
}


void Reader::branchesSite(unsigned u, unsigned c, unsigned x, unsigned y) const
{
    assert(x || y);
    inc_obs(cur_run, u, c, 0);
    inc_obs(cur_run, u, c, 1);
    if (x) inc_tru(cur_run, u, c, 0);
    if (y) inc_tru(cur_run, u, c, 1);
}


void Reader::gObjectUnrefSite(unsigned u, unsigned c, unsigned x, unsigned y, unsigned z, unsigned w) const
{
    assert(x || y || z || w);
    inc_obs(cur_run, u, c, 0);
    inc_obs(cur_run, u, c, 1);
    inc_obs(cur_run, u, c, 2);
    inc_obs(cur_run, u, c, 3);
    if (x) inc_tru(cur_run, u, c, 0);
    if (y) inc_tru(cur_run, u, c, 1);
    if (z) inc_tru(cur_run, u, c, 2);
    if (w) inc_tru(cur_run, u, c, 3);
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
};


int main(int argc, char** argv)
{
    unsigned u, c;

    process_cmdline(argc, argv);

    classify_runs();

    ofp = fopenWrite(obs_txt_file);
    tfp = fopenWrite(tru_txt_file);

    site_info.resize(num_units);
    for (u = 0; u < num_units; u++) site_info[u].resize(units[u].num_sites);

    pfp = fopenRead(PredStats::filename);
    while (1) {
	pred_info pi;
	const bool got = read_pred_full(pfp, pi);
	if (!got)
	    break;
	site_info[pi.u][pi.c].obs[pi.p] = new bit_vector(num_runs);
	assert(site_info[pi.u][pi.c].obs[pi.p]);
	site_info[pi.u][pi.c].tru[pi.p] = new bit_vector(num_runs);
	assert(site_info[pi.u][pi.c].tru[pi.p]);
    }
    fclose(pfp);

    Progress progress("computing obs and tru", num_runs);
    for (cur_run = 0; cur_run < num_runs; cur_run++) {
	progress.step();
	if (!is_srun[cur_run] && !is_frun[cur_run])
	    continue;

	Reader().read(cur_run);
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
