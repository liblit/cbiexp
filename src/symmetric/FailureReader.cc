#include <cassert>
#include <iostream>
#include "Boths.h"
#include "Candidates.h"
#include "FailureReader.h"
#include "Predicate.h"

using namespace std;


FailureReader::FailureReader(Candidates &candidates, Boths &boths, unsigned runId)
  : candidates(candidates),
    boths(boths),
    runId(runId)
{
}


void
FailureReader::tallyPair(const PredCoords &aCoords, unsigned a,
			 const PredCoords &bCoords, unsigned b) const
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

  if (a) aCounts->trueInFailures.set(runId);
  if (b) bCounts->trueInFailures.set(runId);
  if (a && b)
    {
      assert(&aCounts->both == &bCounts->both);
      aCounts->both.trueInFailures.set(runId);
    }
}
