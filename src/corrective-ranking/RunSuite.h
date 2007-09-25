#ifndef INCLUDE_corrective_ranking_RunSuite_h
#define INCLUDE_corrective_ranking_RunSuite_h

#include <iosfwd>

#include "RunSet.h"

class Predicate;




enum RunOutcome { Succeed, Fail, Neither };
  //A three valued enumeration, for use with RunSuite's
  //get() and set() methods.



////////////////////////////////////////////////////////////////////////
//
//  A pair of run sets: failures and successes
//


struct RunSuite
{
  RunSet failures;
  RunSet successes;

  RunSuite();
  RunSuite(int);
    //Creates RunSets of the specified size, initialized to false.
    //Use this if you plan on manually setting failures and successes
    //using set().
   
 
  RunSuite( RunSuite rs1, RunSuite rs2 );
    //Creates a new RunSuite which holds the intersection
    //of the two parameters.  In other words, its failures and
    //successes will represent the runs where both rs1 and rs2
    //failed or both succeeded.
    //Don't use this for RunSuites representing two different
    //supersets of runs (why would you?)


  void load(std::istream &);

  double count() const;
  double failRate() const;
  double errorPart() const;


  RunOutcome get( int );
    //Returns the RunOutcome of the specified run.
  void set( int, RunOutcome );
    //Sets the specified run to the specified value.  Throws
    //"out_of_range" if out of range. 
    //            failures[int]     successes[int]
    // Succeed        0                  1
    // Fail           1                  0
    // Neither        0                  0

  void dilute(const Predicate &, const RunSuite &);
};


////////////////////////////////////////////////////////////////////////


inline double
RunSuite::count() const
{
  return failures.count + successes.count;
}


#endif // !INCLUDE_corrective_ranking_RunSuite_h
