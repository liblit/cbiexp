#include <iostream>
#include <numeric>
#include <vector>
#include "PredStats.h"
#include "SetVector.h"
#include "PredSet.h"

using namespace std;

PredSet::PredSet()
{
    resize(PredStats::count());
}

void computeOR(const PredSet & left, const PredSet & right, PredSet & result)
{
    left.computeOR(right, result);
}

void computeAND(const PredSet & left, const PredSet & right, PredSet & result)
{
    left.computeAND(right, result);
}

bool nonEmptyIntersection(const PredSet & left, const PredSet & right)
{
    return left.nonEmptyIntersection(right);
}
