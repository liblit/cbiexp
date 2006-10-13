#include <iostream>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "SetVector.h"

using namespace std;
using namespace boost;

/******************************************************************************
* Member functions
******************************************************************************/
void
SetVector::resize(size_type numbits, bool val)
{
    imp::resize(numbits, val);
}

void
SetVector::set(size_type n, bool val)
{
    imp::set(n,val);
}

size_type
SetVector::size() const
{
    return imp::size();
}

bool
SetVector::test(size_type n) const
{
    return imp::test(n); 
}

void
SetVector::print(ostream &out) const
{
    for (unsigned runId = 0; runId < size(); ++runId)
	if ((*this)[runId])
	    out << ' ' << runId;
}

/******************************************************************************
* Non-member friend procedure
******************************************************************************/
void
computeOR(const SetVector & left, const SetVector & right, SetVector & result)
{
    ((left) | (right)).swap(result);
}

void computeAnd(const SetVector & left, const SetVector & right, SetVector & result)
{
    ((left) & (right)).swap(result); 
}

bool
nonEmptyIntersection(const SetVector & left, const SetVector & right)
{
    return ((left) & (right)).any();
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
        this->set(id);
    }
}

void
SetVector::load(vector <bool> & other)
{
    assert(size() == other.size());
    for(unsigned int i = 0; i < other.size(); i++) {
        if(other[i]) set(i);
    }
}

istream &
operator>>(istream &in, SetVector &runs)
{

    runs.load(in);
    return in;
}
