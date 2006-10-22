#include <math.h> 
#include <functional>
#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/dynamic_bitset.hpp>
#include "../NumRuns.h"
#include "../SetVector.h"
#include "FailureUniverse.h"
#include "../classify_runs.h"


FRunSet::FRunSet(const dynamic_bitset<> & m)
:mask(m)
{
}

void
FRunSet::set(size_t n, bool val)
{
    SetVector::set(n,val);
    theVector &= mask;
}

void
FRunSet::load(istream & in)
{
    SetVector::load(in);
    theVector &= mask;
}

void
FRunSet::load(const imp & theSet)
{
    SetVector::load(theSet);
    theVector &= mask;
}

void
FRunSet::load(const vector <bool> & theVec)
{
    SetVector::load(theVec);
    theVector &= mask;
}

FUniverse::FUniverse()
{
    classify_runs();
    begin = NumRuns::begin;
    end = NumRuns::end;
    mask.resize(end, false);
    for(unsigned int i = begin; i < end; i++) {
        if(is_frun[i]) mask.set(i);
    }
    cardinality = mask.count(); 
    if(cardinality == 0) throw EmptyUniverseException();
}

vector <unsigned int>
FUniverse::getIndices() const
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
FUniverse::c_Xtrue_Ytrue(const SetVector & X, const SetVector & Y) const
{
    return (X.value() & Y.value()).count();
}

/*****************************************************************************
* Number of times X is true and Y isn't 
*****************************************************************************/
unsigned int
FUniverse::c_Xtrue_Yfalse(const SetVector & X, const SetVector & Y) const
{
    return ((X.value() - Y.value())).count();
}

/*****************************************************************************
* Number of times neither is true 
*****************************************************************************/
unsigned int
FUniverse::c_Xfalse_Yfalse(const SetVector & X, const SetVector & Y) const
{
    return ((X.value() | Y.value()).flip() & mask).count();
}

/*****************************************************************************
* Number of times X is true 
*****************************************************************************/
unsigned int
FUniverse::c_Xtrue(const SetVector & X) const
{
    return (X.value()).count();
}

/*****************************************************************************
* Number of times X is false 
*****************************************************************************/
unsigned int
FUniverse::c_Xfalse(const SetVector & X) const
{
    return (X.copy().flip() & mask).count();
}

FailureUniverse::FailureUniverse()
{
}

const FailureUniverse &
FailureUniverse::getUniverse()
{
    static FailureUniverse u;
    return u;
}

FRunSet
FailureUniverse::makeFRunSet() const
{
    return FRunSet(univ.mask);
}

void
FailureUniverse::vote(const vector <FRunSet *> & voters, const VotingRule & rule, FRunSet & result) const
{
    vector <SetVector *> set_voters; 
    copy(voters.begin(), voters.end(), back_inserter(set_voters));
    univ.vote(set_voters, rule, result);
}

double
FailureUniverse::entropy(const FRunSet & theSet) const
{
    return univ.entropy(theSet);
}

double
FailureUniverse::signedMI(const FRunSet & left, const FRunSet & right) const
{
    return univ.signedMI(left, right);
}

unsigned int
FailureUniverse::intersectionSize(const FRunSet & left, const FRunSet & right) const
{
    return univ.intersectionSize(left,right);
}

vector <unsigned int>
FailureUniverse::getIndices() const
{
    return univ.getIndices();
}

unsigned int
FailureUniverse::cardinality() const
{
    return univ.cardinality;
}

bool
FailureUniverse::nonEmptyIntersection(const FRunSet & left, const FRunSet & right) const
{
    return univ.nonEmptyIntersection(left, right);
}

class CastToRun : public unary_function <SetVector *, FRunSet *> {
public:
    CastToRun(const FailureUniverse & u) 
    :univ(u)
    {
    }
    FRunSet * operator()(SetVector * theVec) {
        FRunSet temp = univ.makeFRunSet();
        temp.load(theVec->value());
        return new FRunSet(temp);
    }
private:
    const FailureUniverse & univ;
};

void
FailureUniverse::coalesce(const list <FRunSet *> & src, vector <FRunSet *> & result) const
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
FailureUniverse::cardinality(const FRunSet & theSet) const
{
    return univ.c_Xtrue(theSet);
}

void
FailureUniverse::accumulateUnion(list<FRunSet*>::iterator begin, list<FRunSet*>::iterator end, FRunSet & result) const
{
    list <SetVector *> temp; 
    copy(begin, end, back_inserter(temp)); 
    univ.accumulateUnion(temp.begin(), temp.end(), result);
}
