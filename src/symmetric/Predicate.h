#ifndef INCLUDE_symmetric_Predicate_h
#define INCLUDE_symmetric_Predicate_h

#include "Counts.h"

class Both;


class Predicate : public Counts
{
public:
  Predicate(Both &, const Predicate &);

  double score();

  Both &both;

private:
  double badness() const;
  double context() const;
  double increase() const;
  double lowerBound() const;
  double recall() const;
  double harmonic() const;

  const Predicate &inverse;
};


////////////////////////////////////////////////////////////////////////


inline double
Predicate::score()
{
  return harmonic();
}


#endif // !INCLUDE_symmetric_Predicate_h
