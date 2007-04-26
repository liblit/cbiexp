#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>

#include "Predicate.h"
#include "RunSet.h"
#include "main.h"

using namespace std;


static ofstream clusters;


void
RunSet::dilute(const Predicate &predicate, const RunSet &winner, bool clustering)
{
  assert(size() == winner.size());

  for (size_t runId = 0; runId < size(); ++runId)
    if ((*this)[runId] && winner[runId])
      {
	(*this)[runId] = false;
	--count;
	if (clustering)
	  clusters << runId << '\t' << predicate.index << '\n';
      }
}


int
main(int argc, char *argv[])
{
  initialize(argc, argv);
  clusters.open("elimination-clusters.txt");
  rankMain("corrected-exact-complete");
  return 0;
}
