#include <cassert>
#include <cmath>
#include <ctype.h>
#include <vector>
#include "CompactReport.h"
#include "Confidence.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "classify_runs.h"
#include "fopen.h"
#include "sites.h"
#include "units.h"
#include "utils.h"

using namespace std;


enum { LT, GEQ, EQ, NEQ, GT, LEQ };

struct site_info_t {
    int S[6], F[6];
    int os, of;
    bool retain[6];
    int lb[6];
    site_info_t()
    {
	os = of = 0;
	for (int i = 0; i < 6; i++) {
	    S[i] = 0;
	    F[i] = 0;
	    retain[i] = false;
	    lb[i] = -1;
	}
    }
};

static vector<vector<site_info_t> > site_info;

static int num_s_preds = 0;
static int num_r_preds = 0;
static int num_b_preds = 0;
static int num_g_preds = 0;

/****************************************************************************
 * Procedures for printing retained predicates
 ***************************************************************************/

inline void print_pred(FILE* fp, int u, int c, int p, int site)
{
    int s  = site_info[u][c].S[p];
    int f  = site_info[u][c].F[p];
    int os = site_info[u][c].os;
    int of = site_info[u][c].of;

    pred_stat ps = compute_pred_stat(s, f, os, of, Confidence::level);

    assert(units[u].scheme_code == sites[site].scheme_code);

    fprintf(fp, "%c %d %d %d %d %g %g %g %g %d %d %d %d\n",
	units[u].scheme_code,
	u, c, p, site,
	ps.lb, ps.in, ps.fs, ps.co,
	s, f, os, of);
}

void print_retained_preds()
{
    unsigned u, c;
    int p, site = 0;

    FILE* fp = fopenWrite(PredStats::filename);
    assert(fp);

    for (u = 0; u < num_units; u++) {
	for (c = 0; c < units[u].num_sites; c++, site++) {
	    switch (units[u].scheme_code) {
	    case 'S':
		for (p = 0; p < 6; p++)
		    if (site_info[u][c].retain[p]) {
			num_s_preds++;
			print_pred(fp, u, c, p, site);
		    }
		break;
	    case 'R':
		for (p = 0; p < 6; p++)
		    if (site_info[u][c].retain[p]) {
			num_r_preds++;
			print_pred(fp, u, c, p, site);
		    }
		break;
	    case 'B':
		for (p = 0; p < 2; p++)
		    if (site_info[u][c].retain[p]) {
			num_b_preds++;
			print_pred(fp, u, c, p, site);
		    }
		break;
	    case 'G':
		for (p = 0; p < 4; p++)
		    if (site_info[u][c].retain[p]) {
			num_g_preds++;
			print_pred(fp, u, c, p, site);
		    }
		break;
	    default:
		assert(0);
	    }
	}
    }

    fclose(fp);
}

/****************************************************************************
 * Procedures for deciding whether to retain/discard
 * each instrumented predicate
 ***************************************************************************/

inline void inc(int r, const SiteCoords &coords, unsigned p)
{
    if (is_srun[r])
	site_info[coords.unitIndex][coords.siteOffset].S[p]++;
    else if (is_frun[r])
	site_info[coords.unitIndex][coords.siteOffset].F[p]++;
}

inline void obs(int r, const SiteCoords &coords)
{
    if (is_srun[r])
	site_info[coords.unitIndex][coords.siteOffset].os++;
    else if (is_frun[r])
	site_info[coords.unitIndex][coords.siteOffset].of++;
}

unsigned cur_run;


class Reader : public ReportReader
{
public:
    void branchesSite(    const SiteCoords &, unsigned, unsigned) const;
    void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned) const;
    void returnsSite(     const SiteCoords &, unsigned, unsigned, unsigned) const;
    void scalarPairsSite( const SiteCoords &, unsigned, unsigned, unsigned) const;

private:
    void tripleSite(      const SiteCoords &, unsigned, unsigned, unsigned) const;
};


void Reader::tripleSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) const
{
    assert(x || y || z);
    obs(cur_run, coords);
    if (x)
	inc(cur_run, coords, 0);
    if (y || z)
	inc(cur_run, coords, 1);
    if (y)
	inc(cur_run, coords, 2);
    if (x || z)
	inc(cur_run, coords, 3);
    if (z)
	inc(cur_run, coords, 4);
    if (x || y)
	inc(cur_run, coords, 5);
}


void Reader::scalarPairsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) const
{
    tripleSite(coords, x, y, z);
}


