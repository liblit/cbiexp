#include <cassert>
#include <cmath>
#include <iostream>

#include <stdio.h>

#include "../Confidence.h"

#include "Conjunction.h"
#include "Predicate.h"
#include "allFailures.h"

using namespace std;

Conjunction::Conjunction(Predicate *pred1_t, Predicate *pred2_t) : Predicate(0) {
    tru = RunSuite(pred1_t->tru, pred2_t->tru);
    obs = RunSuite(pred1_t->obs, pred2_t->obs);
    
    pred1 = pred1_t;
    pred2 = pred2_t;
    
//     initial = effective = score();
}

bool Conjunction::isInteresting() {
    if(tru.failures.count == 0)
        return false;
    if(harmonic() < 0)
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
