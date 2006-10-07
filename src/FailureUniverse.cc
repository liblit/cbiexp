#include "functional"
#include "NumRuns.h"
#include "RunSet.h"
#include "FailureUniverse.h"
#include "classify_runs.h"

IsMember::IsMember()
{
    classify_runs();
}

bool 
IsMember::operator()(unsigned int runId) const
{
    return is_frun[runId];
}

class IsIn : public unary_function <unsigned int, bool> {
public:
    IsIn() { }
    bool operator()(bool result) const {
        return result; 
    }
};

FailureUniverse::FailureUniverse()
{
    begin = NumRuns::begin;
    end = NumRuns::end;
    cardinality = count(); 
    if(cardinality == 0) throw EmptyUniverseException();
}

template <class Predicate>
unsigned int
FailureUniverse::count(const RunSet & X, const Predicate cond) const
{
    unsigned result = 0;
    for(unsigned int runId = begin; runId < end; ++runId) {
        if((test)(runId) && (cond)(X.find(runId))) result++;
    }
    return result;
}

template <class PredicateX, class PredicateY>
unsigned int
FailureUniverse::count(const RunSet & X, const PredicateX condX, const RunSet & Y, const PredicateY condY) const
{
    unsigned result = 0;
    for(unsigned int runId = begin; runId < end; ++runId) {
        if((test)(runId) && (condX)(X.find(runId)) && (condY)(Y.find(runId))) result++;
    }
    return result;
}


/******************************************************************************
* Count the number of failing runs in the test set---but not in the train set.
******************************************************************************/
unsigned int
FailureUniverse::count() const
{
    unsigned result = 0;
    for(unsigned runId = begin; runId < end; ++runId) {
        if((test)(runId)) ++result;
    }
    return result;
}

/******************************************************************************
* Calculates mean of RunSet
******************************************************************************/
double
FailureUniverse::mean(const RunSet & X) const
{
    return ((double)count(X, IsIn()))/((double)cardinality); 
}

/******************************************************************************
* Calculates covariance of two RunSets
******************************************************************************/
double
FailureUniverse::covariance(const RunSet & X, const RunSet & Y) const
{
    double Xmean = mean(X);
    double Ymean = mean(Y);
    double result = 0.0;
    for(unsigned runId = begin; runId < end; ++runId) {
        if((test)(runId)) {
            result += (X.find(runId) - Xmean) * (Y.find(runId) - Ymean);
        }
    }
    return ((double)result)/((double)cardinality);
}

/*****************************************************************************
* Number of times both are true at once
*****************************************************************************/
unsigned int
FailureUniverse::c_Xtrue_Ytrue(const RunSet & X, const RunSet & Y) const
{
    return count(X, IsIn(), Y, IsIn());
}

/******************************************************************************
* Joint probability that both are true. 
******************************************************************************/
double
FailureUniverse::p_Xtrue_Ytrue(const RunSet & X, const RunSet & Y) const 
{
    return ((double)c_Xtrue_Ytrue(X, Y))/((double)cardinality);
}


/******************************************************************************
* Joint probability that X is true and Y false. 
******************************************************************************/
double
FailureUniverse::p_Xtrue_Yfalse(const RunSet & X, const RunSet & Y) const
{
    return ((double)count(X, IsIn(), Y, not1(IsIn())))/((double)cardinality);
}

/******************************************************************************
* Joint probability that both are false. We assume that the number of 
* failing runs in the test set is non-negative.
******************************************************************************/
double
FailureUniverse::p_Xfalse_Yfalse(const RunSet & X, const RunSet & Y) const
{
    return ((double)count(X, not1(IsIn()), Y, not1(IsIn())))/((double)cardinality);
}

/******************************************************************************
* Marginal probability that X is true.
******************************************************************************/
double
FailureUniverse::p_Xtrue(const RunSet & X) const
{
    return ((double)count(X, IsIn()))/((double)cardinality);
}

/******************************************************************************
* Marginal probability that X is false.
******************************************************************************/
double
FailureUniverse::p_Xfalse(const RunSet & X) const
{
    return ((double)count(X, not1(IsIn())))/((double)cardinality);
}
