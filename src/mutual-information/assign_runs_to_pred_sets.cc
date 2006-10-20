#include <argp.h>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include "../RunsDirectory.h"
#include "../Progress/Bounded.h"
#include "../Progress/Unbounded.h"
#include "../OutcomeRunSets.h"
#include "../NumRuns.h"
#include "../RunSet.h"
#include "FailureUniverse.h"
#include "PredUniverse.h"
#include "../PredStats.h"

using namespace std;

#ifdef HAVE_ARGP_H

static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
        { &RunsDirectory::argp, 0, 0, 0 }, 
        { &NumRuns::argp, 0, 0, 0 },
        { 0, 0, 0, 0 }
    };

    static const argp argp = {
        0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0); 
}

#else // !HAVE_ARGP_H

inline void process_cmdline(int, char *[]) { }

#endif // HAVE_ARGP_H

class ExtractFailures : public unary_function <OutcomeRunSets *, FRunSet *> {
public:
    FRunSet * operator()(OutcomeRunSets * outcomes) const {
        FRunSet current = FailureUniverse::getUniverse().makeFRunSet();
        current.load((outcomes->failure).value());
        return new FRunSet(current);
    }
};

class Majority : public VotingRule {
public:
    Majority(unsigned int divisor) {
        this->divisor = divisor;
    }
    bool operator()(unsigned int count) const {
         return ((double)count/(double)divisor) > 0.5 ? true : false;     
    }
private:
    unsigned int divisor;
};

class Unanimous : public VotingRule {
public:
    Unanimous(unsigned int num_voters) {
        this->num_voters = num_voters;
    }
    bool operator()(unsigned int count) const {
        return count == num_voters; 
    }
private:
    unsigned int num_voters;
};

class AtLeastOne : public VotingRule {
public:
    bool operator()(unsigned int count) const {
      return count > 0;
    }
};

class FindRunSets : public unary_function <vector <OutcomeRunSets *> *, FRunSet *> {
public:
    FRunSet * operator()(vector <OutcomeRunSets *> * outcomes) const {
        vector <FRunSet *> run_sets; 
        transform(outcomes->begin(), outcomes->end(), back_inserter(run_sets), ExtractFailures());
        FRunSet result = FailureUniverse::getUniverse().makeFRunSet();
        FailureUniverse::getUniverse().vote(run_sets, Majority(run_sets.size()), result);
        return new FRunSet(result); 
    } 
};

class FindOutcomeRunSets : public unary_function <PSet *, vector <OutcomeRunSets *> * > {
public:
    FindOutcomeRunSets (const vector <OutcomeRunSets *> & outcomes) {
        this->outcomes = &outcomes;
    }
    vector <OutcomeRunSets * > * operator()(const PSet * thePreds) const {
        unsigned int numPreds = PredStats::count();
        assert(numPreds == outcomes->size());
        vector <OutcomeRunSets *> * result = new vector<OutcomeRunSets *>();
        for(unsigned int i = 0; i < numPreds; i++) {
            if(thePreds->test(i)) result->push_back((*outcomes)[i]);
        }
        return result;
    }
private:
    const vector <OutcomeRunSets *> * outcomes;
};

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    /**************************************************************************
    * Read in each pred set.
    **************************************************************************/
    vector < PSet * > sets; 
    ifstream pred_sets("pred_sets.txt");
    Progress::Unbounded reading("reading predicate sets");
    while(pred_sets.peek() != EOF) {
        reading.step();
        string line;
        getline(pred_sets, line);
        istringstream parse(line);
        PSet current = PredUniverse::getUniverse().makePredSet(); 
        parse >> current;
        sets.push_back(new PSet(current));
    }
    pred_sets.close();

    /**************************************************************************
    * Read in outcomes
    **************************************************************************/
    ifstream tru("tru.txt");
    vector <OutcomeRunSets *> outcomes;
    const unsigned numPreds = PredStats::count();
    Progress::Bounded progress("reading outcomes for predicate", numPreds);
    for(unsigned int count = 0; count < numPreds; ++count) {
        progress.step();
        OutcomeRunSets * current = new OutcomeRunSets();
        tru >> *current;
        outcomes.push_back(current);
    }
    assert(tru.peek() == EOF);
    tru.close();

    /*************************************************************************
    * For each set, find the failing runs it claims and print them out.
    *************************************************************************/
    vector <vector <OutcomeRunSets *> * > outcomes_sets; 
    transform(sets.begin(), sets.end(), back_inserter(outcomes_sets), FindOutcomeRunSets(outcomes));  
    vector <FRunSet *> results;
    transform(outcomes_sets.begin(), outcomes_sets.end(), back_inserter(results), FindRunSets());
    ofstream out("pred_set_runs.txt");
    for(unsigned int i = 0; i < results.size(); i++) {
        out << *results[i];
        out << "\n";
    }
    out.close();

}
