#include <cassert>
#include <iostream>
#include <numeric>
#include "../PredCoords.h"
#include "Reader.h"

using namespace std;


void
Reader::tallyPair(const SiteCoords &site, int id, bool a, bool b) const
{
  assert(a || b);

  const PredCoords aCoords(site, 2 * id);
  const PredCoords bCoords(site, 2 * id + 1);

  tallyPair(aCoords, a, bCoords, b);
}


void
Reader::handleSite(const SiteCoords &site, const vector<unsigned> &counts)
{
  const unsigned sum = accumulate(counts.begin(), counts.end(), 0);
  for (unsigned predicate = 0; predicate < counts.size(); ++predicate)
    tallyPair(site, predicate, counts[predicate], sum - counts[predicate]);
}
