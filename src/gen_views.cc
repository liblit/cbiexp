#include <iterator>
#include <map>
#include <list>
#include <string>
#include "PredicatePrinter.h"
#include "Score/Fail.h"
#include "Score/HarmonicMeanLog.h"
#include "Score/HarmonicMeanSqrt.h"
#include "Score/Increase.h"
#include "Score/LowerBound.h"
#include "Score/TrueInFails.h"
#include "ViewPrinter.h"
#include "preds_txt.h"
#include "sorts.h"
#include "sites.h"
#include "utils.h"

using namespace std;

typedef list<pred_info> Stats;


template <class Get>
static void
gen_view(const string &scheme, Stats &stats)
{
    // create XML output file and write initial header
    // make sure we will be able to write before we spend time sorting
    ViewPrinter view("view", scheme, Get::code, "none");

    // sort using the given sorter
    stats.sort(Sort::Descending<Get>());

    // print predicates into view in sorted order
    copy(stats.begin(), stats.end(), ostream_iterator<pred_info>(view));
}


static void
gen_views(const string &scheme, Stats &stats)
{
    using namespace Score;

    gen_view<LowerBound>(scheme, stats);
    gen_view<Increase>(scheme, stats);
    gen_view<Fail>(scheme, stats);
    gen_view<TrueInFails>(scheme, stats);
    gen_view<HarmonicMeanLog>(scheme, stats);
    gen_view<HarmonicMeanSqrt>(scheme, stats);
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static const argp_child argpChildren[] = {
    { &preds_txt_argp, 0, 0, 0 },
    { &classify_runs_argp, 0, "Outcome summary files:", 0 },
    { 0, 0, 0, 0 }
};

static const argp argp = {
    0, 0, 0, 0, argpChildren, 0, 0
};


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int
main(int argc, char** argv)
{
    // command line processing and other initialization
    argp_parse(&argp, argc, argv, 0, 0, 0);

    // group predicates by scheme for easier iteraton later
    // map key "all" includes all schemes
    typedef map<string, Stats> StatsMap;
    StatsMap statsMap;

    // load up predicates, grouped by scheme
    FILE * const statsFile = fopen_read(preds_txt_filename);
    pred_info info;
    while (read_pred_full(statsFile, info)) {
	static const string all("all");
	const string &scheme = scheme_name(sites[info.site].scheme_code);
	statsMap[all].push_back(info);
	statsMap[scheme].push_back(info);
    }

    // generate sorted views for each individual scheme
    for (StatsMap::iterator scheme = statsMap.begin(); scheme != statsMap.end(); ++scheme)
	gen_views(scheme->first, scheme->second);

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
