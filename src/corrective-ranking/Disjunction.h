#ifndef INCLUDE_corrective_ranking_Disjunction_h
#define INCLUDE_corrective_ranking_Disjunction_h

#include <iosfwd>

#include "Predicate.h"
#include "Candidates.h"
#include "Conjunction.h"
#include <list>


////////////////////////////////////////////////////////////////////////
//
//  A Disjunction of two predicates
//


class Disjunction: public Conjunction
{
public:
  Disjunction(Predicate *pred1_t, Predicate *pred2_t);
  
  // If onlyEstimate is true, the actual Disjunction is not computed.  Instead,
  // the count fields of obs.{successes|failures} and tru.{successes|failrues}
  // are initialized with conservative estimates.
  Disjunction(Predicate *pred1_t, Predicate *pred2_t, bool onlyEstimate);

  // Temporary hack to distinguish between a disjunction and a conjunction
  char what() { return 'D'; }
};
  
////////////////////////////////////////////////////////////////////////

#endif // !INCLUDE_corrective_ranking_Disjunction_h
