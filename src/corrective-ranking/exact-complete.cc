#include <algorithm>
#include <cassert>
#include <iostream>

#include "RunSet.h"
#include "main.h"

using namespace std;


void
RunSet::dilute(const Predicate &, const RunSet &winner)
{
  assert(size() == winner.size());

  for (size_t runId = 0; runId < size(); ++runId)
    if ((*this)[runId] && winner[runId])
      {
	(*this)[runId] = 0;
	--count;
      }
}


int
main(int argc, char *argv[])
{
  initialize(argc, argv);
  rankMain("corrected-exact-complete");
  return 0;
}
