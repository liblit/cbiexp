#include <cassert>
#include <iterator>

#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../RunsDirectory.h"
#include "../Stylesheet.h"
#include "../ViewPrinter.h"
#include "../classify_runs.h"

#include "Candidates.h"
#include "Predicate.h"
#include "Boths.h"
#include "Reader.h"
#include "Run.h"
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

  classify_runs();

  Boths boths;
  Candidates candidates;

  {
    Progress::Bounded progress("reading reports", NumRuns::count());

    for (unsigned runId = NumRuns::begin; runId < NumRuns::end; runId++)
      {
	progress.step();

	Run *failure;

	if (__builtin_expect(is_srun[runId], 1))
	  failure = 0;
	else if (__builtin_expect(is_frun[runId], 1))
	  failure = new Run();
	else
	  continue;

	Reader(boths, candidates, failure).read(runId);

	if (failure)
	  if (failure->empty())
	    cerr << "\nwarning: inexplicable failure " << runId << " has no true predicates\n";
	  else
	    {
	      const bool novel = allFailures.insert(failure).second;
	      assert(novel);
	    }
      }
  }

  // create XML output file and write initial header
  ViewPrinter view(Stylesheet::filename, "symmetric", "symmetric.xml");

  // Progress::Bounded progress("ranking predicates", candidates.size());

  // pluck out predicates one by one, printing as we go
  // stop when we run out of predicates
  while (true)
    {
      cerr << candidates.size() << " candidates left to explain " << allFailures.size() << " failures\n";
      candidates.filter();
      cerr << candidates.size() << " candidates left that could explain at least one failure\n";
      if (candidates.empty()) break;
      assert(!allFailures.empty());

      const Candidates::iterator winner(candidates.best());
      view << winner->first;

      const Predicate &winnerCounts = *winner->second;
      const RunSet &explainedFailures = winnerCounts.trueInFailures;
      cerr << "next selected predictor was true in " << explainedFailures.size() << " failures and has score " << winner->second->score() << '\n';

      while (!explainedFailures.empty())
	{
	  const Run &explained = **explainedFailures.begin();

	  for (Run::const_iterator counts = explained.begin(); counts != explained.end(); ++counts)
	    {
	      assert(*counts);
	      Counts &affected = **counts;
	      affected.reclassify(explained);
	    }

	  assert(allFailures.find(&explained) != allFailures.end());
	  allFailures.erase(&explained);
	  delete &explained;
	}

      candidates.erase(winner);
      // progress.step();
    }

  cerr << "ranking complete with " << allFailures.size() << " unexplained failures\n";

  return 0;
}
