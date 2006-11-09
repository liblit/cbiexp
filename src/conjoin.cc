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

void conjoin() {
    initialize();

    Candidates candidates;
    list<Conjunction> all;
    candidates.load();
    
   Candidates::iterator i, j;
    for(i = candidates.begin(); i != candidates.end(); i ++)
        for(j = i; j != candidates.end(); j ++) {
	    if ( i == j )
	      continue;
            Conjunction c(&*i, &*j);
            if(c.isInteresting()) {
                all.push_back(c);
            }
        }
    list<Conjunction>::iterator winner = max_element(all.begin(), all.end());
    cout << "Number of interesting conjunctions: " << all.size() << endl;
    cout << "(Max possible: " << candidates.size() * candidates.size() << ")\n";
    
    cout << "Best conjunction: Score " << (*winner).score() << endl;
    cout << "The two predicates: " << endl;
    cout << *winner << endl;
        
    const Candidates::iterator wi1 = max_element(candidates.begin(), candidates.end());
    cout << "Best single predicate's Score " << (*wi1).score() << " and it is" << endl;
    cout << *wi1 << endl;
    
}

