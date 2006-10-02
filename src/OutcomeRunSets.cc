#include <iostream>
#include <sstream>
#include <vector>

#include "OutcomeRunSets.h"
#include "RunSet.h"

using namespace std;

////////////////////////////////////////////////////////////////////////
//
//  a pair of run sets segregated by outcome
//


void
OutcomeRunSets::insert(Outcome outcome, unsigned runId)
{
    (this->*outcome).insert(runId);
}

void
OutcomeRunSets::load(istream &in, RunSet & outcome) {
    string actual;
    in >> actual;
    if(in) {
        assert(actual.size() == 2);
        assert(actual[0] == (&outcome == &(this->failure) ? 'F' : 'S'));
        assert(actual[1] == ':');

        string line;
        getline(in, line);

        istringstream parse(line);
        parse >> outcome; 
    }
}

istream &
operator>>(istream &in, OutcomeRunSets &predicate)
{
    predicate.load(in, predicate.failure);
    predicate.load(in, predicate.success);
    return in;
}    

ostream &
operator<<(ostream &out, const OutcomeRunSets &predicate)
{
    return out << "F:" << predicate.failure << '\n'
	       << "S:" << predicate.success << '\n';
}
