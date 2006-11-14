#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

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
	else if (pred1->obs.get(r) != Neither && pred2->obs.get(r) != Neither)
	    obs.set(r, pred1->obs.get(r));
        else
            obs.set(r, Neither);
    }

    initial = effective = score();
}

Conjunction::Conjunction(Predicate *pred1_t, Predicate *pred2_t, bool onlyEstimate)
  : Predicate(0)
{
  if(! onlyEstimate) {
    Conjunction(pred1_t, pred2_t);
    return;
  }
  
  pred1 = pred1_t;
  pred2 = pred2_t;
  
  Predicate opt1(0), opt2(0);
  double score1, score2 = 0;
  
  // s, f - tru successes, failures.  os, of - obs successes and failures
  // BOUNDS on conjunctions of p1, p2
  // s' = 0, f' = p1.f  i.e., no successes are retained and all failures are retained
  // os' = p1.os + (p2.os - p2.s)  the second term is the runs in which p2 was observed false
  // of' = p1.f
  
  // get upper bound based on pred1
  opt1.tru.successes.count = 0;
  opt1.tru.failures.count = pred1->tru.failures.count;
  opt1.obs.successes.count = pred1->obs.successes.count + pred2->obs.successes.count - pred2->tru.successes.count;
  opt1.obs.failures.count = pred1->tru.failures.count;
  score1 = opt1.score();
  
  // get upper bound based on pred2
  opt2.tru.successes.count = 0;
  opt2.tru.failures.count = pred2->tru.failures.count;
  opt2.obs.successes.count = pred2->obs.successes.count + pred1->obs.successes.count - pred1->tru.successes.count;
  opt2.obs.failures.count = pred2->tru.failures.count;
  score2 = opt2.score();
  
  Predicate &best = (score1 > score2)? opt1 : opt2;
  tru.successes.count = best.tru.successes.count;
  tru.failures.count = best.tru.failures.count;
  obs.successes.count = best.obs.successes.count;
  obs.failures.count = best.obs.failures.count;
  
  initial = effective = score();
}


bool Conjunction::isInteresting() {
    return initial > pred1->initial && initial > pred2->initial;
}

double Conjunction::score() {
  if(tru.failures.count == 0)
    return 0;
  if(increase() < 0 || lowerBound() < 0 || harmonic() < 0)
    return 0;
  return Predicate::score();
}  




std::string
Conjunction::bugometerXML() {
  //Returns a string representing a bug-o-meter XML tag.

  ostringstream oss( ostringstream::out );
  oss << "<bug-o-meter true-success=\"" << tru.successes.count
      << "\" true-failure=\"" << tru.failures.count
      << "\" seen-success=\"" << obs.successes.count
      << "\" seen-failure=\"" << obs.failures.count
      << "\" fail=\"" << badness()
      << "\" context=\"" << context()
      << "\" increase=\"" << increase()
      << "\" lower-bound=\"" << lowerBound()
      << "\" log10-true=\"" << log10(double(tru.count()))
      << "\"/>";
  return oss.str();
}



std::vector<unsigned>
Conjunction::getPredicateList() const
{
  //Returns a vector containing all the indices for the
  //predicates being conjoined to form this one.  For a
  //primitive predicate (not a conjunction) this vector has
  //one element.  If printing to an XML file, add one to each
  //element.

  std::vector<unsigned> vect1 = pred1->getPredicateList();
  std::vector<unsigned> vect2 = pred2->getPredicateList();
  std::vector<unsigned> conjVect;

  //Copy the elements into the conjoined array
  for ( unsigned i = 0; i < vect1.size(); i++ ) 
    conjVect.push_back( vect1.at(i) );
  for ( unsigned i = 0; i < vect2.size(); i++ )
    conjVect.push_back( vect2.at(i) );

  return conjVect;
}



// Computes conjunctions of predicates from input list.  The length of
// the resulting candidate list is bound by the second parameter.
std::list<Conjunction> conjoin(Candidates candidates, unsigned limit) {  
  std::list<Conjunction> result;
  double minMax = 0;
  
  int skipped = 0;
  Candidates::iterator i, j;
  for(i = candidates.begin(); i != candidates.end(); i ++)
    for(j = candidates.begin(); j != i; j ++) {
      
      if(result.size() == limit) {
        Conjunction dummy(&*i, &*j, true);
        if(dummy.score() < minMax) {
          skipped ++;
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
  printf("CONJOIN:: was able to prune %d conjunctions\n", skipped);
  return result;
}

std::ostream &
operator<<(std::ostream &out, const Conjunction &conjunction)
{
    return
        out << *(conjunction.pred1) << " " << (conjunction.pred1)->score() << "\n" << *(conjunction.pred2) << " " << (conjunction.pred1)->score();
}
