#include <cassert>
#include <cmath>
#include <iostream>
#include "Counts.h"
#include "Run.h"

using namespace std;


void
Counts::trueInRun(Run *failure)
{
  if (__builtin_expect(!failure, 1))
    ++trueInSuccesses;
  else
    {
      failure->push_back(this);
      const bool novel = trueInFailures.insert(failure).second;
      assert(novel);
    }
}


void
Counts::reclassify(const Run &failure)
{
  const RunSet::size_type removed = trueInFailures.erase(&failure);
  assert(removed == 1);
  ++trueInSuccesses;
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
      << "\" failures=\"" << trueInFailures.size() << "/>";
}
