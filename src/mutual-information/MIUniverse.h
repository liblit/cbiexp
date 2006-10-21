#ifndef MI_UNIVERSE_H
#define MI_UNIVERSE_H

#include <vector>
#include <list>
#include <boost/dynamic_bitset.hpp>
#include "../SetVector.h"

class EmptyUniverseException
{
};

class VotingRule {
public:
    virtual bool operator()(unsigned int count) const = 0;
};

class MIUniverse
{
public:
    virtual ~MIUniverse() = 0; 

    unsigned int intersectionSize(const SetVector &, const SetVector &) const;
    bool nonEmptyIntersection(const SetVector &, const SetVector &) const; 
    double mean(const SetVector &) const;
    double covariance(const SetVector &, const SetVector &) const;
    double entropy(const SetVector &) const;
    double MI(const SetVector &, const SetVector &) const;
    double signedMI(const SetVector &, const SetVector &) const;
    void vote(const vector <SetVector *> &, const VotingRule &, SetVector &) const;
    void computeUnion(const SetVector &, const SetVector &, SetVector &) const; 
    void coalesce(list <SetVector *> &, vector <SetVector *> &) const;

protected:
    double p_Xtrue_Ytrue(const SetVector &, const SetVector &) const;
    double p_Xtrue_Yfalse(const SetVector &, const SetVector &) const;
    double p_Xfalse_Yfalse(const SetVector &, const SetVector &) const;
    double p_Xtrue(const SetVector &) const;
    double p_Xfalse(const SetVector &) const;


    /***************************************************************************
    * virtual methods
    ***************************************************************************/
    virtual unsigned int c_Xtrue_Ytrue(const SetVector &, const SetVector &) const;
    virtual unsigned int c_Xtrue_Yfalse(const SetVector &, const SetVector &) const;
    virtual unsigned int c_Xfalse_Yfalse(const SetVector &, const SetVector &) const;
    virtual unsigned int c_Xtrue(const SetVector &) const;
    virtual unsigned int c_Xfalse(const SetVector &) const;

    /***************************************************************************
    * The size of the universe
    ***************************************************************************/
    unsigned int cardinality;


private:
    /**************************************************************************
    * These methods are called only by the methods in public interface 
    **************************************************************************/
    double MIterm(double, double, double) const;
    double entropyTerm(double) const;
    double log2(double) const;
    void coalesceStep(list <SetVector *> &, SetVector &) const;
};

#endif // !MI_UNIVERSE_H
