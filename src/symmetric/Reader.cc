#include <cassert>
#include <iostream>
#include "../PredCoords.h"
#include "Reader.h"

using namespace std;


void
Reader::tallyPair(const SiteCoords &site, int id, unsigned a, unsigned b) const
{
  assert(a || b);

  const PredCoords aCoords(site, 2 * id);
  const PredCoords bCoords(site, 2 * id + 1);

  tallyPair(aCoords, a, bCoords, b);
}


void
Reader::branchesSite(const SiteCoords &site, unsigned a, unsigned b)
{
  assert(a || b);

  tallyPair(site, 0, a, b);
}


void
Reader::tripleSite(const SiteCoords &site, unsigned a, unsigned b, unsigned c) const
{
  assert(a || b || c);

  tallyPair(site, 0, a, b || c);
  tallyPair(site, 1, b, a || c);
  tallyPair(site, 2, c, a || b);
}


void
Reader::gObjectUnrefSite(const SiteCoords &site, unsigned a, unsigned b, unsigned c, unsigned d)
{
  assert(a || b || c || d);

  tallyPair(site, 0, a, b || c || d);
  tallyPair(site, 1, b, a || c || d);
  tallyPair(site, 2, c, a || b || d);
  tallyPair(site, 3, d, a || b || c);
}


void
Reader::returnsSite(const SiteCoords &site, unsigned a, unsigned b, unsigned c)
{
  assert(a || b || c);

  tallyPair(site, 0, a, b || c);
  tallyPair(site, 1, b, a || c);
  tallyPair(site, 2, c, a || b);
}


void
Reader::scalarPairsSite(const SiteCoords &site, unsigned a, unsigned b, unsigned c)
{
  assert(a || b || c);

  tallyPair(site, 0, a, b || c);
  tallyPair(site, 1, b, a || c);
  tallyPair(site, 2, c, a || b);
}
