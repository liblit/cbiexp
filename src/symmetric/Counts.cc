#include <cassert>
#include <cmath>
#include <iostream>
#include "Counts.h"

using namespace std;


bool
Counts::reclassify(unsigned runId)
{
  const bool removed = trueInFailures.test(runId);
  trueInFailures.reset(runId);

  if (removed)
    ++trueInSuccesses;

  return removed;
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
