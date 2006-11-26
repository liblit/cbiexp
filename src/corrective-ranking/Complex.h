#ifndef INCLUDE_corrective_ranking_Complex_h
#define INCLUDE_corrective_ranking_Complex_h

#include <iosfwd>

#include "Predicate.h"
#include "Candidates.h"
#include <list>


////////////////////////////////////////////////////////////////////////
//
//  A conjunction of two predicates
//

class Complex: public Predicate
{
private:
  char type;
  static int nextIndex;

protected:
  Predicate *pred1, *pred2;
  virtual void estimate() {}
  virtual void initialize() {}

public:
  Complex(char, Predicate *, Predicate *);
     
  bool isInteresting();
  double score();
  double lowerBound() const;
  
  std::string bugometerXML();
    //Returns a string representing a bug-o-meter XML tag.

  std::vector<unsigned> getPredicateList() const;
    //Returns a vector containing all the indices for the
    //predicates being conjoined to form this one.  For a
    //primitive predicate (not a conjunction) this vector has
    //one element.  If printing to an XML file, add one to each
    //element.
  
  // Temporary hack to distinguish between a disjunction and a conjunction  
  char getType() {return type;}
  
  friend std::ostream &operator<<(std::ostream &, const Complex &);
};

std::ostream &operator<<(std::ostream &, const Complex &);

// Computes conjunctions of predicates from input list.  The length of
// the resulting candidate list is bound by the second parameter.
std::list<Complex> combine(Candidates &, unsigned, double = 0);
  
////////////////////////////////////////////////////////////////////////

#endif // !INCLUDE_corrective_ranking_Complex_h
