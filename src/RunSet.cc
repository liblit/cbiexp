#include <iostream>
#include <numeric>
#include <vector>
#include "NumRuns.h"
#include "SetVector.h"
#include "RunSet.h"

using namespace std;

////////////////////////////////////////////////////////////////////////
//
//  a set of run ids represented as a membership bit vector
//

RunSet::RunSet()
{
    resize(NumRuns::end);
}

void computeOR(const RunSet & left, const RunSet & right, RunSet & result)
{
    left.computeOR(right, result);
}

void computeAND(const RunSet & left, const RunSet & right, RunSet & result)
{
    left.computeAND(right, result);
}

bool nonEmptyIntersection(const RunSet & left, const RunSet & right)
{
    return left.nonEmptyIntersection(right);
}

imp getOR(const RunSet & left, const RunSet & right) 
{
    return left.getOR(right);
}

imp getAND(const RunSet & left, const RunSet & right)
{
    return left.getAND(right);
}

imp getDIFF(const RunSet & left, const RunSet & right)
{
    return left.getDIFF(right);
}
