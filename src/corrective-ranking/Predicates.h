#ifndef INCLUDE_corrective_ranking_Predicates_h
#define INCLUDE_corrective_ranking_Predicates_h

#include <list>

#include "Predicate.h"

namespace Progress { class Bounded; }


////////////////////////////////////////////////////////////////////////
//
//  A sequence of predicates
//


struct Predicates : public std::list<Predicate>
{
  Predicates();
  unsigned count;

  void rescore(Progress::Bounded &);
};


#endif // !INCLUDE_corrective_ranking_Predicates_h
