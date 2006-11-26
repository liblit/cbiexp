#include <stdio.h>

#include "../NumRuns.h"

#include "Conjunction.h"
#include "Predicate.h"
#include "RunSuite.h"

using namespace std;

Conjunction::Conjunction(Predicate *pred1_t, Predicate *pred2_t, bool onlyEstimate)
  : Complex('C', pred1_t, pred2_t)
{
  if(onlyEstimate)
    estimate();
  else
    initialize();
}

void Conjunction::initialize() {
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

void Conjunction::estimate() {  
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
