#include <algorithm>
#include <iostream>

#include "RunSet.h"
#include "main.h"

using namespace std;


void
RunSet::dilute(const RunSet &winner)
{
  list<unsigned> difference;
  set_difference(begin(), end(), winner.begin(), winner.end(), back_inserter(difference));
  
  swap(difference);
  count = size();
}


int
main(int argc, char *argv[])
{
  rankMain(argc, argv, "corrected-exact-complete");
  return 0;
}
