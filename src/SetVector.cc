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
    theVector.resize(numbits, val);
}

void
SetVector::set(size_type n, bool val)
{
    theVector.set(n,val);
}

size_type
SetVector::size() const
{
    return theVector.size();
}

size_type
SetVector::count() const
{
    return theVector.count();
}

imp
SetVector::value() const
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
SetVector::test(size_type n) const
{
    return theVector.test(n); 
}

void
SetVector::print(ostream &out) const
{
    for (unsigned runId = 0; runId < size(); ++runId)
	if ((theVector)[runId])
	    out << ' ' << runId;
}

void
SetVector::computeOR(const SetVector & other, SetVector & result) const
{
    result.theVector = getOR(other);
}

void
SetVector::computeAND(const SetVector & other, SetVector & result) const
{
    result.theVector= getAND(other); 
}

bool
SetVector::nonEmptyIntersection(const SetVector & other) const
{
    return getAND(other).any(); 
}

imp
SetVector::getOR(const SetVector & other) const
{
    return theVector | other.theVector;
}

imp
SetVector::getAND(const SetVector & other) const
{
    return theVector & other.theVector;
}

imp
SetVector::getDIFF(const SetVector & other) const
{
    return theVector - other.theVector;    
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
