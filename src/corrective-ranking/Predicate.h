#ifndef INCLUDE_corrective_ranking_Predicate_h
#define INCLUDE_corrective_ranking_Predicate_h

#include <iosfwd>

#include "RunSuite.h"


////////////////////////////////////////////////////////////////////////
//
//  A numbered predicate with various run sets
//


// Made into a class (rather than a struct) so that Conjunction
// can subclass off of it.
// --rosin
class Predicate
{
public:
  Predicate(unsigned);
  /*const*/ unsigned index;
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


  virtual std::vector<int> getPredicateList() const {
    //Returns a vector containing all the indices for the
    //predicates being conjoined to form this one.  For a
    //primitive predicate (not a conjunction) this vector has
    //one element.  If printing to an XML file, add one to each
    //element.

    std::vector<int> vect;
    vect.push_back(index);
    return vect;
  }
  
  //Destructor is needed because virtual methods are used.
  virtual ~Predicate() { }
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