void Reader::returnsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) const
{
    tripleSite(coords, x, y, z);
}


void Reader::branchesSite(const SiteCoords &coords, unsigned x, unsigned y) const
{
    assert(x || y);
    obs(cur_run, coords);
    if (x) inc(cur_run, coords, 0);
    if (y) inc(cur_run, coords, 1);
}


void Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z, unsigned w) const
{
    assert(x || y || z || w);
    obs(cur_run, coords);
    if (x) inc(cur_run, coords, 0);
    if (y) inc(cur_run, coords, 1);
    if (z) inc(cur_run, coords, 2);
    if (w) inc(cur_run, coords, 3);
}


inline void cull(int u, int c, int p)
{
    int s = site_info[u][c].S[p];
    int f = site_info[u][c].F[p];
    pred_stat ps = compute_pred_stat(s, f, site_info[u][c].os, site_info[u][c].of, Confidence::level);

    if (retain_pred(s, f, ps.lb)) {
	site_info[u][c].retain[p] = true;
	site_info[u][c].lb[p] = (int) rint(ps.lb * 100);
    }
}

void cull_preds()
{
    unsigned u, c;
    int p;

    for (u = 0; u < num_units; u++) {
	for (c = 0; c < units[u].num_sites; c++) {
	    switch (units[u].scheme_code) {
	    case 'S':
		for (p = 0; p < 6; p++)
		    cull(u, c, p);
		break;
	    case 'R':
		for (p = 0; p < 6; p++)
		    cull(u, c, p);
		break;
	    case 'B':
		for (p = 0; p < 2; p++)
		    cull(u, c, p);
		break;
	    case 'G':
		for (p = 0; p < 4; p++)
		    cull(u, c, p);
		break;
	    default:
		assert(0);
	    }
	}
    }
}

inline bool have_equal_increase(int u1, int c1, int p1, int u2, int c2, int p2)
{
    return site_info[u1][c1].lb[p1] == site_info[u2][c2].lb[p2];
}

void cull_preds_aggressively1()
{
    unsigned u, c;

    for (u = 0; u < num_units; u++) {
	for (c = 0; c < units[u].num_sites; c++) {
	    switch (units[u].scheme_code) {
	    case 'S':
	    case 'R':
	    {
		site_info_t& s = site_info[u][c];

		if (s.retain[LEQ] &&
		    s.retain[LT ] &&
		    s.retain[EQ ] &&
		    have_equal_increase(u, c, LT, u, c, LEQ) &&
		    have_equal_increase(u, c, EQ, u, c, LEQ)) {
		    s.retain[LT] = s.retain[EQ] = false;
		    break;
		}
		if (s.retain[GEQ] &&
		    s.retain[GT ] &&
		    s.retain[EQ ] &&
		    have_equal_increase(u, c, GT, u, c, GEQ) &&
		    have_equal_increase(u, c, EQ, u, c, GEQ)) {
		    s.retain[GT] = s.retain[EQ] = false;
		    break;
		}
		if (s.retain[NEQ] &&
		    s.retain[LT ] &&
		    s.retain[GT ] &&
		    have_equal_increase(u, c, LT, u, c, NEQ) &&
		    have_equal_increase(u, c, GT, u, c, NEQ)) {
		    s.retain[LT] = s.retain[GT] = false;
		    break;
		}
		if (s.retain[LT ] &&
		    s.retain[LEQ] &&
		    s.retain[NEQ] &&
		    have_equal_increase(u, c, LEQ, u, c, LT) &&
		    have_equal_increase(u, c, NEQ, u, c, LT)) {
		    s.retain[LEQ] = s.retain[NEQ] = false;
		    break;
		}
		if (s.retain[GT ] &&
		    s.retain[GEQ] &&
		    s.retain[NEQ] &&
		    have_equal_increase(u, c, GEQ, u, c, GT) &&
		    have_equal_increase(u, c, NEQ, u, c, GT)) {
		    s.retain[GEQ] = s.retain[NEQ] = false;
		    break;
		}
		if (s.retain[EQ ] &&
		    s.retain[LEQ] &&
		    s.retain[GEQ] &&
		    have_equal_increase(u, c, LEQ, u, c, EQ) &&
		    have_equal_increase(u, c, GEQ, u, c, EQ)) {
		    s.retain[LEQ] = s.retain[GEQ] = false;
		    break;
		}
		if (s.retain[LT ] &&
		    s.retain[LEQ] &&
		    have_equal_increase(u, c, LT, u, c, LEQ)) {
		    s.retain[LEQ] = false;
		    break;
		}
		if (s.retain[LT ] &&
		    s.retain[NEQ] &&
		    have_equal_increase(u, c, LT, u, c, NEQ)) {
		    s.retain[NEQ] = false;
		    break;
		}
		if (s.retain[GT ] &&
		    s.retain[GEQ] &&
		    have_equal_increase(u, c, GT, u, c, GEQ)) {
		    s.retain[GEQ] = false;
		    break;
		}
		if (s.retain[GT ] &&
		    s.retain[NEQ] &&
		    have_equal_increase(u, c, GT, u, c, NEQ)) {
		    s.retain[NEQ] = false;
		    break;
		}
		if (s.retain[EQ ] &&
		    s.retain[LEQ] &&
		    have_equal_increase(u, c, EQ, u, c, LEQ)) {
		    s.retain[LEQ] = false;
		    break;
		}
		if (s.retain[EQ ] &&
		    s.retain[GEQ] &&
		    have_equal_increase(u, c, EQ, u, c, GEQ)) {
		    s.retain[GEQ] = false;
		    break;
		}
		break;
	    }
	    case 'B':
	    case 'G':
		break;
	    default:
		assert(0);
	    }
	}
    }
}

