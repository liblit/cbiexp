#include <cassert>
#include <cmath>
#include <iostream>
#include "../Confidence.h"
#include "Predicate.h"
#include "Both.h"
#include "allFailures.h"

using namespace std;


Predicate::Predicate(Both &both, const Predicate &inverse)
  : both(both),
    inverse(inverse)
{
  assert((this == &both.first && &inverse == &both.second) ||
	 (this == &both.second && &inverse == &both.first));
}


////////////////////////////////////////////////////////////////////////


double
Predicate::badness() const
{
  const unsigned ft = trueInFailures.size();
  const unsigned fb = both.trueInFailures.size();
  const unsigned st = trueInSuccesses;
  const unsigned sb = both.trueInSuccesses;

  return double(fb - 2 * ft) / (fb + sb - 2 * (ft + st));
}


double
Predicate::context() const
{
  const unsigned ft = trueInFailures.size();
  const unsigned fi = inverse.trueInFailures.size();
  const unsigned fb = both.trueInFailures.size();
  const unsigned st = trueInSuccesses;
  const unsigned si = inverse.trueInSuccesses;
  const unsigned sb = both.trueInSuccesses;

  return double(ft + fi - fb) / (ft + fi + st + si - fb - sb);
}


double
Predicate::increase() const
{
  return badness() - context();
}


double
Predicate::lowerBound() const
{
  const unsigned ft = trueInFailures.size();
  const unsigned fi = inverse.trueInFailures.size();
  const unsigned fb = both.trueInFailures.size();
  const unsigned st = trueInSuccesses;
  const unsigned si = inverse.trueInSuccesses;
  const unsigned sb = both.trueInSuccesses;

  const double badnessUncertainty = uncertainty(ft - fb / 2, st - sb / 2);
  const double contextUncertainty = uncertainty(ft + fi - fb, st + si - sb);
  const double standardError = sqrt(badnessUncertainty + contextUncertainty);
  const double halfWidth = Confidence::critical() * standardError;

  return increase() - halfWidth;
}


double
Predicate::recall() const
{
  // !!!: deviation from formal definitions:
  //   increment denominator to avoid NAN when allFailures.size() == 1
  //   increment numerator to counterbalance incremented denominator

  return log(1. + trueInFailures.size()) / log(1. + allFailures.size());
}


double
Predicate::harmonic() const
{
  return 2 / (1 / lowerBound() + 1 / recall());
}


////////////////////////////////////////////////////////////////////////


void
Predicate::print(ostream &out) const
{
  Counts::print(out, "self");
  inverse.Counts::print(out, "inverse");
  both.Counts::print(out, "both");
}
