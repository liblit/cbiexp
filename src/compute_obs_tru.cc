#include <argp.h>
#include <cassert>
#include <cstdio>
#include <ext/hash_map>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include "CompactReport.h"
#include "NumRuns.h"
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


////////////////////////////////////////////////////////////////////////
//
//  a set of run ids represented as a membership bit vector
//


class RunSet : private vector<bool>
{
public:
    RunSet();
    void insert(unsigned);
    bool find(unsigned) const;

    void print(ostream &) const;
};


inline
RunSet::RunSet()
    : vector<bool>(NumRuns::end)
{
}


inline void
RunSet::insert(unsigned runId)
{
    at(runId) = 1;
}


inline bool
RunSet::find(unsigned runId) const
{
    return at(runId);
}


void
RunSet::print(ostream &out) const
{
    for (unsigned runId = 0; runId < size(); ++runId)
	if ((*this)[runId])
	    out << ' ' << runId;
}


inline ostream &
operator <<(ostream &out, const RunSet &runs)
{
    runs.print(out);
    return out;
}


////////////////////////////////////////////////////////////////////////
//
//  a pair of run sets segregated by outcome
//


class OutcomeRunSets;
typedef RunSet (OutcomeRunSets::* Outcome);

class OutcomeRunSets {
public:
    RunSet failure;
    RunSet success;

    void insert(Outcome, unsigned);
};


inline void
OutcomeRunSets::insert(Outcome outcome, unsigned runId)
{
    (this->*outcome).insert(runId);
}


ostream &
operator<<(ostream &out, const OutcomeRunSets &predicate)
{
    return out << "F:" << predicate.failure << '\n'
	       << "S:" << predicate.success << '\n';
}


////////////////////////////////////////////////////////////////////////
//
//  information about one interesting predicate
//


struct PredInfo
{
    OutcomeRunSets obs;
    OutcomeRunSets tru;
};


////////////////////////////////////////////////////////////////////////
//
//  information about all interesting predicates
//


class PredInfos : public hash_map<PredCoords, PredInfo>
{
};


static PredInfos predInfos;


////////////////////////////////////////////////////////////////////////
//
//  report reader
//


class Reader : public ReportReader
{
public:
    Reader(Outcome, unsigned);

    void branchesSite(    const SiteCoords &, unsigned, unsigned);
    void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned);
    void returnsSite(     const SiteCoords &, unsigned, unsigned, unsigned);
    void scalarPairsSite( const SiteCoords &, unsigned, unsigned, unsigned);

private:
    void tripleSite(const SiteCoords &, unsigned, unsigned, unsigned) const;
    void notice(const SiteCoords &coords, unsigned, bool) const;

    const Outcome outcome;
    const unsigned runId;
};


inline
Reader::Reader(Outcome outcome, unsigned runId)
    : outcome(outcome),
      runId(runId)
{
}


void
Reader::notice(const SiteCoords &site, unsigned predicate, bool tru) const
{
    const PredCoords coords(site, predicate);
    const PredInfos::iterator found = predInfos.find(coords);
    if (found != predInfos.end()) {
	PredInfo &info = found->second;
	info.obs.insert(outcome, runId);
	if (tru)
	    info.tru.insert(outcome, runId);
    }
}


void
Reader::tripleSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) const
{
    assert(x || y || z);
    notice(coords, 0, x);
    notice(coords, 1, y || z);
    notice(coords, 2, y);
    notice(coords, 3, x || z);
    notice(coords, 4, z);
    notice(coords, 5, x || y);
}


inline void
Reader::scalarPairsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z)
{
    tripleSite(coords, x, y, z);
}


inline void
Reader::returnsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z)
{
    tripleSite(coords, x, y, z);
}


void
Reader::branchesSite(const SiteCoords &coords, unsigned x, unsigned y)
{
    assert(x || y);
    notice(coords, 0, x);
    notice(coords, 1, y);
}


void
Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z, unsigned w)
{
    assert(x || y || z || w);
    notice(coords, 0, x);
    notice(coords, 1, y);
    notice(coords, 2, z);
    notice(coords, 3, w);
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
	{ &CompactReport::argp, 0, 0, 0 },
	{ &NumRuns::argp, 0, 0, 0 },
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
    set_terminate(__gnu_cxx::__verbose_terminate_handler);
    process_cmdline(argc, argv);

    classify_runs();

    ofstream ofp("obs.txt");
    ofstream tfp("tru.txt");

    // interesting predicates in "preds.txt" order
    typedef queue<const PredInfo *> InterestingPreds;
    InterestingPreds interesting;

    {
	const unsigned numPreds = PredStats::count();
	Progress::Bounded progress("reading interesting predicate list", numPreds);
	FILE * const pfp = fopenRead(PredStats::filename);
	pred_info pi;
	while (read_pred_full(pfp, pi)) {
	    progress.step();
	    const PredInfo &info = predInfos[pi];
	    interesting.push(&info);
	}
	fclose(pfp);
    }

    {
	Progress::Bounded progress("computing obs and tru", NumRuns::count());
	for (unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
	    progress.step();

	    Outcome outcome = 0;
	    if (is_srun[runId])
		outcome = &OutcomeRunSets::success;
	    else if (is_frun[runId])
		outcome = &OutcomeRunSets::failure;
	    else
		continue;

	    Reader(outcome, runId).read(runId);
	}
    }

    {
	Progress::Bounded progress("printing obs and tru", predInfos.size());
	while (!interesting.empty()) {
	    progress.step();
	    const PredInfo * const info = interesting.front();
	    ofp << info->obs;
	    tfp << info->tru;
	    interesting.pop();
	}
    }

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
