#ifndef FAILURE_UNIVERSE_H
#define FAILURE_UNIVERSE_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "RunSet.h"

class EmptyUniverseException
{
};

class VotingRule {
public:
    virtual bool operator()(unsigned int count) const = 0;
};

class FailureUniverse
{
public:
    FailureUniverse();
    double mean(const RunSet &) const;
    double covariance(const RunSet &, const RunSet &) const;
    double entropy(const RunSet &) const;
    unsigned int c_Xtrue_Ytrue(const RunSet &, const RunSet &) const;
    double p_Xtrue_Ytrue(const RunSet &, const RunSet &) const;
    double p_Xtrue_Yfalse(const RunSet &, const RunSet &) const;
    double p_Xfalse_Yfalse(const RunSet &, const RunSet &) const;
    double p_Xtrue(const RunSet &) const;
    double p_Xfalse(const RunSet &) const;
    double MI(const RunSet &, const RunSet &) const;
    double signedMI(const RunSet &, const RunSet &) const;
    void vote(const vector <RunSet *> &, const VotingRule &, RunSet & result) const;

private:
    unsigned int cardinality;
    unsigned int begin;
    unsigned int end;
    dynamic_bitset<> mask;
    double MIterm(double, double, double) const;
    double entropyTerm(double) const;
    double log2(double) const;
};

#endif // !FAILURE_UNIVERSE_H
