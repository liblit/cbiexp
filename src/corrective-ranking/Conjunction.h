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
  
};


////////////////////////////////////////////////////////////////////////



#endif // !INCLUDE_corrective_ranking_Conjunction_h
