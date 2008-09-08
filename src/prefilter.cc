#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <memory>
#include <numeric>
#include <vector>
#include "AmplifyReport.h"
#include "Confidence.h"
#include "CullPredicates.h"
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


static auto_ptr<StaticSiteInfo> staticSiteInfo;

enum { LT, GEQ, EQ, NEQ, GT, LEQ };

struct site_info_t {
    int S[18], F[18];
    int OS[18], OF[18];
    bool retain[18];
    site_info_t()
    {
	for (int i = 0; i < 18; i++) {
	    S[i] = 0;
	    F[i] = 0;
	    OS[i] = 0;
	    OF[i] = 0;
	    retain[i] = false;
	}
    }
};

static vector<vector<site_info_t> > site_info;

static int num_s_preds = 0;
static int num_r_preds = 0;
static int num_b_preds = 0;
static int num_f_preds = 0;
static int num_g_preds = 0;

inline pred_stat get_pred_stat(int u, int c, int p)
{
    int s = site_info[u][c].S[p];
    int f = site_info[u][c].F[p];
    int os = site_info[u][c].OS[p];
    int of = site_info[u][c].OF[p];
    return compute_pred_stat(s, f, os, of, Confidence::level);
}

int num_preds(char scheme_code)
{
    switch (scheme_code) {
        case 'B':
            return 2;
            break;
        case 'F':
            return 18;
            break;
        case 'G':
            return 8;
            break;
        case 'R':
        case 'S':
            return 6;
            break;
        default:
            assert(0);
    }
}

/****************************************************************************
 * Procedures for printing retained predicates
 ***************************************************************************/

static void print_pred(FILE* fp, int u, int c, int p, int site)
{

    pred_stat ps = get_pred_stat(u, c, p);

    fprintf(fp, "%c %d %d %d %d %g %g %g %g %d %d %d %d\n",
	staticSiteInfo->unit(u).scheme_code,
	u, c, p, site,
	ps.lb, ps.in, ps.fs, ps.co,
        site_info[u][c].S[p],
        site_info[u][c].F[p],
        site_info[u][c].OS[p],
        site_info[u][c].OF[p]);
}

