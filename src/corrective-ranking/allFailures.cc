#include "allFailures.h"


RunSet allFailures(Failure);


AllFailuresSnapshot::AllFailuresSnapshot()
  : original(allFailures)
{
  assert(allFailures.count > 0);
}


AllFailuresSnapshot::~AllFailuresSnapshot()
{
  original.swap(allFailures);
}
