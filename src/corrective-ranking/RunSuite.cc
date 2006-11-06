#include <cassert>
#include <cmath>

#include "RunSuite.h"

using namespace std;


RunSuite::RunSuite()
  : failures(Failure),
    successes(Success)
{
}


  //Creates a new RunSuite which holds the intersection
  //of the two parameters.  In other words, its failures and
  //successes will represent the runs where both rs1 and rs2
  //failed or both succeeded.
  //Don't use this for RunSuites representing two different sets of
  //runs (why would you?)
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
  //Assumption: rs1.failures.size
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


void
RunSuite::dilute(const Predicate &winPredicate, const RunSuite &winSuite)
{
  failures.dilute(winPredicate, winSuite.failures);
  successes.dilute(winPredicate, winSuite.successes);
}
