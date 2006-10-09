#include <iostream>
#include <numeric>
#include <vector>
#include "SetVector.h"

using namespace std;

void
SetVector::initialize(unsigned int length)
{
    resize(length, false);
}

void
SetVector::insert(unsigned runId)
{
    at(runId) = true;
}


bool
SetVector::find(unsigned runId) const
{
    return at(runId);
}


void
SetVector::print(ostream &out) const
{
    for (unsigned runId = 0; runId < size(); ++runId)
	if ((*this)[runId])
	    out << ' ' << runId;
}


ostream &
operator <<(ostream &out, const SetVector &runs)
{
    runs.print(out);
    return out;
}

void
SetVector::load(istream & in)
{
    unsigned id;
    in.exceptions(ios::badbit);
    while(in >> id) {
        this->insert(id);
    }
}

istream &
operator>>(istream &in, SetVector &runs)
{

    runs.load(in);
    return in;
}

size_t
SetVector::setSize()
{
    return count(this->begin(), this->end(), true);
}

size_t
SetVector::intersectionSize(const SetVector &other) const
{
    assert(size() == other.size());

    size_t result = 0;
    for (size_t runId = 0; runId < size(); ++runId)
        if ((*this)[runId] && other[runId])
            ++result;
    return result;
}
