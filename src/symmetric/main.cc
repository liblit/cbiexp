#include <cassert>
#include <cstring>

#include "../ClassifyRuns.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../RunsDirectory.h"
#include "../Stylesheet.h"
#include "../ViewPrinter.h"
#include "../termination.h"

#include "Candidates.h"
#include "Predicate.h"
#include "Boths.h"
#include "SuccessReader.h"
#include "FailureReader.h"
#include "allFailures.h"

using namespace std;

const char *Stylesheet::filename = "symmetric.xsl";



////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static void
processCommandLine(int argc, char *argv[])
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
}


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


static void
open(ifstream &stream, const char filename[])
{
  stream.open(filename);
  if (stream.fail())
    {
      const int code = errno;
      cerr << "cannot read " << filename << ": " << strerror(code) << '\n';
      exit(code || 1);
    }
}


int
main(int argc, char *argv[])
{
  // command line processing and other initialization
  set_terminate_verbose();
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  Boths boths;
  Candidates candidates;

  {
    ifstream runs;
    unsigned runId;
    open(runs, ClassifyRuns::failuresFilename);

    while (runs >> runId && runId < NumRuns::end)
      if (runId >= NumRuns::begin)
	++RunSet::universeSize;

    allFailures.resize(RunSet::universeSize, true);
  }

  {
    Progress::Bounded progress("reading failure reports", NumRuns::count());
    ifstream runs;
    unsigned runId;
    unsigned failureId = 0;
    open(runs, ClassifyRuns::failuresFilename);

    while (runs >> runId && runId < NumRuns::end)
      if (runId >= NumRuns::begin)
	{
	  progress.stepTo(runId - NumRuns::begin);
	  FailureReader(candidates, boths, failureId).read(runId);
	  ++failureId;
	}
  }

  {
    Progress::Bounded progress("reading success reports", NumRuns::count());
    ifstream runs;
    unsigned runId;
    open(runs, ClassifyRuns::successesFilename);

    while (runs >> runId && runId < NumRuns::end)
      if (runId >= NumRuns::begin)
	{
	  progress.stepTo(runId - NumRuns::begin);
	  SuccessReader(candidates).read(runId);
	}
  }

  cerr << '\n' << candidates.size() << " candidates to explain " << allFailures.count() << " failures\n";
  candidates.filter();

  {
    cerr << "dumping initial candidates list into \"debug.xml\": ";
    ViewPrinter debug(Stylesheet::filename, "symmetric", "debug.xml");
    debug << candidates;
    cerr << "done\n";
  }

  // create XML output file and write initial header
  ViewPrinter view(Stylesheet::filename, "symmetric", "symmetric.xml");

  // pluck out predicates one by one, printing as we go
  // stop when we run out of predicates
  while (true)
    {
      cerr << '\n' << candidates.size() << " candidates left that could explain at least one of " << allFailures.count() << " failures\n";
      if (candidates.empty()) break;
      assert(allFailures.any());

      const Candidates::const_iterator winner(candidates.best());
      view << *winner;

      const Predicate &winnerCounts = *winner->second;
      const RunSet explainedFailures = winnerCounts.trueInFailures;
      cerr << "next best candidate was true in " << explainedFailures.count() << " failures and has score " << winner->second->score() << '\n';

      __gnu_cxx::hash_map<PredCoords, Predicate *> newCandidates;
      for (Candidates::iterator candidate = candidates.begin(); candidate != candidates.end(); ++candidate)
	if (candidate->second->reclassifyFailures(explainedFailures))
	  newCandidates.insert(*candidate);
      candidates.swap(newCandidates);

      allFailures -= explainedFailures;
    }

  cerr << "\nranking complete with " << allFailures.count() << " unexplained failures\n";

  return 0;
}
