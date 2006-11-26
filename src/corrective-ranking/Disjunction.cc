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
    if(pred1->obs.get(r) != Neither)
      obs.set(r, pred1->obs.get(r));
    else
      obs.set(r, pred2->obs.get(r));
      
    if(pred1->tru.get(r) != Neither)
      tru.set(r, pred1->tru.get(r));
    else
      tru.set(r, pred2->tru.get(r));
  }

  initial = effective = score();
}

void Disjunction::estimate() {
  cout << "I dont know to estimate score for a disjunction yet :(\n";
  
  initialize();
}
