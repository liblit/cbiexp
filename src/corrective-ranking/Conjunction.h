#ifndef INCLUDE_corrective_ranking_Conjunction_h
#define INCLUDE_corrective_ranking_Conjunction_h

#include "Complex.h"
#include "Predicate.h"

////////////////////////////////////////////////////////////////////////
//
//  A conjunction of two predicates
//


class Conjunction: public Complex
{
private:
  void estimate();
  void initialize();

public:
  // If onlyEstimate is true, the actual conjunction is not computed.  Instead,
  // the count fields of obs.{successes|failures} and tru.{successes|failures}
  // are initialized with conservative estimates.
  Conjunction(Predicate *pred1_t, Predicate *pred2_t, bool onlyEstimate = false);
};
  
////////////////////////////////////////////////////////////////////////

#endif // !INCLUDE_corrective_ranking_Conjunction_h
