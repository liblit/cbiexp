#include <cassert>
#include <cmath>

#include "RunSuite.h"

using namespace std;


void
RunSuite::load(istream &in)
{
  failures.load(in, 'F');
  successes.load(in, 'S');
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
RunSuite::dilute(const RunSuite &winner)
{
  failures.dilute(winner.failures);
  successes.dilute(winner.successes);
}
