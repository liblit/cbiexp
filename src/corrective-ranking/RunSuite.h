#ifndef INCLUDE_corrective_ranking_RunSuite_h
#define INCLUDE_corrective_ranking_RunSuite_h

#include <iosfwd>

#include "RunSet.h"


////////////////////////////////////////////////////////////////////////
//
//  A pair of run sets: failures and successes
//


struct RunSuite
{
  RunSet failures;
  RunSet successes;

  void load(std::istream &);

  double count() const;
  double failRate() const;
  double errorPart() const;

  void dilute(const RunSuite &winner);
};


////////////////////////////////////////////////////////////////////////


inline double
RunSuite::count() const
{
  return failures.count + successes.count;
}


#endif // !INCLUDE_corrective_ranking_RunSuite_h
