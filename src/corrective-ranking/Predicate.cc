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
  double quantile = 0;
  switch (Confidence::level)
    {
    case 90:
      quantile = 1.645;
      break;
    case 95:
      quantile = 1.96;
      break;
    case 96:
      quantile = 2.05;
      break;
    case 98:
      quantile = 2.33;
      break;
    case 99:
      quantile = 2.58;
      break;
    default:
      cerr << "confidence table is incomplete for level " << Confidence::level << "%\n";
      exit(1);
    }

  const double standardError = sqrt(tru.errorPart() + obs.errorPart());
  return increase() - quantile * standardError;
}


double
Predicate::recall() const
{
  const double trueFailuresCount = tru.failures.count;
  const double allFailuresCount = allFailures.count;
  assert(trueFailuresCount > 0);
  assert(allFailuresCount > 0);

  const double result = log(trueFailuresCount) / log(allFailuresCount);
  return (result < 0
	  ? 0
	  : (result > 1
	     ? 1
	     : result));
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
