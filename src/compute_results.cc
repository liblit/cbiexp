#include <cassert>
#include <cmath>
#include <ctype.h>
#include <numeric>
#include <vector>
#include "CompactReport.h"
#include "Confidence.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "StaticSiteInfo.h"
#include "classify_runs.h"
#include "fopen.h"
#include "utils.h"

using namespace std;


static StaticSiteInfo staticSiteInfo;

enum { LT, GEQ, EQ, NEQ, GT, LEQ };

struct site_info_t {
    int S[18], F[18];
    int os, of;
    bool retain[18];
    int lb[18];
    site_info_t()
    {
	os = of = 0;
	for (int i = 0; i < 18; i++) {
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
static int num_f_preds = 0;
static int num_g_preds = 0;

/****************************************************************************
 * Procedures for printing retained predicates
 ***************************************************************************/

static void print_pred(FILE* fp, int u, int c, int p, int site)
{
    int s  = site_info[u][c].S[p];
    int f  = site_info[u][c].F[p];
    int os = site_info[u][c].os;
    int of = site_info[u][c].of;

    pred_stat ps = compute_pred_stat(s, f, os, of, Confidence::level);

    fprintf(fp, "%c %d %d %d %d %g %g %g %g %d %d %d %d\n",
	staticSiteInfo.unit(u).scheme_code,
	u, c, p, site,
	ps.lb, ps.in, ps.fs, ps.co,
	s, f, os, of);
}

static void print_retained_preds()
{
    unsigned u, c;
    int p, site = 0;

    FILE* fp = fopenWrite(PredStats::filename);
    assert(fp);

    for (u = 0; u < staticSiteInfo.unitCount; u++) {
	const unit_t &unit = staticSiteInfo.unit(u);
	for (c = 0; c < unit.num_sites; c++, site++) {
	    switch (unit.scheme_code) {
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
	    case 'F':
		for (p = 0; p < 18; p++)
		    if (site_info[u][c].retain[p]) {
			num_f_preds++;
			print_pred(fp, u, c, p, site);
		    }
		break;
	    case 'G':
		for (p = 0; p < 8; p++)
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
protected:
    void handleSite(const SiteCoords &, vector<unsigned> &);
};


void Reader::handleSite(const SiteCoords &coords, vector<unsigned> &counts)
{
    assert(coords.unitIndex < staticSiteInfo.unitCount);
    assert(coords.unitIndex < site_info.size());
    assert(coords.siteOffset < staticSiteInfo.unit(coords.unitIndex).num_sites);
    assert(coords.siteOffset < site_info[coords.unitIndex].size());

    obs(cur_run, coords);

    const size_t size = counts.size();
    if (size == 2)
	for (unsigned predicate = 0; predicate < counts.size(); ++predicate) {
	    if (counts[predicate])
		inc(cur_run, coords, predicate);
	}
    else {
	const unsigned sum = accumulate(counts.begin(), counts.end(), 0);
	for (unsigned predicate = 0; predicate < counts.size(); ++predicate) {
	    if (counts[predicate])
		inc(cur_run, coords, 2 * predicate);
	    if (sum - counts[predicate])
		inc(cur_run, coords, 2 * predicate + 1);
	}
    }
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
    for (unsigned u = 0; u < staticSiteInfo.unitCount; u++) {
	const unit_t &unit = staticSiteInfo.unit(u);
	for (unsigned c = 0; c < unit.num_sites; c++) {
	    int numPreds;
	    switch (unit.scheme_code) {
	    case 'B':
		numPreds = 2;
		break;
	    case 'F':
		numPreds = 18;
		break;
	    case 'G':
		numPreds = 8;
		break;
	    case 'R':
	    case 'S':
		numPreds = 6;
		break;
	    default:
		assert(0);
	    }
	    for (int p = 0; p < numPreds; p++)
		cull(u, c, p);
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

    for (u = 0; u < staticSiteInfo.unitCount; u++) {
	const unit_t &unit = staticSiteInfo.unit(u);
	for (c = 0; c < unit.num_sites; c++) {
	    switch (unit.scheme_code) {
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
	    case 'F':
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
    if (!site_info[u][c].retain[p])
	return false;

    const site_t site = staticSiteInfo.site(s + c);
    return isdigit(site.args[1][0]) || site.args[1][0] != '-';
}

void checkG(unsigned u, unsigned c, int p, int s)
{
    const unit_t &unit = staticSiteInfo.unit(u);
    const site_t siteC = staticSiteInfo.site(s + c);

    for (unsigned d = 0; d < unit.num_sites; d++) {
	const site_t siteD = staticSiteInfo.site(s + d);
	if (c != d &&
	    siteC.line == siteD.line &&
	    strcmp(siteC.args[0], siteD.args[0]) == 0 &&
	    atoi(siteC.args[1]) < atoi(siteD.args[1]) &&
	    ((is_eligible(u, d, GT , s) && have_equal_increase(u, c, p, u, d, GT )) ||
	     (is_eligible(u, d, GEQ, s) && have_equal_increase(u, c, p, u, d, GEQ)))) {
	    site_info[u][c].retain[p] = false;
	    break;
	}
    }
}

void checkL(unsigned u, unsigned c, int p, int s)
{
    const unit_t &unit = staticSiteInfo.unit(u);
    const site_t siteC = staticSiteInfo.site(s + c);

    for (unsigned d = 0; d < unit.num_sites; d++) {
	const site_t siteD = staticSiteInfo.site(s + d);
	if (c != d &&
	    siteC.line == siteD.line &&
	    strcmp(siteC.args[0], siteD.args[0]) == 0 &&
	    atoi(siteC.args[1]) > atoi(siteD.args[1]) &&
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

    for (s = 0, u = 0; u < staticSiteInfo.unitCount; s += staticSiteInfo.unit(u).num_sites, u++) {
	const unit_t &unit = staticSiteInfo.unit(u);
	if (unit.scheme_code == 'S') {
	    for (c = 0; c < unit.num_sites; c++) {
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
	{ &ReportReader::argp, 0, 0, 0 },
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

    site_info.resize(staticSiteInfo.unitCount);
    for (unsigned u = 0; u < staticSiteInfo.unitCount; u++)
	site_info[u].resize(staticSiteInfo.unit(u).num_sites);

    Progress::Bounded progress("computing results", NumRuns::count());
    for (cur_run = NumRuns::begin; cur_run < NumRuns::end; cur_run++) {
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
