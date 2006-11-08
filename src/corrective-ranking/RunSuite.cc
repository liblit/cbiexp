#include <cassert>
#include <cmath>

#include "RunSuite.h"

using namespace std;


RunSuite::RunSuite()
  : failures(Failure),
    successes(Success)
{
}



  //Creates RunSets of the specified size,
  //initialized to false.
RunSuite::RunSuite(int size)
  : failures( Failure, size ),
    successes( Success, size )
{  
}


  //Creates a new RunSuite which holds the intersection
  //of the two parameters.  In other words, its failures and
  //successes will represent the runs where both rs1 and rs2
  //failed or both succeeded.
  //Don't use this for RunSuites representing two different
  //supersets of runs (why would you?)
RunSuite::RunSuite( RunSuite rs1, RunSuite rs2 )
  : failures(Failure),
    successes(Success)
 {

   //Can't meaningfully intersect them if they don't cover the same
   //area...  and all vectors must have the same length.
  assert( rs1.failures.size() == rs1.successes.size() );
  assert( rs1.failures.size() == rs2.failures.size() );
  assert( rs1.successes.size() == rs2.successes.size() );

  //Perform the intersection.
  for( unsigned i = 0; i < rs1.failures.size(); i++ ) {
    if ( rs1.failures.at(i) && rs2.failures.at(i) ) {
      failures.push_back(1);
      failures.count++;
    }
    else
      failures.push_back(0);

    if ( rs1.successes.at(i) && rs2.successes.at(i) ) {
      successes.push_back(1);
      successes.count++;
    }
    else
      successes.push_back(0);
  }
}


void
RunSuite::load(istream &in)
{
  failures.load(in);
  successes.load(in);
}


double
RunSuite::failRate() const
{
  const double population = count();
  const double result =
    population
    ? failures.count / population
    : 0;
  assert(result >= 0);
  assert(result <= 1);
  return result;
}


double
RunSuite::errorPart() const
{
  const double population = count();
  return population
    ? failures.count * successes.count / pow(population, 3)
    : 0;
}



RunValue
RunSuite::get( int index )
{
  //Returns the RunValue of the specified run.

  if ( successes.at( index ) )
    return Succeed;
  else if ( failures.at( index ) )
    return Fail;
  else
    return Neither;
}



void
RunSuite::set( int index, RunValue value )
{
  //Sets the specified run to the specified value.
  //Throws "out_of_range" if out of range.
  //            failures[int]     successes[int]
  // Succeed        0                  1
  // Fail           1                  0
  // Neither        0                  0

  // Set both, correct one (maybe)
  if(value == Succeed) {
    successes.set(index, true);
    failures.set(index, false);
  }
  else {
    successes.set(index, false);
    failures.set(index, true);
  }
}



void
RunSuite::dilute(const Predicate &winPredicate, const RunSuite &winSuite)
{
  failures.dilute(winPredicate, winSuite.failures);
  successes.dilute(winPredicate, winSuite.successes);
}
