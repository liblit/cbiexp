#ifndef Pred_UNIVERSE_H
#define Pred_UNIVERSE_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "MIUniverse.h"
#include "../PredSet.h"

class PSet : public PredSet
{
private:
    PSet();
    friend class PredUniverse;
};

class PUniverse : public MIUniverse
{
private:
    PUniverse();
    PUniverse(const PUniverse &);
    friend class PredUniverse;
};

class PredUniverse
{
public:
    static const PredUniverse & getUniverse();  
    PSet makePredSet() const;
    bool nonEmptyIntersection(const PSet &, const PSet &) const;
    unsigned int cardinality() const;
    double signedMI(const PSet &, const PSet &) const;
    double entropy(const PSet &) const;
    void coalesce(const list <PSet *> &, vector <PSet *> &) const;

private:
    PredUniverse();
    PredUniverse(const PredUniverse &);
    PUniverse univ;

};
#endif // !Pred_UNIVERSE_H
