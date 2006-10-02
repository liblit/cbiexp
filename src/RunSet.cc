#include <iostream>
#include <vector>
#include "NumRuns.h"
#include "RunSet.h"

using namespace std;

////////////////////////////////////////////////////////////////////////
//
//  a set of run ids represented as a membership bit vector
//

RunSet::RunSet()
    : vector<bool>(NumRuns::end)
{
}


void
RunSet::insert(unsigned runId)
{
    at(runId) = 1;
}


bool
RunSet::find(unsigned runId) const
{
    return at(runId);
}


void
RunSet::print(ostream &out) const
{
    for (unsigned runId = 0; runId < size(); ++runId)
	if ((*this)[runId])
	    out << ' ' << runId;
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
    runs.assign(NumRuns::end, false);
    runs.count = 0;

    unsigned runId;
    in.exceptions(ios::badbit);
    while(in >> runId)
    {
        runs[runId] = 1;
        ++runs.count;
    }

    return in;
}

size_t
RunSet::intersectionSize(const RunSet &other)
{
    assert(size() == other.size());

    size_t result = 0;
    for (size_t runId = 0; runId < size(); ++runId)
        if ((*this)[runId] && other[runId])
            ++result;
    return result;
}
