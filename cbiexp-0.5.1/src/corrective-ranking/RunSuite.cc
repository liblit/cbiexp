#include <cassert>
#include <cmath>

#include "RunSuite.h"

using namespace std;


RunSuite::RunSuite()
  : failures(Failure),
    successes(Success)
{
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
