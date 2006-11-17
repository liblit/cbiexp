#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>

#include "../NumRuns.h"
#include "../Confidence.h"

#include "Disjunction.h"
#include "Predicate.h"
#include "Candidates.h"
#include "RunSuite.h"
#include "allFailures.h"

using namespace std;

Disjunction::Disjunction(Predicate *pred1_t, Predicate *pred2_t) : Conjunction(nextIndex) {
  mych = 'D';
  nextIndex ++;
  if(nextIndex == 0)
    cout << "Warning: Disjunction index overflow\n";
    
  pred1 = pred1_t;
  pred2 = pred2_t;

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
//   cout << score() << " " << isInteresting() << endl;
}

Disjunction::Disjunction(Predicate *pred1_t, Predicate *pred2_t, bool onlyEstimate)
  : Conjunction(nextIndex)
{
  // If we have to compute the entire Disjunction, i.e. if onlyEstimate == False,
  // call the other constructor.
  if(! onlyEstimate) {
    Disjunction(pred1_t, pred2_t);
    return;
  }
  
  cout << "I dont know to estimate score for a disjunction yet :(\n";
}

