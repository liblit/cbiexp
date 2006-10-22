#ifndef FAILURE_UNIVERSE_H
#define FAILURE_UNIVERSE_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "MIUniverse.h"
#include "../SetVector.h"
#include "../RunSet.h"

class FRunSet : public RunSet {
public:
    virtual void set(size_t, bool val = true);
    virtual void load(istream &);
    virtual void load(const imp &);
    virtual void load(const vector <bool> &); 

private:
    FRunSet();
    FRunSet(const dynamic_bitset<> &);
    const dynamic_bitset<> & mask;
    friend class FailureUniverse;
};

class FUniverse : public MIUniverse
{
public:
    virtual vector <unsigned int> getIndices() const;

protected:
    /***************************************************************************
    * virtual methods
    ***************************************************************************/
    virtual unsigned int c_Xtrue_Ytrue(const SetVector &, const SetVector &) const;
    virtual unsigned int c_Xtrue_Yfalse(const SetVector &, const SetVector &) const;
    virtual unsigned int c_Xfalse_Yfalse(const SetVector &, const SetVector &) const;
    virtual unsigned int c_Xtrue(const SetVector &) const;
    virtual unsigned int c_Xfalse(const SetVector &) const;

private:
    FUniverse();
    FUniverse(const FUniverse &);
    unsigned int begin;
    unsigned int end;
    dynamic_bitset<> mask;
    friend class FailureUniverse;
};

class FailureUniverse
{
public:
    static const FailureUniverse & getUniverse();  
    FRunSet makeFRunSet() const;
    void vote(const vector <FRunSet*> &, const VotingRule &, FRunSet & result) const;
    double entropy(const FRunSet &) const;
    void entropy(const vector <FRunSet *> &, vector <double> &) const;
    double signedMI(const FRunSet &, const FRunSet &) const; 
    void signedMI(const FRunSet &, const vector <FRunSet *> &, vector <double> &) const;
    unsigned int intersectionSize(const FRunSet &, const FRunSet &) const; 
    vector <unsigned int> getIndices() const;
    unsigned int cardinality() const;
    bool nonEmptyIntersection(const FRunSet &, const FRunSet &) const;
    void coalesce(const list <FRunSet *> &, vector <FRunSet *> &) const;
    unsigned int cardinality(const FRunSet &) const;
    void accumulateUnion(list<FRunSet*>::iterator, list<FRunSet*>::iterator, FRunSet &) const;

private:
    FailureUniverse();
    FailureUniverse(const FailureUniverse &);
    FUniverse univ;

};
#endif // !FAILURE_UNIVERSE_H
