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
  const unsigned index;
  double initial, effective;
  int delta;

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


  virtual std::vector<unsigned> getPredicateList() const {
    //Returns a vector containing all the indices for the
    //predicates being conjoined to form this one.  For a
    //primitive predicate (not a conjunction) this vector has
    //one element.  If printing to an XML file, add one to each
    //element.

    std::vector<unsigned> vect;
    vect.push_back(index);
    return vect;
  }
  
  //Destructor is needed because virtual methods are used.
  virtual ~Predicate() { }
  virtual int getDelta() { return 0; }
};


bool operator<(Predicate &,Predicate &);

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
operator<(Predicate &a, Predicate &b)
{
  if (a.effective == b.effective){
    if(a.getPredicateList().size() == 1 && b.getPredicateList().size() == 1)
      return a.effective < b.effective;
    else if(a.getPredicateList().size() == 1){
      return 1;
    } else if(b.getPredicateList().size() == 1){
      return 0;
    } else {
      return a.getDelta() > b.getDelta();
    }
     
  } else
    return a.effective < b.effective;
  
}


#endif // !INCLUDE_corrective_ranking_Predicate_h
