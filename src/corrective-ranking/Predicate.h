#ifndef INCLUDE_corrective_ranking_Predicate_h
#define INCLUDE_corrective_ranking_Predicate_h

#include <iosfwd>

#include "RunSuite.h"


////////////////////////////////////////////////////////////////////////
//
//  A numbered predicate with various run sets
//


struct Predicate
{
  Predicate(unsigned);
  const unsigned index;
  double initial, effective;

  RunSuite tru;
  RunSuite obs;

  void load(std::istream &tru, std::istream &obs);

  double badness() const;
  double context() const;
  double increase() const;
  double lowerBound() const;
  double recall() const;
  double harmonic() const;
  double score() const;

  void dilute(const Predicate &winner);
};


bool operator<(const Predicate &, const Predicate &);

std::ostream &operator<<(std::ostream &, const Predicate &);


////////////////////////////////////////////////////////////////////////


#include <cassert>


inline double
Predicate::badness() const
{
  return tru.failRate();
}


inline double
Predicate::context() const
{
  return obs.failRate();
}


inline double
Predicate::score() const
{
  const double result = harmonic();
  assert(result >= 0);
  return result;
}


inline bool
operator<(const Predicate &a, const Predicate &b)
{
  return a.effective < b.effective;
}


#endif // !INCLUDE_corrective_ranking_Predicate_h
