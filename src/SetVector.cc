#include <iostream>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "SetVector.h"

using namespace std;
using namespace boost;

SetVector::~SetVector()
{
}

/******************************************************************************
* Member functions
******************************************************************************/
void
SetVector::resize(size_t numbits, bool val)
{
    theVector.resize(numbits, val);
}

void
SetVector::set(size_t n, bool val)
{
    theVector.set(n,val);
}

size_t
SetVector::count() const
{
    return theVector.count();
}

const imp &
SetVector::value() const
{
    return theVector;
}

imp
SetVector::copy() const
{
    return imp(theVector);
}

vector<unsigned int> 
SetVector::toCounts() const
{
    vector <unsigned int> result;
    for(unsigned int i = 0; i < theVector.size(); i++) {
        result.push_back(theVector[i] ? 1 : 0);   
    }
    return result;
}

bool
SetVector::test(size_t n) const
{
    return theVector.test(n); 
}

void
SetVector::print(ostream &out) const
{
    for (unsigned runId = 0; runId < theVector.size(); ++runId)
	if ((theVector)[runId])
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
        this->set(id);
    }
}

void
SetVector::load(const vector <bool> & other)
{
    assert(theVector.size() == other.size());
    for(unsigned int i = 0; i < theVector.size(); i++) {
        if(other[i]) set(i);
    }
}

void
SetVector::load(const imp & other)
{
    theVector = other;
}

istream &
operator>>(istream &in, SetVector &runs)
{

    runs.load(in);
    return in;
}
