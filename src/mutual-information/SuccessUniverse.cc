#include <math.h> 
#include <functional>
#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/dynamic_bitset.hpp>
#include "../NumRuns.h"
#include "../SetVector.h"
#include "SuccessUniverse.h"
#include "../classify_runs.h"


SRunSet::SRunSet(const dynamic_bitset<> & m)
:mask(m)
{
}

void
SRunSet::set(size_t n, bool val)
{
    SetVector::set(n,val);
    theVector &= mask;
}

void
SRunSet::load(istream & in)
{
    SetVector::load(in);
    theVector &= mask;
}

void
SRunSet::load(const imp & theSet)
{
    SetVector::load(theSet);
    theVector &= mask;
}

void
SRunSet::load(const vector <bool> & theVec)
{
    SetVector::load(theVec);
    theVector &= mask;
}

SUniverse::SUniverse()
{
    classify_runs();
    begin = NumRuns::begin;
    end = NumRuns::end;
    mask.resize(end, false);
    for(unsigned int i = begin; i < end; i++) {
        if(is_srun[i]) mask.set(i);
    }
    cardinality = mask.count(); 
    if(cardinality == 0) throw EmptyUniverseException();
}

vector <unsigned int>
SUniverse::getIndices() const
{
    vector <unsigned int> result;
    for(unsigned int i = 0; i < mask.size(); i++) {
        if(mask[i]) result.push_back(i);
    }
    return result;
}

/*****************************************************************************
* Number of times both are true at once
*****************************************************************************/
unsigned int
SUniverse::c_Xtrue_Ytrue(const SetVector & X, const SetVector & Y) const
{
    return (X.value() & Y.value()).count();
}

/*****************************************************************************
* Number of times X is true and Y isn't 
*****************************************************************************/
unsigned int
SUniverse::c_Xtrue_Yfalse(const SetVector & X, const SetVector & Y) const
{
    return ((X.value() - Y.value())).count();
}

/*****************************************************************************
* Number of times neither is true 
*****************************************************************************/
unsigned int
SUniverse::c_Xfalse_Yfalse(const SetVector & X, const SetVector & Y) const
{
    return ((X.value() | Y.value()).flip() & mask).count();
}

/*****************************************************************************
* Number of times X is true 
*****************************************************************************/
unsigned int
SUniverse::c_Xtrue(const SetVector & X) const
{
    return (X.value()).count();
}

/*****************************************************************************
* Number of times X is false 
*****************************************************************************/
unsigned int
SUniverse::c_Xfalse(const SetVector & X) const
{
    return (X.copy().flip() & mask).count();
}

SuccessUniverse::SuccessUniverse()
{
}

const SuccessUniverse &
SuccessUniverse::getUniverse()
{
    static SuccessUniverse u;
    return u;
}

SRunSet
SuccessUniverse::makeSRunSet() const
{
    return SRunSet(univ.mask);
}

void
SuccessUniverse::vote(const vector <SRunSet *> & voters, const VotingRule & rule, SRunSet & result) const
{
    vector <SetVector *> set_voters; 
    copy(voters.begin(), voters.end(), back_inserter(set_voters));
    univ.vote(set_voters, rule, result);
}

double
SuccessUniverse::entropy(const SRunSet & theSet) const
{
    return univ.entropy(theSet);
}

void
SuccessUniverse::entropy(const vector <SRunSet *> & theSets, vector <double> & entropies) const
{
    vector <SetVector*> temp;
    copy(theSets.begin(), theSets.end(), back_inserter(temp));
    univ.entropy(temp, entropies);
}

double
SuccessUniverse::signedMI(const SRunSet & left, const SRunSet & right) const
{
    return univ.signedMI(left, right);
}

void
SuccessUniverse::signedMI(const SRunSet & that, const vector <SRunSet *> & others, vector <double> & mis) const
{
    vector <SetVector *> temp;
    copy(others.begin(), others.end(), back_inserter(temp));
    return univ.signedMI(that, temp, mis);
}

unsigned int
SuccessUniverse::intersectionSize(const SRunSet & left, const SRunSet & right) const
{
    return univ.intersectionSize(left,right);
}

vector <unsigned int>
SuccessUniverse::getIndices() const
{
    return univ.getIndices();
}

unsigned int
SuccessUniverse::cardinality() const
{
    return univ.cardinality;
}

bool
SuccessUniverse::nonEmptyIntersection(const SRunSet & left, const SRunSet & right) const
{
    return univ.nonEmptyIntersection(left, right);
}

class CastToRun : public unary_function <SetVector *, SRunSet *> {
public:
    CastToRun(const SuccessUniverse & u) 
    :univ(u)
    {
    }
    SRunSet * operator()(SetVector * theVec) {
        SRunSet temp = univ.makeSRunSet();
        temp.load(theVec->value());
        return new SRunSet(temp);
    }
private:
    const SuccessUniverse & univ;
};

void
SuccessUniverse::coalesce(const list <SRunSet *> & src, vector <SRunSet *> & result) const
{
    list <SetVector *> temp_src;
    copy(src.begin(), src.end(), back_inserter(temp_src)); 
    vector <SetVector *> temp_result;
    univ.coalesce(temp_src, temp_result); 
    result.clear();
    transform(temp_result.begin(), 
              temp_result.end(), 
              back_inserter(result),
              CastToRun(*this)); 
}

unsigned int
SuccessUniverse::cardinality(const SRunSet & theSet) const
{
    return univ.c_Xtrue(theSet);
}

void
SuccessUniverse::accumulateUnion(list<SRunSet*>::iterator begin, list<SRunSet*>::iterator end, SRunSet & result) const
{
    list <SetVector *> temp; 
    copy(begin, end, back_inserter(temp)); 
    univ.accumulateUnion(temp.begin(), temp.end(), result);
}
