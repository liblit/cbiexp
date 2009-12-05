#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <memory>
#include <numeric>
#include <vector>
#include "Confidence.h"
#include "CullPredicates.h"
#include "DatabaseFile.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
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

static vector< site_info_t > site_info;

static int num_s_preds = 0;
static int num_r_preds = 0;
static int num_b_preds = 0;
static int num_f_preds = 0;
static int num_g_preds = 0;
static int num_y_preds = 0;
static int num_a_preds = 0;
static int num_z_preds = 0;
static int num_c_preds = 0;
static int num_w_preds = 0;


inline pred_stat get_pred_stat(int si, int p)
{
    int s = site_info[si].S[p];
    int f = site_info[si].F[p];
    int os = site_info[si].OS[p];
    int of = site_info[si].OF[p];
    return compute_pred_stat(s, f, os, of, Confidence::level);
}

unsigned int num_preds(char scheme_code)
{
    switch (scheme_code) {
        case 'A':
        case 'B':
        case 'Y':
        case 'Z':
        case 'C':
        case 'W':
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

static void print_pred(FILE* fp, int si, int p)
{

    const site_t site = staticSiteInfo->site(si);
    pred_stat ps = get_pred_stat(si, p);

    fprintf(fp, "%c %d %d %g %g %g %g %d %d %d %d\n",
        site.scheme_code,
        si, p,
        ps.lb, ps.in, ps.fs, ps.co,
        site_info[si].S[p],
        site_info[si].F[p],
        site_info[si].OS[p],
        site_info[si].OF[p]);
}

static void print_retained_preds()
{
    unsigned int p, si = 0;

    FILE* fp = fopenWrite(PredStats::filename);
    assert(fp);

    for (si = 0; si < staticSiteInfo->siteCount; si++) {
        const site_t site = staticSiteInfo->site(si);
        switch(site.scheme_code) {
            case 'S':
                for (p = 0; p < num_preds('S'); p++)
                    if (site_info[si].retain[p]) {
                        num_s_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            case 'R':
                for (p = 0; p < num_preds('R'); p++)
                    if (site_info[si].retain[p]) {
                        num_r_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            case 'B':
                for (p = 0; p < num_preds('B'); p++)
                    if (site_info[si].retain[p]) {
                        num_b_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            case 'F':
                for (p = 0; p < num_preds('F'); p++)
                    if (site_info[si].retain[p]) {
                        num_f_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            case 'G':
                for (p = 0; p < num_preds('G'); p++)
                    if (site_info[si].retain[p]) {
                        num_g_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            case 'A':
                for (p = 0; p < num_preds('A'); p++)
                    if (site_info[si].retain[p]) {
                        num_a_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            case 'Y':
                for (p = 0; p < num_preds('Y'); p++)
                    if (site_info[si].retain[p]) {
                        num_y_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            case 'Z':
                for (p = 0; p < num_preds('Z'); p++)
                    if (site_info[si].retain[p]) {
                        num_z_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            case 'C':
                for (p = 0; p < num_preds('C'); p++)
                    if (site_info[si].retain[p]) {
                        num_c_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            case 'W':
                for (p = 0; p < num_preds('W'); p++)
                    if (site_info[si].retain[p]) {
                        num_w_preds++;
                        print_pred(fp, si, p);
                    }
                break;
            default:
                assert(0);
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
        site_info[coords.siteIndex].S[p]++;
    else if (is_frun[r])
        site_info[coords.siteIndex].F[p]++;
}

inline void obs(int r, const SiteCoords &coords)
{
    if (is_srun[r])
    	for (int i = 0; i < 18; i++)
            site_info[coords.siteIndex].OS[i]++;
    else if (is_frun[r])
        for (int i = 0; i < 18; i++)
            site_info[coords.siteIndex].OF[i]++;
}

inline void obs(int r, const SiteCoords &coords, unsigned p) {
    if (is_srun[r])
        site_info[coords.siteIndex].OS[p]++;
    else if (is_frun[r])
        site_info[coords.siteIndex].OF[p]++;
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

    const size_t size = counts.size();
    assert (size == 2 || size == num_preds(site.scheme_code) / 2);

    obs(cur_run, coords);

    if (size == 2)
        for (unsigned predicate = 0; predicate < size; ++predicate) {
            if (counts[predicate])
                inc(cur_run, coords, predicate);
        }
    else {
        const count_tp sum = accumulate(counts.begin(), counts.end(), (count_tp) 0);
        for (unsigned predicate = 0; predicate < size; ++predicate) {
            if (counts[predicate])
                inc(cur_run, coords, 2 * predicate);
            if (sum - counts[predicate])
                inc(cur_run, coords, 2 * predicate + 1);
        }
    }
}


inline void retain(int si, int p)
{
    site_info[si].retain[p] = true;
}

inline void cull(int si, int p)
{
    pred_stat ps = get_pred_stat(si, p);

    if (!CullPredicates::cull || retain_pred(site_info[si].S[p], site_info[si].F[p], ps.lb)) {
        site_info[si].retain[p] = true;
    }
}

typedef void (*pfct)(int,int);

void foreach_pred(pfct thef)
{
    for (unsigned si = 0; si < staticSiteInfo->siteCount; si++) {
        const site_t site = staticSiteInfo->site(si);
        for (unsigned int p = 0; p < num_preds(site.scheme_code); p++)
            (*thef)(si, p);
    }
}

void cull_preds()
{
    foreach_pred(&cull);
}

inline bool have_equal_increase(int s1, int p1, int s2, int p2)
{
    pred_stat ps1 = get_pred_stat(s1, p1);
    pred_stat ps2 = get_pred_stat(s2, p2);

    return (int) rint(ps1.lb * 100) == (int) rint(ps2.lb * 100);
}

void cull_preds_aggressively1()
{
    if (!CullPredicates::cull) return;

    unsigned si;

    for (si = 0; si < staticSiteInfo->siteCount; si++) {
        const site_t &site = staticSiteInfo->site(si);
        switch (site.scheme_code) {
            case 'S':
            case 'R':
                {
                    site_info_t& s = site_info[si];

                    if (s.retain[LEQ] &&
                        s.retain[LT ] &&
                        s.retain[EQ ] &&
                        have_equal_increase(si, LT, si, LEQ) &&
                        have_equal_increase(si, EQ, si, LEQ))
                    {
                        s.retain[LT] = s.retain[EQ] = false;
                        break;
                    }

                    if (s.retain[GEQ] &&
                        s.retain[GT ] &&
                        s.retain[EQ ] &&
                        have_equal_increase(si, GT, si, GEQ) &&
                        have_equal_increase(si, EQ, si, GEQ))
                    {
                        s.retain[GT] = s.retain[EQ] = false;
                        break;
                    }

                    if (s.retain[NEQ] &&
                        s.retain[LT ] &&
                        s.retain[GT ] &&
                        have_equal_increase(si, LT, si, NEQ) &&
                        have_equal_increase(si, GT, si, NEQ))
                    {
                        s.retain[LT] = s.retain[GT] = false;
                        break;
                    }

                    if (s.retain[LT ] &&
                        s.retain[LEQ] &&
                        s.retain[NEQ] &&
                        have_equal_increase(si, LEQ, si, LT) &&
                        have_equal_increase(si, NEQ, si, LT))
                    {
                        s.retain[LEQ] = s.retain[NEQ] = false;
                        break;
                    }

                    if (s.retain[GT ] &&
                        s.retain[GEQ] &&
                        s.retain[NEQ] &&
                        have_equal_increase(si, GEQ, si, GT) &&
                        have_equal_increase(si, NEQ, si, GT))
                    {
                        s.retain[GEQ] = s.retain[NEQ] = false;
                        break;
                    }

                    if (s.retain[EQ ] &&
                        s.retain[LEQ] &&
                        s.retain[GEQ] &&
                        have_equal_increase(si, LEQ, si, EQ) &&
                        have_equal_increase(si, GEQ, si, EQ))
                    {
                        s.retain[LEQ] = s.retain[GEQ] = false;
                        break;
                    }

                    if (s.retain[LT ] &&
                        s.retain[LEQ] &&
                        have_equal_increase(si, LT, si, LEQ))
                    {
                        s.retain[LEQ] = false;
                        break;
                    }

                    if (s.retain[LT ] &&
                        s.retain[NEQ] &&
                        have_equal_increase(si, LT, si, NEQ))
                    {
                        s.retain[NEQ] = false;
                        break;
                    }

                    if (s.retain[GT ] &&
                        s.retain[GEQ] &&
                        have_equal_increase(si, GT, si, GEQ))
                    {
                        s.retain[GEQ] = false;
                        break;
                    }

                    if (s.retain[GT ] &&
                        s.retain[NEQ] &&
                        have_equal_increase(si, GT, si, NEQ))
                    {
                        s.retain[NEQ] = false;
                        break;
                    }

                    if (s.retain[EQ ] &&
                        s.retain[LEQ] &&
                        have_equal_increase(si, EQ, si, LEQ))
                    {
                        s.retain[LEQ] = false;
                        break;
                    }

                    if (s.retain[EQ ] &&
                        s.retain[GEQ] &&
                        have_equal_increase(si, EQ, si, GEQ))
                    {
                        s.retain[GEQ] = false;
                        break;
                    }
                    break;
                }

            case 'A':
            case 'B':
            case 'F':
            case 'G':
            case 'Y': 
            case 'Z':
            case 'C':
            case 'W':
                break;
            default:
                assert(0);
        }
    }
}

inline bool is_eligible(int si, int p)
{
    if (!site_info[si].retain[p])
        return false;

    const site_t site = staticSiteInfo->site(si);
    return isdigit(site.args[1][0]) || site.args[1][0] != '-';
}

void checkG(unsigned si, int p)
{
    const site_t siteC = staticSiteInfo->site(si);

    for (unsigned d = 0; d < staticSiteInfo->siteCount; d++) {
        const site_t siteD = staticSiteInfo->site(d);
        if (si != d &&
            siteC.scheme_code == siteD.scheme_code &&
            siteC.line == siteD.line &&
            strcmp(siteC.args[0], siteD.args[0]) == 0 &&
            atoi(siteC.args[1]) < atoi(siteD.args[1]) &&
            ((is_eligible(d, GT) && have_equal_increase(si, p, d, GT )) ||
             (is_eligible(d, GEQ) && have_equal_increase(si, p, d, GEQ))))
        {
            site_info[si].retain[p] = false;
            break;
        }
    }
}

void checkL(unsigned si, int p)
{
    const site_t siteC = staticSiteInfo->site(si);

    for (unsigned d = 0; d < staticSiteInfo->siteCount; d++) {
        const site_t siteD = staticSiteInfo->site(d);
        if (si != d &&
            siteC.scheme_code == siteD.scheme_code &&
            siteC.line == siteD.line &&
            strcmp(siteC.args[0], siteD.args[0]) == 0 &&
            atoi(siteC.args[1]) > atoi(siteD.args[1]) &&
            ((is_eligible(d, LT) && have_equal_increase(si, p, d, LT )) ||
             (is_eligible(d, LEQ) && have_equal_increase(si, p, d, LEQ))))
        {
            site_info[si].retain[p] = false;
            break;
        }
    }
}

void cull_preds_aggressively2()
{
    if (!CullPredicates::cull) return;

    unsigned si;

    for (si = 0; si < staticSiteInfo->siteCount; si ++) {
        const site_t &site = staticSiteInfo->site(si);
        if (site.scheme_code == 'S') {
            if (is_eligible(si, GT))
                checkG(si, GT);
            if (is_eligible(si, GEQ))
                checkG(si, GEQ);
            if (is_eligible(si, LT))
                checkL(si, LT);
            if (is_eligible(si, LEQ))
                checkL(si, LEQ);
        }
    }
}

/****************************************************************************
 * Processing command line options
 ***************************************************************************/


void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
        { &Confidence::argp, 0, 0, 0 },
        { &DatabaseFile::argp, 0, 0, 0 },
        { &NumRuns::argp, 0, 0, 0 },
        { &CullPredicates::argp, 0, 0, 0 },
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

    staticSiteInfo.reset(new StaticSiteInfo());
    site_info.resize(staticSiteInfo->siteCount);

    Reader reader(DatabaseFile::DatabaseName);
    Progress::Bounded progress("computing results", NumRuns::count());
    for (cur_run = NumRuns::begin(); cur_run < NumRuns::end(); cur_run++) {
        progress.step();
        reader.read(cur_run);
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
