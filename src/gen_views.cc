#include <argp.h>
#include <iterator>
#include <map>
#include <list>
#include <string>
#include "IndexedPredInfo.h"
#include "PredStats.h"
#include "Score/Fail.h"
#include "Score/HarmonicMeanLog.h"
#include "Score/HarmonicMeanSqrt.h"
#include "Score/Increase.h"
#include "Score/LowerBound.h"
#include "Score/TrueInFails.h"
#include "Stylesheet.h"
#include "ViewPrinter.h"
#include "classify_runs.h"
#include "fopen.h"
#include "sites.h"
#include "sorts.h"
#include "utils.h"

using namespace std;

typedef list<IndexedPredInfo> Stats;

const char *Stylesheet::filename = "view.xsl";


template <class Get>
static void
gen_view(const string &scheme, Stats &stats)
{
    // create XML output file and write initial header
    // make sure we will be able to write before we spend time sorting
    ViewPrinter view(Stylesheet::filename, "view", scheme, Get::code, "none");

    // sort using the given sorter
    stats.sort(Sort::Descending<Get>());

    // print predicates into view in sorted order
    copy(stats.begin(), stats.end(), ostream_iterator<IndexedPredInfo>(view));
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
//  The main event
//


int
main(int argc, char** argv)
{
    // command line processing and other initialization
    argp_parse(&Stylesheet::argp, argc, argv, 0, 0, 0);
    classify_runs();

    // group predicates by scheme for easier iteraton later
    // map key "all" includes all schemes
    typedef map<string, Stats> StatsMap;
    StatsMap statsMap;

    // load up predicates, grouped by scheme
    FILE * const statsFile = fopenRead(PredStats::filename);
    pred_info info;
    unsigned index = 0;
    while (read_pred_full(statsFile, info)) {
	static const string all("all");
	const string &scheme = scheme_name(sites[info.siteIndex].scheme_code);
	const IndexedPredInfo indexed(info, index++);
	statsMap[all].push_back(indexed);
	statsMap[scheme].push_back(indexed);
    }

    // generate sorted views for each individual scheme
    for (StatsMap::iterator scheme = statsMap.begin(); scheme != statsMap.end(); ++scheme)
	gen_views(scheme->first, scheme->second);

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
