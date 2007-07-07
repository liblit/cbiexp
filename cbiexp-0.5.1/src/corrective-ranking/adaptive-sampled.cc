#include <fstream>
#include <vector>

#include "../PredStats.h"
#include "../Progress/Bounded.h"

#include "Predicate.h"
#include "main.h"

using namespace std;


////////////////////////////////////////////////////////////////////////
//
//  dilute by peeking at complete data for winner only
//


vector<RunSuite> trueComplete;


void
RunSet::dilute(const Predicate &winPred, const RunSet &winSet, bool)
{
  if (!empty())
    {
      const RunSet RunSuite::* selector;
      switch (winSet.outcome)
	{
	case Success:
	  selector = &RunSuite::successes;
	  break;
	case Failure:
	  selector = &RunSuite::failures;
	  break;
	default:
	  abort();
	}

      const RunSet &completeWinSet = trueComplete.at(winPred.index).*selector;

      const size_t common = intersectionSize(completeWinSet);
      count -= common;
      if (count < 0)
	count = 0;
    }
}


////////////////////////////////////////////////////////////////////////
//
//  the main event
//


int
main(int argc, char *argv[])
{
  initialize(argc, argv);

  {
    ifstream in("../analysis-adapt/tru.txt");
    assert(in);
    const unsigned numPreds = PredStats::count();
    trueComplete.reserve(numPreds);

    Progress::Bounded progress("reading predicates for complete data", numPreds);
    for (unsigned pred = 0; pred < numPreds; ++pred)
      {
	progress.step();
	trueComplete.push_back(RunSuite());
	trueComplete.back().load(in);
      }
  }

  rankMain("adaptive-sampled");
  return 0;
}
