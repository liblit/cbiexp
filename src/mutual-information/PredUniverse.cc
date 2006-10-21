#include "PredUniverse.h"
#include "../PredStats.h"

PSet::PSet()
{
}

PUniverse::PUniverse()
{
    cardinality = PredStats::count();
}

PredUniverse::PredUniverse()
{
}

const PredUniverse &
PredUniverse::getUniverse()
{
    static PredUniverse univ;
    return univ;
}

PSet
PredUniverse::makePredSet() const
{
    return PSet();
}

bool
PredUniverse::nonEmptyIntersection(const PSet & left, const PSet & right) const
{
    return univ.nonEmptyIntersection(left,right);
}

void
PredUniverse::computeUnion(const PSet & left, const PSet & right, PSet & result) const
{
    return univ.computeUnion(left, right, result);
}

unsigned int
PredUniverse::cardinality() const
{
    return univ.cardinality;
}

double
PredUniverse::signedMI(const PSet & left, const PSet & right) const
{
    return univ.signedMI(left, right); 
}

double
PredUniverse::entropy(const PSet & theSet) const
{
    return univ.entropy(theSet);
}
