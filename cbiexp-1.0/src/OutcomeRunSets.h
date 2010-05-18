#ifndef INCLUDE_OutcomeRunSets_h
#define INCLUDE_OutcomeRunSets_h

#include <iostream>
#include <vector>

#include "RunSet.h"

using namespace std;


class OutcomeRunSets;
typedef RunSet (OutcomeRunSets::* Outcome);

class OutcomeRunSets
{
public:
    RunSet failure;
    RunSet success;

    void insert(Outcome, unsigned);
    void load(istream &, RunSet &);
};

ostream & operator<<(ostream &, const OutcomeRunSets &);
istream & operator>>(istream &, OutcomeRunSets &);

#endif // !INCLUDE_OutcomeRunSets_h

