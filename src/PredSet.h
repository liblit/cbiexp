#ifndef INCLUDE_PredSet_h
#define INCLUDE_PredSet_h

#include <iostream>
#include <vector>
#include "SetVector.h"

using namespace std;

class PredSet : public SetVector {
public:
    PredSet();
    PredSet(vector<bool> &);
};

ostream & operator<<(ostream &, const PredSet &);
istream & operator>>(istream &, PredSet &); 

#endif // !INCLUDE_RunSet_h

