#include <fstream>
#include <vector>

#include "Predicate.h"
#include "main.h"

using namespace std;


////////////////////////////////////////////////////////////////////////
//
//  dilute by peeking at complete data for winner only
//


vector<RunSuite> trueComplete;


void
RunSet::dilute(const Predicate &winPred, const RunSet &winSet)
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
  ifstream in("../complete/tru.txt");
  assert(in);
  do
    {
      trueComplete.push_back(RunSuite());
      trueComplete.back().load(in);
    }
  while (in);
  trueComplete.pop_back();

  rankMain(argc, argv, "adaptive-sampled");
  return 0;
}