static void print_retained_preds()
{
    unsigned u, c;
    int p, site = 0;

    FILE* fp = fopenWrite(PredStats::filename);
    assert(fp);

    for (u = 0; u < staticSiteInfo->unitCount; u++) {
	const unit_t &unit = staticSiteInfo->unit(u);
	for (c = 0; c < unit.num_sites; c++, site++) {
	    switch (unit.scheme_code) {
	    case 'S':
		for (p = 0; p < num_preds('S'); p++)
		    if (site_info[u][c].retain[p]) {
			num_s_preds++;
			print_pred(fp, u, c, p, site);
		    }
		break;
	    case 'R':
		for (p = 0; p < num_preds('R'); p++)
		    if (site_info[u][c].retain[p]) {
			num_r_preds++;
			print_pred(fp, u, c, p, site);
		    }
		break;
	    case 'B':
		for (p = 0; p < num_preds('B'); p++)
		    if (site_info[u][c].retain[p]) {
			num_b_preds++;
			print_pred(fp, u, c, p, site);
		    }
		break;
	    case 'F':
		for (p = 0; p < num_preds('F'); p++)
		    if (site_info[u][c].retain[p]) {
			num_f_preds++;
			print_pred(fp, u, c, p, site);
		    }
		break;
	    case 'G':
		for (p = 0; p < num_preds('G'); p++)
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

inline void inc(int r, const site_t &site, unsigned p)
{
    if (is_srun[r])
	site_info[site.unit_index][site.site_offset].S[p]++;
    else if (is_frun[r])
	site_info[site.unit_index][site.site_offset].F[p]++;
}

inline void obs(int r, const site_t &site)
{
    if (is_srun[r])
    	for (int i = 0; i < 18; i++)
	    site_info[site.unit_index][site.site_offset].OS[i]++;
    else if (is_frun[r])
	for (int i = 0; i < 18; i++)
	    site_info[site.unit_index][site.site_offset].OF[i]++;
}

inline void obs(int r, const site_t &site, unsigned p) {
    if (is_srun[r])
	site_info[site.unit_index][site.site_offset].OS[p]++;
    else if (is_frun[r])
	site_info[site.unit_index][site.site_offset].OF[p]++;
}

unsigned cur_run;


class Reader : public ReportReader
{
public:
    Reader(const char* filename) : ReportReader(filename) {}
protected:
    void handleSite(const SiteCoords &, vector<count_tp> &);
};


void Reader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
{
    const site_t site = staticSiteInfo->site(coords);

    assert(site.unit_index < staticSiteInfo->unitCount);
    assert(site.unit_index < site_info.size());
    assert(site.site_offset < staticSiteInfo->unit(site.unit_index).num_sites);
    assert(site.site_offset < site_info[site.unit_index].size());

    obs(cur_run, site);

    const size_t size = counts.size();
    if (size == 2)
	for (unsigned predicate = 0; predicate < size; ++predicate) {
	    if (counts[predicate])
		inc(cur_run, site, predicate);
	}
    else {
	const count_tp sum = accumulate(counts.begin(), counts.end(), (count_tp) 0);
	for (unsigned predicate = 0; predicate < size; ++predicate) {
	    if (counts[predicate])
		inc(cur_run, site, 2 * predicate);
	    if (sum - counts[predicate])
		inc(cur_run, site, 2 * predicate + 1);
	}
    }
}

class AmplifyReader : public ReportReader
{
public:
    AmplifyReader(const char* filename) : ReportReader(filename) {}
protected:
    void handleSite(const SiteCoords &, vector<count_tp> &);
    const std::string format(const unsigned) const;
};

void AmplifyReader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
{
    const site_t site = staticSiteInfo->site(coords);

    assert(site.unit_index < staticSiteInfo->unitCount);
    assert(site.unit_index < site_info.size());
    assert(site.site_offset < staticSiteInfo->unit(site.unit_index).num_sites);
    assert(site.site_offset < site_info[site.unit_index].size());

    const size_t size = counts.size();
    if (size == 2) {
      for (unsigned predicate = 0; predicate < size; ++predicate)
          if (counts[predicate]) {
	      inc(cur_run, site, predicate);
          }
    }
    else {
	for (unsigned predicate = 0; predicate < size; ++predicate) {
	    if (counts[predicate]) {
	        inc(cur_run, site, 2 * predicate);
	    	switch(predicate) {
		    case 0:			//unsynthesized LT
			inc(cur_run, site, LEQ);
			break;
		    case 1:			//unsynthesized EQ
		    	inc(cur_run, site, LEQ);
		    	inc(cur_run, site, GEQ);
			break;
 		    case 2:			//unsynthesized GT
		    	inc(cur_run, site, GEQ);
			break;
	  	    default:		
		        assert(0);
		}
            }
	}
    }
}

const string AmplifyReader::format(const unsigned runId) const
{
  return AmplifyReport::format(runId);
}

inline void retain(int u, int c, int p)
{
    site_info[u][c].retain[p] = true;
}

inline void cull(int u, int c, int p)
{
    pred_stat ps = get_pred_stat(u, c, p);

    if (retain_pred(site_info[u][c].S[p], site_info[u][c].F[p], ps.lb)) {
	site_info[u][c].retain[p] = true;
    }
}

typedef void (*pfct)(int,int,int);

void foreach_pred(pfct thef)
{
    for (unsigned u = 0; u < staticSiteInfo->unitCount; u++) {
	const unit_t &unit = staticSiteInfo->unit(u);
	for (unsigned c = 0; c < unit.num_sites; c++) {
	    for (int p = 0; p < num_preds(unit.scheme_code); p++)
		(*thef)(u, c, p);
	}
    }
}

void retain_all_preds()
{
    foreach_pred(&retain);
}

void cull_preds()
{
    foreach_pred(&cull);
}

inline bool have_equal_increase(int u1, int c1, int p1, int u2, int c2, int p2)
{
    pred_stat ps1 = get_pred_stat(u1, c1, p1);
    pred_stat ps2 = get_pred_stat(u2, c2, p2);

    return (int) rint(ps1.lb * 100) == (int) rint(ps2.lb * 100);
}

void cull_preds_aggressively1()
{
    unsigned u, c;

    for (u = 0; u < staticSiteInfo->unitCount; u++) {
	const unit_t &unit = staticSiteInfo->unit(u);
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

    const site_t site = staticSiteInfo->site(s + c);
    return isdigit(site.args[1][0]) || site.args[1][0] != '-';
}

void checkG(unsigned u, unsigned c, int p, int s)
{
    const unit_t &unit = staticSiteInfo->unit(u);
    const site_t siteC = staticSiteInfo->site(s + c);

    for (unsigned d = 0; d < unit.num_sites; d++) {
	const site_t siteD = staticSiteInfo->site(s + d);
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
    const unit_t &unit = staticSiteInfo->unit(u);
    const site_t siteC = staticSiteInfo->site(s + c);

    for (unsigned d = 0; d < unit.num_sites; d++) {
	const site_t siteD = staticSiteInfo->site(s + d);
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

    for (s = 0, u = 0; u < staticSiteInfo->unitCount; s += staticSiteInfo->unit(u).num_sites, u++) {
	const unit_t &unit = staticSiteInfo->unit(u);
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
	{ &AmplifyReport::argp, 0, 0, 0 },
	{ &Confidence::argp, 0, 0, 0 },
        { &CullPredicates::argp, 0, 0, 0 },
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

    staticSiteInfo.reset(new StaticSiteInfo());
    site_info.resize(staticSiteInfo->unitCount);
    for (unsigned u = 0; u < staticSiteInfo->unitCount; u++)
	site_info[u].resize(staticSiteInfo->unit(u).num_sites);

    Reader reader("tru.txt");
    AmplifyReader amp("tru.txt");
    Progress::Bounded progress("computing results", NumRuns::count());
    for (cur_run = NumRuns::begin(); cur_run < NumRuns::end(); cur_run++) {
	progress.step();

	reader.read(cur_run);
        // if (AmplifyReport::amplify) amp.read(cur_run);
    }

    if (CullPredicates::cull) {
        cull_preds();
        cull_preds_aggressively1();
        cull_preds_aggressively2();
    }
    else {
        retain_all_preds();
    }

    print_retained_preds();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
