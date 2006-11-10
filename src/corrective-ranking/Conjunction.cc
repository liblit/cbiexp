#include <cassert>
#include <cmath>
#include <iostream>

#include <stdio.h>

#include "../NumRuns.h"

#include "Conjunction.h"
#include "Predicate.h"
#include "Candidates.h"
#include "RunSuite.h"
#include "allFailures.h"

using namespace std;

Conjunction::Conjunction(Predicate *pred1_t, Predicate *pred2_t) : Predicate(0) {
    pred1 = pred1_t;
    pred2 = pred2_t;    
    tru = RunSuite(pred1->tru, pred2->tru);
    
    obs = RunSuite(NumRuns::end);
    for(unsigned r = NumRuns::begin; r < NumRuns::end; r ++) {        
        if(pred1->obs.get(r) != Neither && pred1->tru.get(r) == Neither)
            obs.set(r, pred1->obs.get(r));
        else if (pred2->obs.get(r) != Neither  && pred2->tru.get(r) == Neither)
            obs.set(r, pred2->obs.get(r));
        else
            obs.set(r, Neither);
    }
}

bool Conjunction::isInteresting() {
    if(tru.failures.count == 0)
        return false;
    if(increase() < 0 || lowerBound() < 0 || harmonic() < 0)
        return false;
        
    initial = effective = score();
    return initial > pred1->initial && initial > pred2->initial;
}



std::vector<int>
Conjunction::getPredicateList() const
{
  //Returns a vector containing all the indices for the
  //predicates being conjoined to form this one.  For a
  //primitive predicate (not a conjunction) this vector has
  //one element.  If printing to an XML file, add one to each
  //element.

  std::vector<int> vect1 = pred1->getPredicateList();
  std::vector<int> vect2 = pred2->getPredicateList();
  std::vector<int> conjVect;

  //Copy the elements into the conjoined array
  for ( unsigned i = 0; i < vect1.size(); i++ ) 
    conjVect.push_back( vect1.at(i) );
  for ( unsigned i = 0; i < vect2.size(); i++ )
    conjVect.push_back( vect2.at(i) );

  return conjVect;
}



std::ostream &
operator<<(std::ostream &out, const Conjunction &conjunction)
{
    return
        out << *(conjunction.pred1) << " " << (conjunction.pred1)->score() << "\n" << *(conjunction.pred2) << " " << (conjunction.pred1)->score();
}


// Computes conjunctions of predicates from input list.  The length of
// the resulting candidate list is bound by the second parameter.
std::list<Conjunction> conjoin(Candidates candidates, unsigned limit) {  
  std::list<Conjunction> result;
  double minMax = 0;
  
  Candidates::iterator i, j;
  for(i = candidates.begin(); i != candidates.end(); i ++)
    for(j = i; j != candidates.end(); j ++) {
      
      if (i == j)
        continue;
      if(result.size() == limit) {
        if(i->conjUB() < minMax || j->conjUB() < minMax) {
          continue;
        }
      }
        
      Conjunction c(&*i, &*j);
      if(c.isInteresting()) {
        result.push_back(c);
          
        if(result.size() > limit) {
          result.erase(min_element(result.begin(), result.end()));
          minMax = min_element(result.begin(), result.end())->score();
        }
      }
    }
  return result;
}
