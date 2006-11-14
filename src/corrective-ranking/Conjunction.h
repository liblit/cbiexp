#ifndef INCLUDE_corrective_ranking_Conjunction_h
#define INCLUDE_corrective_ranking_Conjunction_h

#include <iosfwd>

#include "Predicate.h"
#include "Candidates.h"
#include <list>


////////////////////////////////////////////////////////////////////////
//
//  A conjunction of two predicates
//


class Conjunction: public Predicate
{
public:
  Predicate *pred1, *pred2;
   
  Conjunction(Predicate *pred1_t, Predicate *pred2_t);
  
  // If onlyEstimate is true, the actual conjunction is not computed.  Instead,
  // the count fields of obs.{successes|failures} and tru.{successes|failrues}
  // are initialized with conservative estimates.
  Conjunction(Predicate *pred1_t, Predicate *pred2_t, bool onlyEstimate);

  bool isInteresting();
  double score();
  
  std::string bugometerXML();
    //Returns a string representing a bug-o-meter XML tag.

  std::vector<unsigned> getPredicateList() const;
    //Returns a vector containing all the indices for the
    //predicates being conjoined to form this one.  For a
    //primitive predicate (not a conjunction) this vector has
    //one element.  If printing to an XML file, add one to each
    //element.
  
};

std::ostream &operator<<(std::ostream &, const Conjunction &);

// Computes conjunctions of predicates from input list.  The length of
// the resulting candidate list is bound by the second parameter.
std::list<Conjunction> conjoin(Candidates, unsigned);
  
////////////////////////////////////////////////////////////////////////



#endif // !INCLUDE_corrective_ranking_Conjunction_h
