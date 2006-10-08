#include <math.h> 
#include <functional>
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

/*******************************************************************************
* log base 2 not available in math header, so must define
*******************************************************************************/
double
FailureUniverse::log2(double val) const
{
    return log(val)/M_LN2;
}

/******************************************************************************
* Calculate a single mutual information term
******************************************************************************/
double
FailureUniverse::MIterm(double joint, double margX, double margY) const
{
    return joint == 0.0 ? 0.0 : joint * log2(joint / (margX * margY));
}

/*****************************************************************************
* Calculates mutual information between two RunSets
*****************************************************************************/
double
FailureUniverse::MI(const RunSet & X, const RunSet & Y) const
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
FailureUniverse::signedMI(const RunSet & X, const RunSet & Y) const
{
    double cov = covariance(X,Y);
    double mi = MI(X,Y);
    return mi * (cov >= 0 ? 1.0 : -1.0); 
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
