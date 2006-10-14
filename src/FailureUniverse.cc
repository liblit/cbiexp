#include <math.h> 
#include <functional>
#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/dynamic_bitset.hpp>
#include "NumRuns.h"
#include "RunSet.h"
#include "FailureUniverse.h"
#include "classify_runs.h"

FailureUniverse::FailureUniverse()
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

class GetCounts : public unary_function <RunSet *, vector <unsigned int> * > {
public:
    vector <unsigned int> * operator()(RunSet * theSet) {
        return new vector <unsigned int> (theSet->toCounts()); 
    }
};

void
FailureUniverse::vote(const vector <RunSet *> & sets, const VotingRule & rule, RunSet & result) const
{
    vector <vector <unsigned int> * > sets_counts; 
    transform(sets.begin(), sets.end(), back_inserter(sets_counts), GetCounts());
    vector <unsigned int> * vote_count = accumulate(sets_counts.begin(), 
               sets_counts.end(), 
               new vector<unsigned int>(mask.size(), 0), 
               Count()); 
    for(unsigned int i = 0; i < vote_count->size(); i++) {
        result.set(i, (rule(mask[i] ? (*vote_count)[i] : 0)));
    }
    delete vote_count;
}

/******************************************************************************
* Calculates mean of RunSet
******************************************************************************/
double
FailureUniverse::mean(const RunSet & X) const
{
    return ((double)((X.value() & mask).count()))/((double)cardinality); 
}

/******************************************************************************
* Calculates covariance of two RunSets
******************************************************************************/
double
FailureUniverse::covariance(const RunSet & X, const RunSet & Y) const
{
    double Xmean = mean(X);
    double Ymean = mean(Y);
    double result = 
      ((double)((getAND(X,Y) & mask).count()) * (1.0 - Xmean) * (1.0 - Ymean)) +
      ((double)((getDIFF(X,Y) & mask).count()) * (1.0 - Xmean) * - Ymean) +
      ((double)((getDIFF(Y,X) & mask).count()) * -Xmean * (1.0 - Ymean)) +
      ((double)((getOR(X,Y).flip() & mask).count()) * -Xmean * -Ymean);
    return ((double)result)/((double)cardinality);
}

/*******************************************************************************
* Calculate entropy of a single run set
*******************************************************************************/
double
FailureUniverse::entropyTerm(double prob) const
{
    return prob == 0.0 ? 0.0 : prob * log2(1.0/ prob); 
}

double
FailureUniverse::entropy(const RunSet & X) const
{
    return entropyTerm(p_Xtrue(X)) + entropyTerm(p_Xfalse(X));
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
    return (getAND(X,Y) & mask).count();
}

/******************************************************************************
* Joint probability that both are true. 
******************************************************************************/
double
FailureUniverse::p_Xtrue_Ytrue(const RunSet & X, const RunSet & Y) const 
{
    return ((double)c_Xtrue_Ytrue(X,Y))/((double)cardinality); 
}


/******************************************************************************
* Joint probability that X is true and Y false. 
******************************************************************************/
double
FailureUniverse::p_Xtrue_Yfalse(const RunSet & X, const RunSet & Y) const
{
    return ((double)(getDIFF(X,Y) & mask).count())/((double)cardinality);
}

/******************************************************************************
* Joint probability that both are false. We assume that the number of 
* failing runs in the test set is non-negative.
******************************************************************************/
double
FailureUniverse::p_Xfalse_Yfalse(const RunSet & X, const RunSet & Y) const
{
    return ((getOR(X,Y).flip() & mask).count())/((double)cardinality);
}

/******************************************************************************
* Marginal probability that X is true.
******************************************************************************/
double
FailureUniverse::p_Xtrue(const RunSet & X) const
{
    return ((double)(X.value() & mask).count()/(double)cardinality);  
}

/******************************************************************************
* Marginal probability that X is false.
******************************************************************************/
double
FailureUniverse::p_Xfalse(const RunSet & X) const
{
    return ((double)((X.value().flip() & mask).count()))/ ((double)cardinality);
}
