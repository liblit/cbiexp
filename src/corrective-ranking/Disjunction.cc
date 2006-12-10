#include <iostream>

#include "../NumRuns.h"

#include "Disjunction.h"
#include "Predicate.h"
#include "RunSuite.h"

using namespace std;

Disjunction::Disjunction(Predicate *pred1_t, Predicate *pred2_t, bool onlyEstimate)
  : Complex('D', pred1_t, pred2_t)
{
  if(onlyEstimate)
    estimate();
  else
    initialize();
}

void Disjunction::initialize() {
  tru = RunSuite(NumRuns::end);
  obs = RunSuite(NumRuns::end);
  for(unsigned r = NumRuns::begin; r < NumRuns::end; r ++) {
    RunOutcome p1obs, p1tru, p2obs, p2tru;
    
    p1obs = pred1->obs.get(r);
    p1tru = pred1->tru.get(r);
    
    p2obs = pred2->obs.get(r);
    p2tru = pred2->tru.get(r);
    
    // If p1 was observed & true, p1 v p2 was observed & true
    if(p1obs != Neither && p1tru != Neither) {
      assert(p1obs == p1tru);
      obs.set(r, p1obs);
      tru.set(r, p1tru);
    }    
    // Same for p2
    else if(p2obs != Neither && p2tru != Neither) {
      assert(p2obs == p2tru);
      obs.set(r, p2obs);
      tru.set(r, p2tru);
    }    
    // If both p1 and p2 were observed false, p1 v p2 was observed false
    else if(p1obs != Neither && p1tru == Neither &&
       p2obs != Neither && p2tru == Neither) {
      assert(p1obs == p2obs);
      obs.set(r, p1obs);
      tru.set(r, Neither);
    }    
    // If either one of them was not observed, p1 v p2 was not observed
    else if(p1obs == Neither || p2obs == Neither) {
      obs.set(r, Neither);
      tru.set(r, Neither);
    }    
    // no other case may arise
    else
      assert(false);
  }

  initial = effective = score();
}

void Disjunction::estimate() {
//   cout << "I dont know to estimate score for a disjunction yet :(\n";
  tru.failures.count = pred1->tru.failures.count + pred2->tru.failures.count;
  tru.successes.count = (pred1->tru.successes.count > pred2->tru.successes.count)
                                                ? pred1->tru.successes.count
                                                : pred2->tru.successes.count;
  obs.failures.count = 0;
  obs.successes.count = 1;
  initial = effective = score();
//   initialize();
}
