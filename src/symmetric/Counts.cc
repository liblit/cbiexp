#include <cassert>
#include <cmath>
#include <iostream>
#include "Counts.h"

using namespace std;


bool
Counts::reclassifyFailures(const RunSet &explained)
{
  const size_t oldCount = trueInFailures.count();
  trueInFailures -= explained;
  const size_t newCount = trueInFailures.count();
  
  assert(oldCount >= newCount);
  trueInSuccesses += oldCount - newCount;

  return newCount > 0;
}


double
Counts::uncertainty(unsigned failures, unsigned successes)
{
  const double population = failures + successes;
  return failures * successes / pow(population, 3);
}


////////////////////////////////////////////////////////////////////////


void
Counts::print(ostream &out, const char tag[]) const
{
  out << '<' << tag
      << " successes=\"" << trueInSuccesses
      << "\" failures=\"" << trueInFailures.count() << "\"/>";
}
