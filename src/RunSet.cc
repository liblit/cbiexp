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
    initialize(NumRuns::end);
}

ostream &
operator <<(ostream &out, const RunSet &runs)
{
    runs.print(out);
    return out;
}

istream &
operator>>(istream &in, RunSet &runs)
{

    runs.load(in);
    return in;
}
