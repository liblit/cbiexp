#ifndef INCLUDE_symmetric_Predicate_h
#define INCLUDE_symmetric_Predicate_h

#include <iosfwd>
#include "Counts.h"

class Both;


class Predicate : public Counts
{
public:
  Predicate(Both &, Predicate &);

  void reclassify(unsigned);
  double score() const;
  void print(std::ostream &) const;

  Both &both;
  Predicate &inverse;

private:
  double badness() const;
  double context() const;
  double increase() const;
  double lowerBound() const;
  double recall() const;
  double harmonic() const;
};


////////////////////////////////////////////////////////////////////////


inline double
Predicate::score() const
{
  return harmonic();
}


#endif // !INCLUDE_symmetric_Predicate_h
