#include <cassert>
#include "CountingIterator.h"
#include "RunSet.h"
#include "main.h"


void
RunSet::dilute(const RunSet &winner)
{
  if (!empty())
    {
      // how many runs do these two sets have in common?
      size_t intersectionSize = 0;
      CountingIterator<unsigned> result(intersectionSize);
      set_intersection(begin(), end(), winner.begin(), winner.end(), result);

      const double probability = double(intersectionSize) / size();
      count -= size() * probability;
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
