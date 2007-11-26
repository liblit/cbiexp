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
  
//   printf("Actual: %lf %lf %lf %lf %lf\n", tru.failures.count, tru.successes.count, obs.failures.count, obs.successes.count, score());
  
  initial = effective = score();
  if(isPerfect())
    perfectCount ++;
}

void Disjunction::estimate() {
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
  tru.failures.count = min(fail, fp + fq);
  obs.successes.count = min(succ, sp + sq + min(spbar, sqbar));
  
  // counts to minimize
  tru.successes.count = max(sp, sq);
  obs.failures.count = max(fp, fq) +
                       max(0, fpbar + fqbar + min(fp, fq) - fail);
                       
//   printf("Estimate: %lf %lf %lf %lf %lf\n", tru.failures.count, tru.successes.count, obs.failures.count, obs.successes.count, score());
  
  if(tru.failures.count + tru.successes.count == 0 ||
     obs.failures.count + obs.successes.count == 0)
    initial = effective = 0;
  else
    initial = effective = score();
}
