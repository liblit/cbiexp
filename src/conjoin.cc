#include <iostream>
#include <fstream>

#include "conjoin.h"

#include "corrective-ranking/allFailures.h"
#include "corrective-ranking/RunSet.h"
#include "corrective-ranking/Candidates.h"
#include "corrective-ranking/Conjunction.h"

using namespace std;


void
RunSet::dilute(const Predicate &, const RunSet &winner) {
    assert(size() == winner.size());
    
    // Nothing
}

void initialize() {
    ifstream failuresFile("f.runs");
    assert(failuresFile);
    failuresFile >> allFailures;
}

void conjoin() {
    initialize();
    
    Candidates candidates, all, working;
    candidates.load();
    
    cout << "\n\n CONJOIN: Dry run: Just printing\n";
    
    while (!candidates.empty())
    {
        const Candidates::iterator winner = max_element(candidates.begin(), candidates.end());
        cout << *winner;
    }
}

