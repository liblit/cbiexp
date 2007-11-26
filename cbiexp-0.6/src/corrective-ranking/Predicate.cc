#include <cassert>
#include <cmath>
#include <iostream>

#include "../Confidence.h"

#include "Predicate.h"
#include "allFailures.h"

using namespace std;


Predicate::Predicate(unsigned index)
  : index(index)
{
}


void
Predicate::load(istream &truFile, istream &obsFile)
{
  tru.load(truFile);
  obs.load(obsFile);

  if (truFile && obsFile)
    initial = effective = score();
}


double
Predicate::increase() const
{
  return badness() - context();
}


double
Predicate::lowerBound() const
{
  const double standardError = sqrt(tru.errorPart() + obs.errorPart());
  return increase() - Confidence::critical() * standardError;
}


double
Predicate::recall() const
{
  const double trueFailuresCount = tru.failures.count;
  const double allFailuresCount = allFailures.count;
  assert(trueFailuresCount > 0);
  assert(allFailuresCount > 0);

  if (trueFailuresCount == 1 && allFailuresCount == 1)
    return 1;
  else
    {
      const double result = log(trueFailuresCount) / log(allFailuresCount);
      return (result < 0
	      ? 0
	      : (result > 1
		 ? 1
		 : result));
    }
}


double
Predicate::harmonic() const
{
  return 2 / (1 / lowerBound() + 1 / recall());
}

void
Predicate::dilute(const Predicate &winner)
{
  tru.dilute(winner, winner.tru);
  obs.dilute(winner, winner.tru);
}

std::ostream &
operator<<(std::ostream &out, const Predicate &predicate)
{
  return
    out << "<predictor index=\"" << predicate.index + 1
	<< "\" initial=\"" << predicate.initial
	<< "\" effective=\"" << predicate.effective
	<< "\">"

	<< "<bug-o-meter true-success=\"" << predicate.tru.successes.count
	<< "\" true-failure=\"" << predicate.tru.failures.count
	<< "\" seen-success=\"" << predicate.obs.successes.count
	<< "\" seen-failure=\"" << predicate.obs.failures.count
	<< "\" fail=\"" << predicate.badness()
	<< "\" context=\"" << predicate.context()
	<< "\" increase=\"" << predicate.increase()
	<< "\" lower-bound=\"" << predicate.lowerBound()
	<< "\" log10-true=\"" << log10(double(predicate.tru.count()))
	<< "\"/>"

	<< "</predictor>";
}

