#include <cassert>
#include <cmath>
#include <iostream>
#include "../Confidence.h"
#include "Predicate.h"
#include "Both.h"
#include "allFailures.h"

using namespace std;


Predicate::Predicate(Both &both, Predicate &inverse)
  : both(both),
    inverse(inverse)
{
  assert((this == &both.first && &inverse == &both.second) ||
	 (this == &both.second && &inverse == &both.first));
}


////////////////////////////////////////////////////////////////////////


void
Predicate::reclassify(unsigned runId)
{
  if (Counts::reclassify(runId))
    both.reclassify(runId);
}


double
Predicate::badness() const
{
  const unsigned ft = trueInFailures.count();
  const unsigned fb = both.trueInFailures.count();
  const unsigned st = trueInSuccesses;
  const unsigned sb = both.trueInSuccesses;

  return double(fb - 2 * ft) / (fb + sb - 2 * (ft + st));
}


double
Predicate::context() const
{
  const unsigned ft = trueInFailures.count();
  const unsigned fi = inverse.trueInFailures.count();
  const unsigned fb = both.trueInFailures.count();
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
  const unsigned ft = trueInFailures.count();
  const unsigned fi = inverse.trueInFailures.count();
  const unsigned fb = both.trueInFailures.count();
  const unsigned st = trueInSuccesses;
  const unsigned si = inverse.trueInSuccesses;
  const unsigned sb = both.trueInSuccesses;

  const double badnessUncertainty = uncertainty(ft - fb / 2, st - sb / 2);
  const double contextUncertainty = uncertainty(ft + fi - fb, st + si - sb);
  const double standardError = sqrt(badnessUncertainty + contextUncertainty);
  const double halfWidth = Confidence::critical() * standardError;

  const double result = increase() - halfWidth;

  // !!!: deviation from formal definitions:
  //   clamping lower bound to be at least zero
  return result < 0 ? 0 : result;
}


double
Predicate::recall() const
{
  // !!!: deviation from formal definitions:
  //   increment denominator to avoid NAN when allFailures.count() == 1
  //   increment numerator to counterbalance incremented denominator

  const double result = log(1. + trueInFailures.count()) / log(1. + allFailures.count());
  assert(result >= 0);
  assert(result <= 1);
  return result;
}


double
Predicate::harmonic() const
{
  const double result = 2 / (1 / lowerBound() + 1 / recall());
  assert(result >= 0);
  assert(result <= 1);
  return result;
}


////////////////////////////////////////////////////////////////////////


void
Predicate::print(ostream &out) const
{
  Counts::print(out, "self");
  inverse.Counts::print(out, "inverse");
  both.Counts::print(out, "both");

  const unsigned ft = trueInFailures.count();
  const unsigned fi = inverse.trueInFailures.count();
  const unsigned fb = both.trueInFailures.count();
  const unsigned st = trueInSuccesses;
  const unsigned si = inverse.trueInSuccesses;
  const unsigned sb = both.trueInSuccesses;

  out << "<bug-o-meter true-success=\"" << st
      << "\" true-failure=\"" << ft
      << "\" seen-success=\"" << (st + si - sb)
      << "\" seen-failure=\"" << (ft + fi - fb)
      << "\" fail=\"" << badness()
      << "\" context=\"" << context()
      << "\" increase=\"" << increase()
      << "\" lower-bound=\"" << lowerBound()
      << "\" log10-true=\"" << log10(double(st + ft))
      << "\"/>";
}
