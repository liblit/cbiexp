#ifndef INCLUDE_corrective_ranking_Conjunction_h
#define INCLUDE_corrective_ranking_Conjunction_h

#include <iosfwd>

#include "Predicate.h"


////////////////////////////////////////////////////////////////////////
//
//  A conjunction of two predicates
//


class Conjunction: public Predicate
{
public:
  Predicate *pred1, *pred2;
  Conjunction(Predicate *pred1_t, Predicate *pred2_t);
  bool isInteresting(); 

  std::vector<int> getPredicateList() const;
    //Returns a vector containing all the indices for the
    //predicates being conjoined to form this one.  For a
    //primitive predicate (not a conjunction) this vector has
    //one element.  If printing to an XML file, add one to each
    //element.
  
};

std::ostream &operator<<(std::ostream &, const Conjunction &);

////////////////////////////////////////////////////////////////////////



#endif // !INCLUDE_corrective_ranking_Conjunction_h
