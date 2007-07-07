#include <cassert>
#include <iostream>
#include "Both.h"
#include "Candidates.h"
#include "Predicate.h"
#include "SuccessReader.h"

using namespace std;


SuccessReader::SuccessReader(const Candidates &candidates)
  : candidates(candidates)
{
}


void
SuccessReader::tallyPair(const PredCoords &aCoords, bool a,
			 const PredCoords &,        bool b) const
{
  Candidates::const_iterator found = candidates.find(aCoords);
  if (found == candidates.end())
    return;

  Predicate &aCounts = *found->second;
  Predicate &bCounts = aCounts.inverse;

  if (a) ++aCounts.trueInSuccesses;
  if (b) ++bCounts.trueInSuccesses;
  if (a && b)
    {
      assert(&aCounts.both == &bCounts.both);
      ++aCounts.both.trueInSuccesses;
    }
}
