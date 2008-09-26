#include <argp.h>
#include <cassert>
#include <cstdio>
#include <ext/hash_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <numeric>
#include <queue>
#include <vector>
#include "NumRuns.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "RunSet.h"
#include "OutcomeRunSets.h"
#include "SiteCoords.h"
#include "classify_runs.h"
#include "fopen.h"
#include "termination.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;

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
    Reader(const char* filename) : ReportReader(filename) {}
    void setstate(unsigned);

protected:
    void handleSite(const SiteCoords &, vector<count_tp> &);

private:
    void notice(const SiteCoords &coords, unsigned, bool) const;

    Outcome outcome;
    unsigned runId;
};


inline void
Reader::setstate(unsigned run)
{
  runId = run;
  if (is_frun[run]) outcome = &OutcomeRunSets::failure;
  else
      if (is_srun[run]) outcome = &OutcomeRunSets::success;
      else {
          ostringstream message;
          message << "Ill-formed run " << run;
          throw runtime_error(message.str());
      }
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
Reader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
{
    const count_tp sum = accumulate(counts.begin(), counts.end(), (count_tp) 0);
    assert(sum > 0);

    const size_t size = counts.size();
    if (size == 2)
	for (unsigned predicate = 0; predicate < size; ++predicate)
	    notice(coords, predicate, counts[predicate]);
    else
	for (unsigned predicate = 0; predicate < size; ++predicate) {
	    notice(coords, 2 * predicate,           counts[predicate]);
	    notice(coords, 2 * predicate + 1, sum - counts[predicate]);
	}
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
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
    set_terminate_verbose();
    process_cmdline(argc, argv);

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
        Reader reader("counts.txt");
	Progress::Bounded progress("computing obs and tru", NumRuns::count());
	for (unsigned runId = NumRuns::begin(); runId < NumRuns::end(); ++runId) {
	    progress.step();
            reader.setstate(runId);
	    reader.read(runId);
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
