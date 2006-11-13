#include <iostream>
#include <fstream>

#include "conjoin.h"

#include "Confidence.h"
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
    
    Confidence::level = 0;
}

void gen_conjunctions() {
    initialize();

    Candidates candidates;
    candidates.load();
    candidates.sort();
    candidates.reverse();
    
    Candidates::iterator i = candidates.begin();
    cout << (*i).score() << endl;
    
    std::list<Conjunction> result = conjoin(candidates, 1);
    std::list<Conjunction>::iterator winner = max_element(result.begin(), result.end());
    cout << "Number of interesting conjunctions: " << result.size() << endl;
    cout << "(Max possible: " << candidates.size() * candidates.size() << ")\n";
    
    cout << "Best conjunction: Score " << (*winner).score() << endl;
    cout << "The two predicates: " << endl;
    cout << *winner << endl;
        
    const Candidates::iterator wi1 = max_element(candidates.begin(), candidates.end());
    cout << "Best single predicate's Score " << (*wi1).score() << " and it is" << endl;
    cout << *wi1 << endl;
    
}

