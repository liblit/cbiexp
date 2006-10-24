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
#include "../PredStats.h"
#include "FailureUniverse.h"

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

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    /**************************************************************************
    * Calculate the mutual information between predicates. 
    **************************************************************************/
    ifstream tru("tru.txt");

    const unsigned numPreds = PredStats::count();
    Progress::Bounded reading("reading predicate info", numPreds);
    vector <FRunSet *> pred_tru_runs;
    for (unsigned int count = 0; count < numPreds; ++count) { 
        reading.step();
        OutcomeRunSets current;
        tru >> current; 
        FRunSet failures = FailureUniverse::getUniverse().makeFRunSet(); 
        failures.load(current.failure.value());
        pred_tru_runs.push_back(new FRunSet(failures));
    }
    assert(tru.peek() == EOF);
    tru.close(); 

    /**************************************************************************
    * Calculating predicate entropy
    **************************************************************************/
    vector <double> pred_entropy; 
    FailureUniverse::getUniverse().entropy(pred_tru_runs, pred_entropy);

    /*************************************************************************
    * Printing out the entropy
    *************************************************************************/
    ofstream out("pred_entropy.txt");
    copy(pred_entropy.begin(), pred_entropy.end(), ostream_iterator<double>(out, "\n"));
    out.close();

    /**************************************************************************
    * Calculating signed mutual information and normalized signed mi
    **************************************************************************/
    out.open("pred_pred_MI.txt");
    ofstream nout("pred_pred_MI_normalized.txt"); 
    ofstream rout("pred_pred_redundancy.txt");
    Progress::Bounded progress("calculating mutual information", numPreds);
    for(unsigned int i = 0; i < numPreds; i++) {
        progress.step();
        vector <double> mi;
        FailureUniverse::getUniverse().signedMI(*pred_tru_runs[i], pred_tru_runs, mi);
        copy(mi.begin(), mi.end(), ostream_iterator<double>(out, "\t"));
        out << "\n";
        for(unsigned int j = 0; j < numPreds; j++) {
            nout << (mi[j] == 0.0 ? 0.0 : mi[j] / max(pred_entropy[i], pred_entropy[j])) << "\t";
            rout << (mi[j] == 0.0 ? 0.0 : mi[j] / (pred_entropy[i] + pred_entropy[j])) << "\t";
        }
        nout << "\n";
        rout << "\n";
    }
    out.close(); 
    nout.close();
    rout.close();

}
