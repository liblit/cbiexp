#include <algorithm>
#include <iostream>

#include "RunSet.h"
#include "main.h"

using namespace std;


ostream &
operator<<(ostream &sink, const RunSet &runs)
{
  for (RunSet::const_iterator run = runs.begin(); run != runs.end(); ++run)
    sink << "\t\t\t" << *run << '\n';
  return sink;
}


void
RunSet::dilute(const RunSet &winner)
{
#if 0
  cerr << "\tdiluting run set\n"
       << "\t\twinner has " << winner.count << " elements:\n"
       << "\t\told loser has " << count << " elements:\n";
#endif

  list<unsigned> difference;
  set_difference(begin(), end(), winner.begin(), winner.end(), back_inserter(difference));
  
  swap(difference);
  count = size();

  // cerr << "\t\tnew loser has " << count << " elements:\n";
}


int
main(int argc, char *argv[])
{
  rankMain(argc, argv, "corrected-exact-complete");
  return 0;
}
