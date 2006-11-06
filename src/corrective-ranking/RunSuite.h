#ifndef INCLUDE_corrective_ranking_RunSuite_h
#define INCLUDE_corrective_ranking_RunSuite_h

#include <iosfwd>

#include "RunSet.h"

class Predicate;


////////////////////////////////////////////////////////////////////////
//
//  A pair of run sets: failures and successes
//


struct RunSuite
{
  RunSet failures;
  RunSet successes;

  RunSuite();
  
  //Creates a new RunSuite which holds the intersection
  //of the two parameters.  In other words, its failures and
  //successes will represent the runs where both rs1 and rs2
  //failed or both succeeded.
  //Don't use this for RunSuites with two different outcomes
  //or two different sizes (why would you?)
  RunSuite( RunSuite rs1, RunSuite rs2 );

  void load(std::istream &);

  double count() const;
  double failRate() const;
  double errorPart() const;

  void dilute(const Predicate &, const RunSuite &);
};


////////////////////////////////////////////////////////////////////////


inline double
RunSuite::count() const
{
  return failures.count + successes.count;
}


#endif // !INCLUDE_corrective_ranking_RunSuite_h
