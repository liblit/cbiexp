#ifndef INCLUDE_PredSet_h
#define INCLUDE_PredSet_h

#include <iostream>
#include <vector>
#include "SetVector.h"

using namespace std;

class PredSet : public SetVector {
public:
    PredSet();
    friend void computeOR(const PredSet &, const PredSet &, PredSet &);
    friend void computeAND(const PredSet &, const PredSet &, PredSet &);
    friend bool nonEmptyIntersection(const PredSet &, const PredSet &);
private:
    PredSet& operator=(const PredSet &); 
};

#endif // !INCLUDE_RunSet_h

