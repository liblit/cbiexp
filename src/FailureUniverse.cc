#include "NumRuns.h"
#include "RunSet.h"
#include "FailureUniverse.h"
#include "classify_runs.h"

FailureUniverse::FailureUniverse()
{
    classify_runs();    
    cardinality = universeSize(); 
    if(cardinality == 0) throw EmptyUniverseException();
}

/******************************************************************************
* Count the number of failing runs in the test set---but not in the train set.
******************************************************************************/
unsigned int
FailureUniverse::universeSize() const
{
    unsigned result = 0;
    for(unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
        if(is_frun[runId]) ++result;
    }
    return result;
}

/******************************************************************************
* Calculates mean of RunSet
******************************************************************************/
double
FailureUniverse::mean(RunSet & X) const
{
    unsigned result = 0;
    for(unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
        if(is_frun[runId] && X[runId]) ++result;
    }
    return ((double)result)/((double)cardinality);
}

/******************************************************************************
* Calculates covariance of two RunSets
******************************************************************************/
double
FailureUniverse::covariance(RunSet & X, RunSet & Y) const
{
    double Xmean = mean(X);
    double Ymean = mean(Y);
    double result = 0.0;
    for(unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
        if(is_frun[runId]) {
            result += (X[runId] - Xmean) * (Y[runId] - Ymean);
        }
    }
    return ((double)result)/((double)cardinality);
}

/******************************************************************************
* Joint probability that both are true. 
******************************************************************************/
double
FailureUniverse::p_Xtrue_Ytrue(RunSet & X, RunSet & Y) const 
{
    int result = 0;
    for(unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
        if(is_frun[runId] && X[runId] && Y[runId]) ++result;
    }
    return (double)result/(double)cardinality;
}


/******************************************************************************
* Joint probability that X is true and Y false. 
******************************************************************************/
double
FailureUniverse::p_Xtrue_Yfalse(RunSet & X, RunSet & Y) const
{
    int result = 0;
    for(unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
        if(is_frun[runId] && X[runId] && !Y[runId]) ++result;
    }
    return (double)result/(double)cardinality;
}

/******************************************************************************
* Joint probability that both are false. We assume that the number of 
* failing runs in the test set is non-negative.
******************************************************************************/
double
FailureUniverse::p_Xfalse_Yfalse(RunSet & X, RunSet & Y) const
{
    int result = 0;
    for(unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
        if(is_frun[runId] && !X[runId] && !Y[runId]) ++result;
    }
    return (double)result/(double)cardinality;
}

/******************************************************************************
* Marginal probability that X is true.
******************************************************************************/
double
FailureUniverse::p_Xtrue(RunSet & X) const
{
    int result = 0;
    for(unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
        if(is_frun[runId] && X[runId]) ++result;
    }
    return (double)result/(double)cardinality;
}

/******************************************************************************
* Marginal probability that X is false.
******************************************************************************/
double
FailureUniverse::p_Xfalse(RunSet & X) const
{
    int result = 0;
    for(unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
        if(is_frun[runId] && !X[runId]) ++result;
    }
    return (double)result/(double)cardinality;
}
