#include <stdio.h>

#include "../NumRuns.h"

#include "Conjunction.h"
#include "Predicate.h"
#include "RunSuite.h"
#include "allFailures.h"

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
  if(isPerfect())
    perfectCount ++;
}

void Conjunction::estimate() {
  double fp, sp, ofp, osp, fpbar, spbar;
  double fq, sq, ofq, osq, fqbar, sqbar;
  
  double fail = allFailures.count;
  static double succ = NumRuns::count() - fail;
  
  fp = pred1->tru.failures.count;
  sp = pred1->tru.successes.count;
  ofp = pred1->obs.failures.count;
  osp = pred1->obs.successes.count;
  fpbar = ofp - fp;
  spbar = osp - sp;
  
  fq = pred2->tru.failures.count;
  sq = pred2->tru.successes.count;
  ofq = pred2->obs.failures.count;
  osq = pred2->obs.successes.count;
  fqbar = ofq - fq;
  sqbar = osq - sq;
  
  // counts to maximize
  tru.failures.count = min(fp, fq);
  obs.successes.count = max(succ, spbar + sqbar + min(sp, sq));
  
  // counts to minimize
  tru.successes.count = max(0, sp + sq - succ);
  obs.failures.count = max(fpbar, fqbar) + 
                       max(0, fp + fq + min(fpbar, fqbar) - fail);
                       
  if(tru.failures.count + tru.successes.count == 0 ||
     obs.failures.count + obs.successes.count == 0)
    initial = effective = 0;
  else
    initial = effective = score();
}
