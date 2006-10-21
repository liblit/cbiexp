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

class CastToPred : public unary_function <SetVector *, PSet *> {
public:
    CastToPred(const PredUniverse & u) 
    :univ(u)
    {
    }
    PSet * operator()(SetVector * theVec) {
        PSet temp = univ.makePredSet();
        temp.load(theVec->value());
        return new PSet(temp);
    }
private:
    const PredUniverse & univ;
};

void
PredUniverse::coalesce(const list <PSet *> & src, vector <PSet *> & result) const
{
    list <SetVector *> temp_src;
    copy(src.begin(), src.end(), back_inserter(temp_src)); 
    vector <SetVector *> temp_result;
    univ.coalesce(temp_src, temp_result); 
    result.clear();
    transform(temp_result.begin(), 
              temp_result.end(), 
              back_inserter(result),
              CastToPred(*this)); 
}
