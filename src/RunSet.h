#ifndef INCLUDE_RunSet_h
#define INCLUDE_RunSet_h

#include <iostream>
#include <vector>
#include "SetVector.h"

using namespace std;
using namespace boost;

class RunSet : public SetVector {
public:
    RunSet();
    friend void computeOR(const RunSet &, const RunSet &, RunSet &);
    friend void computeAND(const RunSet &, const RunSet &, RunSet &);
    friend bool nonEmptyIntersection(const RunSet &, const RunSet &);
    friend imp getOR(const RunSet &, const RunSet &);
    friend imp getAND(const RunSet &, const RunSet &);
    friend imp getDIFF(const RunSet &, const RunSet &);
};

#endif // !INCLUDE_RunSet_h

