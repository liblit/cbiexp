#include <cassert>

#include "../ClassifyRuns.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../RunsDirectory.h"
#include "../Stylesheet.h"
#include "../ViewPrinter.h"

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


int
main(int argc, char *argv[])
{
  // command line processing and other initialization
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  Boths boths;
  Candidates candidates;
  allFailures.resize(NumRuns::end, false);

  {
    Progress::Bounded progress("reading failure reports", NumRuns::count());
    ifstream runs(ClassifyRuns::failuresFilename);
    unsigned runId;
    while (runs >> runId && runId < NumRuns::end)
      {
	progress.stepTo(runId);
	if (runId >= NumRuns::begin)
	  {
	    allFailures.set(runId);
	    FailureReader(candidates, boths, runId).read(runId);
	  }
      }
  }

  {
    Progress::Bounded progress("reading success reports", NumRuns::count());
    ifstream runs(ClassifyRuns::successesFilename);
    unsigned runId;
    while (runs >> runId && runId < NumRuns::end)
      {
	progress.stepTo(runId);
	if (runId >= NumRuns::begin)
	  SuccessReader(candidates).read(runId);
      }
  }

  // create XML output file and write initial header
  ViewPrinter view(Stylesheet::filename, "symmetric", "symmetric.xml");

  // Progress::Bounded progress("ranking predicates", candidates.size());

  // pluck out predicates one by one, printing as we go
  // stop when we run out of predicates
  while (true)
    {
      cerr << candidates.size() << " candidates left to explain " << allFailures.count() << " failures\n";
      candidates.filter();
      cerr << candidates.size() << " candidates left that could explain at least one failure\n";
      if (candidates.empty()) break;
      assert(allFailures.any());

      {
	static bool dumped = false;
	if (!dumped)
	  {
	    dumped = true;
	    ViewPrinter debug(Stylesheet::filename, "symmetric", "debug.xml");
	    debug << candidates;
	  }
      }

      const Candidates::iterator winner(candidates.best());
      view << *winner << '\n';

      const Predicate &winnerCounts = *winner->second;
      const RunSet &explainedFailures = winnerCounts.trueInFailures;
      cerr << "next selected predictor was true in " << explainedFailures.count() << " failures and has score " << winner->second->score() << '\n';

      for (unsigned explained = explainedFailures.find_first();
	   explained != RunSet::npos;
	   explained = explainedFailures.find_next(explained))
	{
	  for (Candidates::iterator affected = candidates.begin(); affected != candidates.end(); ++affected)
	    affected->second->reclassify(explained);

	  assert(allFailures.test(explained));
	  allFailures.reset(explained);
	  cerr << "  failure " << explained << " has been explained\n";
	}

      candidates.erase(winner);
      // progress.step();
    }

  cerr << "ranking complete with " << allFailures.count() << " unexplained failures\n";

  return 0;
}
