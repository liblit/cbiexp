#ifndef INCLUDE_RunSet_h
#define INCLUDE_RunSet_h

#include <iostream>
#include <vector>
#include "SetVector.h"

using namespace std;

class RunSet : public SetVector {
public:
    RunSet();
};

ostream & operator<<(ostream &, const RunSet &);
istream & operator>>(istream &, RunSet &); 

#endif // !INCLUDE_RunSet_h