inline bool is_eligible(int u, int c, int p, int s)
{
    return site_info[u][c].retain[p] &&
	   (isdigit(sites[s + c].args[1][0]) || sites[s + c].args[1][0] != '-');
}

void checkG(unsigned u, unsigned c, int p, int s)
{
    for (unsigned d = 0; d < units[u].num_sites; d++) {
	if (c != d &&
	    sites[s + c].line == sites[s + d].line &&
	    strcmp(sites[s + c].args[0], sites[s + d].args[0]) == 0 &&
	    atoi(sites[s + c].args[1]) < atoi(sites[s + d].args[1]) &&
	    ((is_eligible(u, d, GT , s) && have_equal_increase(u, c, p, u, d, GT )) ||
	     (is_eligible(u, d, GEQ, s) && have_equal_increase(u, c, p, u, d, GEQ)))) {
	    site_info[u][c].retain[p] = false;
	    break;
	}
    }
}

void checkL(unsigned u, unsigned c, int p, int s)
{
    for (unsigned d = 0; d < units[u].num_sites; d++) {
	if (c != d &&
	    sites[s + c].line == sites[s + d].line &&
	    strcmp(sites[s + c].args[0], sites[s + d].args[0]) == 0 &&
	    atoi(sites[s + c].args[1]) > atoi(sites[s + d].args[1]) &&
	    ((is_eligible(u, d, LT , s) && have_equal_increase(u, c, p, u, d, LT )) ||
	     (is_eligible(u, d, LEQ, s) && have_equal_increase(u, c, p, u, d, LEQ)))) {
	    site_info[u][c].retain[p] = false;
	    break;
	}
    }
}

void cull_preds_aggressively2()
{
    unsigned u, c, s;

    for (s = 0, u = 0; u < num_units; s += units[u].num_sites, u++) {
	if (units[u].scheme_code == 'S') {
	    for (c = 0; c < units[u].num_sites; c++) {
		if (is_eligible(u, c, GT , s))
		    checkG(u, c, GT , s);
		if (is_eligible(u, c, GEQ, s))
		    checkG(u, c, GEQ, s);
		if (is_eligible(u, c, LT , s))
		    checkL(u, c, LT , s);
		if (is_eligible(u, c, LEQ, s))
		    checkL(u, c, LEQ, s);
	    }
	}
    }
}

/****************************************************************************
 * Processing command line options
 ***************************************************************************/


void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &CompactReport::argp, 0, 0, 0 },
	{ &Confidence::argp, 0, 0, 0 },
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}

/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    classify_runs();

    site_info.resize(num_units);
    for (unsigned u = 0; u < num_units; u++)
	site_info[u].resize(units[u].num_sites);

    const unsigned num_runs = NumRuns::value();
    Progress::Bounded progress("computing results", num_runs);
    for (cur_run = 0; cur_run < num_runs; cur_run++) {
	progress.step();
	if (!is_srun[cur_run] && !is_frun[cur_run])
	    continue;

	Reader().read(cur_run);
    }

    cull_preds();
    cull_preds_aggressively1();
    cull_preds_aggressively2();

    print_retained_preds();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
