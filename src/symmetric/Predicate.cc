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


bool
Predicate::reclassifyFailures(const RunSet &explained)
{
  both.reclassifyFailures(explained);
  return Counts::reclassifyFailures(explained);
}


double
Predicate::badness() const
{
  // Conversion to int here is intentional and significant.  Some
  // subexpressions within the return expression have negative values.
  // If calculated using unsigned, those negative values instead
  // become large positive values, and the entire result is mangled.

  const int ft = trueInFailures.count();
  const int fb = both.trueInFailures.count();
  const int st = trueInSuccesses;
  const int sb = both.trueInSuccesses;

  const double result = double(fb - 2 * ft) / (fb + sb - 2 * (ft + st));
  assert(result >= 0);
  assert(result <= 1);
  return result;
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

  const double result = double(ft + fi - fb) / (ft + fi + st + si - fb - sb);
  assert(result >= 0);
  assert(result <= 1);
  return result;
}


double
Predicate::increase() const
{
  const double result = badness() - context();
  assert(result <= 1);

  // !!!: deviation from formal definitions:
  //   clamping increase to be at least zero
  return result < 0 ? 0 : result;
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
  const size_t captured = trueInFailures.count();
  const size_t possible = allFailures.count();
  assert(captured > 0);
  assert(possible > 0);
  assert(captured <= possible);

  const double result = log(double(captured)) / log(double(possible));

  assert(result >= 0);
  assert(result <= 1);
  return result;
}


double
Predicate::harmonic() const
{
  const double result = 2 / (1 / increase() + 1 / recall());
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
