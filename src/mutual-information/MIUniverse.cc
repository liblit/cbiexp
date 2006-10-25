#include <math.h> 
#include <functional>
#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/dynamic_bitset.hpp>
#include <boost/checked_delete.hpp>
#include "../SetVector.h"
#include "MIUniverse.h"

class Count : public binary_function <vector <unsigned int> *, vector <unsigned int> *, vector <unsigned int> *> {
public:
    vector <unsigned int> * operator()(vector<unsigned int> * left, vector<unsigned int> * right) {
        vector <unsigned int> * result = new vector <unsigned int>();

        transform(left->begin(), 
                  left->end(), 
                  right->begin(),  
                  back_inserter(*result),
                  plus<unsigned int>());

        delete left;
        delete right;
        return result;
    }
};

class GetCounts : public unary_function <SetVector *, vector <unsigned int> * > {
public:
    vector <unsigned int> * operator()(SetVector * theSet) {
        return new vector <unsigned int> (theSet->toCounts()); 
    }
};

MIUniverse::~MIUniverse()
{
}

void
MIUniverse::vote(const vector <SetVector *> & sets, const VotingRule & rule, SetVector & result) const
{
    vector <vector <unsigned int> * > sets_counts; 
    transform(sets.begin(), sets.end(), back_inserter(sets_counts), GetCounts());
    vector <unsigned int> * vote_count = accumulate(sets_counts.begin(), 
               sets_counts.end(), 
               new vector<unsigned int>(sets_counts.front()->size(), 0), 
               Count()); 
    for(unsigned int i = 0; i < vote_count->size(); i++) {
        result.set(i, rule((*vote_count)[i]));
    }
    delete vote_count;
}

unsigned int
MIUniverse::intersectionSize(const SetVector & left, const SetVector & right) const
{
    return c_Xtrue_Ytrue(left, right);
}

bool
MIUniverse::nonEmptyIntersection(const SetVector & left, const SetVector & right) const
{
    return (left.value() & right.value()).any(); 
}

void
MIUniverse::computeUnion(const SetVector & left, const SetVector & right,
SetVector & result) const
{
    result.load(left.value() | right.value());
}


class AccUnion : public unary_function <SetVector *, void> {
public:
    AccUnion(const MIUniverse & u, SetVector & s)
    : univ(u), accum(s)
    {
    }
    void operator()(SetVector * other) {
        univ.computeUnion(accum, *other, accum);
    }
private:
    const MIUniverse & univ;
    SetVector & accum; 
};

/***************************************** *************************************
* We don't need to find the set which is the intersection; we just need to know
* whether it is non-empty.
******************************************************************************/
class NonEmptyIntersection : public binary_function <SetVector *, SetVector *, bool> {
public:
    NonEmptyIntersection(const MIUniverse & u)
    : univ(u)
    {
    }

    bool operator()(const SetVector * first, const SetVector * second) const {
        return univ.nonEmptyIntersection(*first, *second);
    }
private:
    const MIUniverse & univ;
};

/******************************************************************************
* Take the head of the list. If any element in the tail of the list has a 
* non-empty intersection with the head remove it from the tail. Return the
* union of the head of the list and all removed elements. 
******************************************************************************/
void
MIUniverse::coalesceStep(list <SetVector *> & theList, SetVector & result) const
{
    result.load(theList.front()->value());
    theList.pop_front();
    vector <SetVector *> temp; 
    while(!theList.empty()) {
        SetVector * item = theList.front();
        theList.pop_front();
        if(nonEmptyIntersection(result, *item)) {
            computeUnion(result, *item, result);
            delete item; 
            copy(temp.begin(), temp.end(), back_inserter(theList));
            temp.clear();
        }
        else {
            temp.push_back(item);
        }
    }
    copy(temp.begin(), temp.end(), back_inserter(theList));
}

void
MIUniverse::coalesce(list <SetVector *> & theList, vector <SetVector *> & result) const
{
    while(!theList.empty()) {
        SetVector * target = new SetVector();
        coalesceStep(theList, *target);
        result.push_back(target);
    }
}

void
MIUniverse::accumulateUnion(list<SetVector*>::iterator begin, list<SetVector*>::iterator end, SetVector & result) const
{
    for_each(begin, end, AccUnion(*this, result));
    
}

/******************************************************************************
* Calculates mean of SetVector
******************************************************************************/
double
MIUniverse::mean(const SetVector & X) const
{
    return p_Xtrue(X);
}

/******************************************************************************
* Calculates covariance of two SetVectors
******************************************************************************/
double
MIUniverse::covariance(const SetVector & X, const SetVector & Y) const
{
    double Xmean = mean(X);
    double Ymean = mean(Y);
    double result = 
      ((double)(c_Xtrue_Ytrue(X,Y)) * (1.0 - Xmean) * (1.0 - Ymean)) +
      ((double)(c_Xtrue_Yfalse(X,Y)) * (1.0 - Xmean) * - Ymean) +
      ((double)(c_Xtrue_Yfalse(Y,X)) * -Xmean * (1.0 - Ymean)) +
      ((double)(c_Xfalse_Yfalse(X,Y)) * -Xmean * -Ymean);
    return ((double)result)/((double)cardinality);
}

/*******************************************************************************
* Calculate entropy of a single run set
*******************************************************************************/
double
MIUniverse::entropyTerm(double prob) const
{
    return prob == 0.0 ? 0.0 : prob * log2(1.0/ prob); 
}

