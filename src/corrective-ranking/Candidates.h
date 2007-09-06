#ifndef INCLUDE_corrective_ranking_Candidates_h
#define INCLUDE_corrective_ranking_Candidates_h

#include <list>

#include "Predicates.h"


////////////////////////////////////////////////////////////////////////
//
//  A sequence of predicates being considered for ranking
//


struct Candidates : public Predicates
{
  Candidates();
  void load(const char[] = ".");
  unsigned count;
};


#endif // !INCLUDE_corrective_ranking_Candidates_h
