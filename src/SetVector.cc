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
SetVector::initialize(const vector<bool> & other)
{
    resize(other.size(), false);
    for(unsigned int i = 0; i < size(); i++) {
        at(i) = other[i];
    }
}

void
SetVector::initialize(vector<bool> & other)
{
    swap(other);
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
SetVector::numEntries() const
{
    return size();
}

size_t
SetVector::setSize() const
{
    return count(this->begin(), this->end(), true);
}

bool
nonEmptyIntersection(const SetVector & first, const SetVector & second)
{
    assert(first.size() == second.size()); 
    return inner_product(first.begin(), first.end(), second.begin(), false);
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

SetVector calc_union(const SetVector & first, const SetVector & snd)
{
    assert(first.size() == snd.size());
    SetVector result;
    result.resize(first.size(), false);
    for(unsigned int i = 0; i < result.size(); i++) {
        result.at(i) = first.at(i) || snd.at(i);
    }
    return result;
}

void
calc_union(const SetVector & first, const SetVector & snd, SetVector & result)
{
    assert(first.size() == snd.size());
    assert(first.size() == result.size());
    for(unsigned int i = 0; i < result.size(); i++) {
        result.at(i) = first.at(i) || snd.at(i);
    }
}
