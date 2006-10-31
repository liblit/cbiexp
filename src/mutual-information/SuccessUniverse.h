#ifndef SUCCESS_UNIVERSE_H
#define SUCCESS_UNIVERSE_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "MIUniverse.h"
#include "../SetVector.h"
#include "../RunSet.h"

class SRunSet : public RunSet {
public:
    virtual void set(size_t, bool val = true);
    virtual void load(istream &);
    virtual void load(const imp &);
    virtual void load(const vector <bool> &); 

private:
    SRunSet();
    SRunSet(const dynamic_bitset<> &);
    const dynamic_bitset<> & mask;
    friend class SuccessUniverse;
};

class SUniverse : public MIUniverse
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
    SUniverse();
    SUniverse(const SUniverse &);
    unsigned int begin;
    unsigned int end;
    dynamic_bitset<> mask;
    friend class SuccessUniverse;
};

class SuccessUniverse
{
public:
    static const SuccessUniverse & getUniverse();  
    SRunSet makeSRunSet() const;
    void vote(const vector <SRunSet*> &, const VotingRule &, SRunSet & result) const;
    double entropy(const SRunSet &) const;
    void entropy(const vector <SRunSet *> &, vector <double> &) const;
    double signedMI(const SRunSet &, const SRunSet &) const; 
    void signedMI(const SRunSet &, const vector <SRunSet *> &, vector <double> &) const;
    unsigned int intersectionSize(const SRunSet &, const SRunSet &) const; 
    vector <unsigned int> getIndices() const;
    unsigned int cardinality() const;
    bool nonEmptyIntersection(const SRunSet &, const SRunSet &) const;
    void coalesce(const list <SRunSet *> &, vector <SRunSet *> &) const;
    unsigned int cardinality(const SRunSet &) const;
    void accumulateUnion(list<SRunSet*>::iterator, list<SRunSet*>::iterator, SRunSet &) const;

private:
    SuccessUniverse();
    SuccessUniverse(const SuccessUniverse &);
    SUniverse univ;

};
#endif // !SUCCESS_UNIVERSE_H
