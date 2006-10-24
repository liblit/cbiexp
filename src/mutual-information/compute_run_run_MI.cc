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
    PredUniverse::getUniverse().entropy(pred_sets, run_entropy);
    
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
    ofstream nout("run_run_MI_normalized.txt");
    ofstream rout("run_run_redundancy.txt");
    Progress::Bounded progress("calculating mutual information", pred_sets.size());
    for(unsigned int i = 0; i < pred_sets.size(); i++) {
        progress.step();
        vector <double> mi;
        PredUniverse::getUniverse().signedMI(*pred_sets[i], pred_sets, mi); 
        copy(mi.begin(), mi.end(), ostream_iterator<double>(out, "\t"));
        out << "\n";
        for(unsigned int j = 0; j < mi.size(); j++) {
            nout << (mi[j] == 0.0 ? 0.0 : mi[j] / max(run_entropy[i], run_entropy[j])) << "\t";
            rout << (mi[j] == 0.0 ? 0.0 : mi[j] /(run_entropy[i] + run_entropy[j])) << "\t";
        }
        nout << "\n";
        rout << "\n";
    }
    out.close(); 
    nout.close();
    rout.close();

}
