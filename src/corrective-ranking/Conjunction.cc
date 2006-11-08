#include <cassert>
#include <cmath>
#include <iostream>

#include <stdio.h>

#include "../NumRuns.h"

#include "Conjunction.h"
#include "Predicate.h"
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

std::ostream &
operator<<(std::ostream &out, const Conjunction &conjunction)
{
    return
        out << *(conjunction.pred1) << " " << (conjunction.pred1)->score() << "\n" << *(conjunction.pred2) << " " << (conjunction.pred1)->score();
}
