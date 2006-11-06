#include <argp.h>
#include <iterator>
#include <map>
#include <list>
#include <string>
#include "IndexedPredInfo.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "RunsDirectory.h"
#include "Score/Fail.h"
#include "Score/HarmonicMeanLog.h"
#include "Score/HarmonicMeanSqrt.h"
#include "Score/Increase.h"
#include "Score/LowerBound.h"
#include "Score/LowerTimesFails.h"
#include "Score/TrueInFails.h"
#include "StaticSiteInfo.h"
#include "Stylesheet.h"
#include "ViewPrinter.h"
#include "classify_runs.h"
#include "fopen.h"
#include "sorts.h"
#include "utils.h"

#include "conjoin.h"

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
    gen_view<LowerTimesFails>(scheme, stats);
    gen_view<HarmonicMeanLog>(scheme, stats);
    gen_view<HarmonicMeanSqrt>(scheme, stats);
}


////////////////////////////////////////////////////////////////////////
//
//  command line processing
//


// group predicates by scheme for easier iteraton later
// map key "all" includes all schemes
typedef map<string, Stats> StatsMap;
static StatsMap statsMap;


static int
parseFlag(int key, char *arg, argp_state *)
{
    switch (key) {
    case 'f':
	statsMap[arg];
	return 0;
    default:
	return ARGP_ERR_UNKNOWN;
    }
}


static void
process_cmdline(int argc, char *argv[])
{
    static const argp_option options[] = {
	{
	    "force-scheme",
	    'f',
	    "SCHEME",
	    0,
	    "always generate view for SCHEME sites; may be given multiple times",
	    0
	},
	{ 0, 0, 0, 0, 0, 0 }
    };

    static const argp_child children[] = {
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ &Stylesheet::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	options, parseFlag, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int
main(int argc, char** argv)
{
    // command line processing and other initialization
    process_cmdline(argc, argv);
    classify_runs();

    // load up predicates, grouped by scheme
    StaticSiteInfo staticSiteInfo;
    FILE * const statsFile = fopenRead(PredStats::filename);
    pred_info info;
    unsigned index = 0;
    while (read_pred_full(statsFile, info)) {
	const string &scheme = scheme_name(staticSiteInfo.site(info.siteIndex).scheme_code);
	const IndexedPredInfo indexed(info, index++);
	static const string all("all");
	statsMap[all].push_back(indexed);
	statsMap[scheme].push_back(indexed);
    }

    conjoin();
    // generate sorted views for each individual scheme
    for (StatsMap::iterator scheme = statsMap.begin(); scheme != statsMap.end(); ++scheme)
	gen_views(scheme->first, scheme->second);

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
