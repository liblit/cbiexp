#include <argp.h>
#include <cassert>
#include <cstdio>
#include <ext/hash_map>
#include <fstream>
#include <queue>
#include <vector>
#include "CompactReport.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "classify_runs.h"
#include "fopen.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;


typedef vector<unsigned> OutcomeRunIds;
typedef pair<OutcomeRunIds, OutcomeRunIds> RunIds;
typedef OutcomeRunIds (RunIds::* Outcome);

static unsigned cur_run;


////////////////////////////////////////////////////////////////////////
//
//  Run lists assocated with one retained site
//


struct SiteInfo
{
    RunIds runs;
    RunIds predicates[6];

    void notice(Outcome);
    void notice(Outcome, unsigned predicate);
};


inline void
SiteInfo::notice(Outcome outcome)
{
    (runs.*outcome).push_back(cur_run);
}


inline void
SiteInfo::notice(const Outcome outcome, unsigned predicate)
{
    (predicates[predicate].*outcome).push_back(cur_run);
}


typedef hash_map<SiteCoords, SiteInfo> SiteSeen;
static SiteSeen siteSeen;


////////////////////////////////////////////////////////////////////////
//
//  Information to print later for one retained predicate
//


class PredInfo
{
public:
    PredInfo(const RunIds &site, const RunIds &pred);

    const RunIds &site;
    const RunIds &pred;
};


inline
PredInfo::PredInfo(const RunIds &site, const RunIds &pred)
    : site(site),
      pred(pred)
{
}


static ostream &
operator<<(ostream &out, const OutcomeRunIds &runs)
{
    copy(runs.begin(), runs.end(), ostream_iterator<unsigned>(out, " "));
    return out;
}


static ostream &
operator<<(ostream &out, const RunIds &runs)
{
    return out << "F: " << runs.first << '\n'
	       << "S: " << runs.second << '\n';
}


////////////////////////////////////////////////////////////////////////
//
//  Report reader
//


class Reader : public ReportReader
{
public:
    Reader(Outcome);

    void branchesSite(    const SiteCoords &, unsigned, unsigned) const;
    void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned) const;
    void returnsSite(     const SiteCoords &, unsigned, unsigned, unsigned) const;
    void scalarPairsSite( const SiteCoords &, unsigned, unsigned, unsigned) const;

private:
    SiteInfo *notice(     const SiteCoords &) const;
    void tripleSite(      const SiteCoords &, unsigned, unsigned, unsigned) const;

    const Outcome outcome;
};


inline
Reader::Reader(Outcome outcome)
    : outcome(outcome)
{
}


SiteInfo *
Reader::notice(const SiteCoords &coords) const
{
    const SiteSeen::iterator found = siteSeen.find(coords);
    if (found == siteSeen.end())
	return 0;
    else {
	SiteInfo &info = found->second;
	info.notice(outcome);
	return &info;
    }
}


void Reader::tripleSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) const
{
    assert(x || y || z);
    SiteInfo * const info = notice(coords);
    if (info) {
	if (x) info->notice(outcome, 0);
	if (y) info->notice(outcome, 2);
	if (z) info->notice(outcome, 4);
	if (y || z) info->notice(outcome, 1);
	if (x || z) info->notice(outcome, 3);
	if (x || y) info->notice(outcome, 5);
    }
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
    SiteInfo * const info = notice(coords);
    if (info) {
	if (x) info->notice(outcome, 0);
	if (y) info->notice(outcome, 1);
    }
}


void Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z, unsigned w) const
{
    assert(x || y || z || w);
    SiteInfo * const info = notice(coords);
    if (info) {
	if (x) info->notice(outcome, 0);
	if (y) info->notice(outcome, 1);
	if (z) info->notice(outcome, 2);
	if (w) info->notice(outcome, 3);
    }
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
	{ &CompactReport::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
};


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    classify_runs();

    ofstream ofp("obs.txt");
    ofstream tfp("tru.txt");

    typedef queue<PredInfo> InterestingPreds;
    InterestingPreds interesting;

    FILE * const pfp = fopenRead(PredStats::filename);
    pred_info pi;
    while (read_pred_full(pfp, pi)) {
	const SiteInfo &siteInfo = siteSeen[pi];
	const PredInfo predInfo(siteInfo.runs, siteInfo.predicates[pi.predicate]);
	interesting.push(predInfo);
    }

    fclose(pfp);

    Progress::Bounded progress("computing obs and tru", num_runs);
    for (cur_run = 0; cur_run < num_runs; cur_run++) {
	progress.step();
	Outcome outcome = 0;
	if (is_srun[cur_run])
	    outcome = &RunIds::second;
	else if (is_frun[cur_run])
	    outcome = &RunIds::first;
	else
	    continue;

	Reader(outcome).read(cur_run);
    }

    while (!interesting.empty()) {
	const PredInfo &info = interesting.front();
	ofp << info.site;
	tfp << info.pred;
	interesting.pop();
    }

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
