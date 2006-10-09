#include <iostream>
#include <numeric>
#include <vector>
#include "PredStats.h"
#include "SetVector.h"
#include "PredSet.h"

using namespace std;

PredSet::PredSet()
{
    initialize(PredStats::count());
}

PredSet::PredSet(vector<bool> & other)
{
    initialize(other);
}

ostream &
operator <<(ostream &out, const PredSet &runs)
{
    runs.print(out);
    return out;
}

istream &
operator>>(istream &in, PredSet &runs)
{

    runs.load(in);
    return in;
}
