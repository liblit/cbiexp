#ifndef INCLUDE_corrective_ranking_allFailures_h
#define INCLUDE_corrective_ranking_allFailures_h

#include "RunSet.h"


////////////////////////////////////////////////////////////////////////
//
//  The global set of all failed runs
//

extern RunSet allFailures;


////////////////////////////////////////////////////////////////////////
//
//  Snapshot the global allFailures set and restore it later
//

class AllFailuresSnapshot
{
public:
  AllFailuresSnapshot();
  ~AllFailuresSnapshot();
  void restore() const;

private:
  const RunSet original;
};


#endif // !INCLUDE_corrective_ranking_allFailures_h
