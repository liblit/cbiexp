#include <cassert>
#include <cmath>
#include <iostream>

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
    
    initial = effective = score();
}

bool Conjunction::isInteresting() {
    return initial > pred1->initial && initial > pred2->initial;
}

std::ostream &
operator<<(std::ostream &out, const Conjunction &conjunction)
{
    return
        out << *(conjunction.pred1) << "\n" << *(conjunction.pred2);
}
