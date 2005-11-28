#include "allFailures.h"


RunSet allFailures(Failure);


AllFailuresSnapshot::AllFailuresSnapshot()
  : original(allFailures)
{
}


AllFailuresSnapshot::~AllFailuresSnapshot()
{
  restore();
}


void
AllFailuresSnapshot::restore() const
{
  allFailures = original;
}
