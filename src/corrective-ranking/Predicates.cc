#include <cassert>
#include <fstream>

#include "../Progress/Bounded.h"
#include "../Progress/Unbounded.h"

#include "Predicates.h"

using namespace std;


Predicates::Predicates()
  : count(0)
{
  ifstream tru("tru.txt");
  ifstream obs("obs.txt");
  assert(tru);
  assert(obs);

  Progress::Unbounded progress("reading predicates");
  while (tru && obs)
    {
      // speculate that there is at least one more predicate so we can
      // create space for it first and then build it in place
      push_back(count++);
      back().load(tru, obs);
      if (tru && obs) progress.step();
    }

  // last speculation was wrong
  pop_back();
  --count;

  assert(tru.eof());
  assert(obs.eof());
}


void
Predicates::rescore(Progress::Bounded &progress)
{
  iterator loser = begin();
  while (loser != end())
    if (loser->lowerBound() > 0)
      {
	loser->effective = loser->score();
	++loser;
      }
    else
      {
	progress.step();
	loser = erase(loser);
      }
}
