#include <cassert>
#include "RunSet.h"
#include "main.h"


void
RunSet::dilute(const Predicate &, const RunSet &winner)
{
  if (!empty())
    {
      const size_t common = intersectionSize(winner);
      count -= common;
      if (count < 0)
	count = 0;
    }
}


int
main(int argc, char *argv[])
{
  rankMain(argc, argv, "corrected-approximate-complete");
  return 0;
}