double
MIUniverse::entropy(const SetVector & X) const
{
    return entropyTerm(p_Xtrue(X)) + entropyTerm(p_Xfalse(X));
}

class Entropy : public unary_function <SetVector *, double> {
public:
    Entropy(const MIUniverse & u)
    : univ(u)
    {
    }
    double operator()(const SetVector * X) const {
         return univ.entropy(*X);
    }
private:
    const MIUniverse & univ;
};

void
MIUniverse::entropy(const vector <SetVector *> & theVec, vector <double> & entropies) const
{
    transform(theVec.begin(), theVec.end(), back_inserter(entropies), Entropy(*this));
    
}

/*******************************************************************************
* log base 2 not available in math header, so must define
*******************************************************************************/
double
MIUniverse::log2(double val) const
{
    return log(val)/M_LN2;
}

/******************************************************************************
* Calculate a single mutual information term
******************************************************************************/
double
MIUniverse::MIterm(double joint, double margX, double margY) const
{
    return joint == 0.0 ? 0.0 : joint * log2(joint / (margX * margY));
}

/*****************************************************************************
* Calculates mutual information between two SetVectors 
*****************************************************************************/
double
MIUniverse::MI(const SetVector & X, const SetVector & Y) const
{
    double Xtrue_Ytrue = p_Xtrue_Ytrue(X,Y);
    double Xtrue_Yfalse = p_Xtrue_Yfalse(X,Y);
    double Xfalse_Ytrue = p_Xtrue_Yfalse(Y,X);
    double Xfalse_Yfalse = p_Xfalse_Yfalse(Y,X);
    double Xtrue = p_Xtrue(X);
    double Xfalse = p_Xfalse(X);
    double Ytrue = p_Xtrue(Y);
    double Yfalse = p_Xfalse(Y);

    return MIterm(Xtrue_Ytrue, Xtrue, Ytrue) +
           MIterm(Xtrue_Yfalse, Xtrue, Yfalse) +
           MIterm(Xfalse_Ytrue, Xfalse, Ytrue) +
           MIterm(Xfalse_Yfalse, Xfalse, Yfalse);
}

/******************************************************************************
* Calculate mutual information but with a sign which is the same as the sign of
* the correlation coefficient.
*******************************************************************************/
double
MIUniverse::signedMI(const SetVector & X, const SetVector & Y) const
{
    double cov = covariance(X,Y);
    double mi = MI(X,Y);
    return mi * (cov >= 0 ? 1.0 : -1.0); 
}

class SignedMI : public binary_function <SetVector *, SetVector *, double> {
public:
    SignedMI(const MIUniverse & u)
    : univ(u)
    {
    }
    double operator()(const SetVector * X, const SetVector * Y) const {
        return univ.signedMI(*X, *Y);
    }
private:
    const MIUniverse & univ;
};

void
MIUniverse::signedMI(const SetVector & theSet, const vector <SetVector *> & them, vector <double> & mis) const
{
    transform(them.begin(), them.end(), back_inserter(mis), 
        bind1st(SignedMI(*this), &theSet));
}

/******************************************************************************
* Joint probability that both are true. 
******************************************************************************/
double
MIUniverse::p_Xtrue_Ytrue(const SetVector & X, const SetVector & Y) const 
{
    return ((double)c_Xtrue_Ytrue(X,Y))/((double)cardinality); 
}


/******************************************************************************
* Joint probability that X is true and Y false. 
******************************************************************************/
double
MIUniverse::p_Xtrue_Yfalse(const SetVector & X, const SetVector & Y) const
{
    return ((double)(c_Xtrue_Yfalse(X,Y)))/((double)cardinality);
}

/******************************************************************************
* Joint probability that both are false. We assume that the number of 
* failing runs in the test set is non-negative.
******************************************************************************/
double
MIUniverse::p_Xfalse_Yfalse(const SetVector & X, const SetVector & Y) const
{
    return ((double)(c_Xfalse_Yfalse(X,Y)))/((double)cardinality); 
}

/******************************************************************************
* Marginal probability that X is true.
******************************************************************************/
double
MIUniverse::p_Xtrue(const SetVector & X) const
{
    return ((double)(c_Xtrue(X)))/((double)cardinality);  
}

/******************************************************************************
* Marginal probability that X is false.
******************************************************************************/
double
MIUniverse::p_Xfalse(const SetVector & X) const
{
    return ((double)(c_Xfalse(X)))/ ((double)cardinality);
}

/*****************************************************************************
* Number of times both are true at once
*****************************************************************************/
unsigned int
MIUniverse::c_Xtrue_Ytrue(const SetVector & X, const SetVector & Y) const
{
    return (X.value() & Y.value()).count();
}

/*****************************************************************************
* Number of times X is true and Y isn't 
*****************************************************************************/
unsigned int
MIUniverse::c_Xtrue_Yfalse(const SetVector & X, const SetVector & Y) const
{
    return (X.value() - Y.value()).count();
}

/*****************************************************************************
* Number of times neither is true 
*****************************************************************************/
unsigned int
MIUniverse::c_Xfalse_Yfalse(const SetVector & X, const SetVector & Y) const
{
    return ((X.value() | Y.value()).flip()).count();
}

/*****************************************************************************
* Number of times X is true 
*****************************************************************************/
unsigned int
MIUniverse::c_Xtrue(const SetVector & X) const
{
    return (X.value()).count();
}

/*****************************************************************************
* Number of times X is false 
*****************************************************************************/
unsigned int
MIUniverse::c_Xfalse(const SetVector & X) const
{
    return (X.copy().flip()).count();
}
