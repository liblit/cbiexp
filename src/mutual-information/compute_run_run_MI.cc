#include <argp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include "../RunsDirectory.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../OutcomeRunSets.h"
#include "../RunSet.h"
#include "../PredStats.h"
#include "FailureUniverse.h"
#include "PredUniverse.h"

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

class Entropy : public unary_function <PSet *, double>
{
public:
    double operator()(const PSet * X) const {
        return PredUniverse::getUniverse().entropy(*X);
    }
};

class SignedMutualInformation : public binary_function <PSet *, PSet *, double> {
public:
   double operator()(const PSet * X, const PSet * Y) const {
       return PredUniverse::getUniverse().signedMI(*X, *Y); 
   }
};

class MakePredSet : public unary_function <unsigned int, PSet *> {
public:
    MakePredSet(const vector <RunSet *> & oc) 
    : outcomes(oc) 
    {
    }

    PSet * operator()(unsigned int runId) const {
        PSet result = PredUniverse::getUniverse().makePredSet();
        for(unsigned int i = 0;
            i < PredUniverse::getUniverse().cardinality();
            i++) {
             if(outcomes[i]->test(runId)) result.set(i);
        }
        return new PSet(result);
    }

private:
    const vector <RunSet *> & outcomes;
};

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    /**************************************************************************
    * Read in the outcomes for intersting predicates
    **************************************************************************/
    ifstream tru("tru.txt");

    const unsigned numPreds = PredStats::count();
    Progress::Bounded reading("reading predicate info", numPreds);
    vector <RunSet *> pred_tru_runs;
    for (unsigned int count = 0; count < numPreds; ++count) { 
        reading.step();
        OutcomeRunSets current;
        tru >> current; 
        pred_tru_runs.push_back(new RunSet(current.failure));
    }
    assert(tru.peek() == EOF);
    tru.close(); 

    /**************************************************************************
    * Form PredSets corresponding to each run of interest
    **************************************************************************/
    vector <PSet *> pred_sets;
    vector <unsigned int> failureIndices = FailureUniverse::getUniverse().getIndices(); 
    transform(failureIndices.begin(), 
              failureIndices.end(),
              back_inserter(pred_sets), 
              MakePredSet(pred_tru_runs)); 

    /**************************************************************************
    * Now we calculate the entropy for each run
    **************************************************************************/
    vector <double> run_entropy;
    transform(pred_sets.begin(),
              pred_sets.end(),
              back_inserter(run_entropy),
              Entropy());
    
    /**************************************************************************
    * We print out the entropy for each run for future reference
    **************************************************************************/
    ofstream out("run_entropy.txt"); 
    copy(run_entropy.begin(), 
         run_entropy.end(), 
         ostream_iterator<double>(out, "\n"));
    out.close();
              
    /**************************************************************************
    * Now we calculate the mutual information
    **************************************************************************/
    out.open("run_run_MI.txt");
    Progress::Bounded progress("calculating mutual information", pred_sets.size());
    for(unsigned int i = 0; i < pred_sets.size(); i++) {
        progress.step();
        transform(pred_sets.begin(), pred_sets.end(),
        ostream_iterator<double>(out, "\t"), bind1st(SignedMutualInformation(), (pred_sets[i])));
        out << "\n";
    }
    out.close(); 

}
