#include <cassert>
#include <iostream>
#include "Boths.h"
#include "Candidates.h"
#include "FailureReader.h"
#include "Predicate.h"

using namespace std;


FailureReader::FailureReader(const char* filename, Candidates &candidates, Boths &boths)
  : Reader(filename), 
    candidates(candidates),
    boths(boths)
{
}


void
FailureReader::setid(unsigned id)
{
  assert (id < RunSet::universeSize);
  failureId = id;
}

void
FailureReader::tallyPair(const PredCoords &aCoords, bool a,
			 const PredCoords &bCoords, bool b) const
{
  Predicate *&aCounts = candidates[aCoords];
  Predicate *&bCounts = candidates[bCoords];

  assert(!aCounts == !bCounts);
  if (!aCounts)
    {
      Both * const both = new Both();
      boths.push_back(both);
      aCounts = &both->first;
      bCounts = &both->second;
    }

  if (a) aCounts->trueInFailures.set(failureId);
  if (b) bCounts->trueInFailures.set(failureId);
  if (a && b)
    {
      assert(&aCounts->both == &bCounts->both);
      aCounts->both.trueInFailures.set(failureId);
    }
}
