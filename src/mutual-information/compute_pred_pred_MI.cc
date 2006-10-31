#include <argp.h>
#include <algorithm>
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
#include "util.h"
#include "FailureUniverse.h"
#include "SuccessUniverse.h"

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
    * We do this separately over true runs and false runs. 
    **************************************************************************/
    ifstream tru("tru.txt");

    const unsigned numPreds = PredStats::count();
    Progress::Bounded reading("reading predicate info", numPreds);
    vector <FRunSet *> failing_runs;
    vector <SRunSet *> succeeding_runs;
    for (unsigned int count = 0; count < numPreds; ++count) { 
        reading.step();
        OutcomeRunSets current;
        tru >> current; 
        FRunSet failures = FailureUniverse::getUniverse().makeFRunSet(); 
        failures.load(current.failure.value());
        failing_runs.push_back(new FRunSet(failures));
        SRunSet successes = SuccessUniverse::getUniverse().makeSRunSet();
        successes.load(current.success.value());
        succeeding_runs.push_back(new SRunSet(successes));
    }
    assert(tru.peek() == EOF);
    tru.close(); 

    /**************************************************************************
    * Calculating predicate entropy for failing runs
    **************************************************************************/
    vector <double> pred_entropy_failingruns; 
    FailureUniverse::getUniverse().entropy(failing_runs, pred_entropy_failingruns);

    /*************************************************************************
    * Printing out the entropy
    *************************************************************************/
    ofstream out("pred_entropy_failing-runs.txt");
    copy(pred_entropy_failingruns.begin(), pred_entropy_failingruns.end(), ostream_iterator<double>(out, "\n"));
    out.close();

    /**************************************************************************
    * Calculate predicate entropy for succesful runs
    **************************************************************************/
    vector <double> pred_entropy_succeedingruns;
    SuccessUniverse::getUniverse().entropy(succeeding_runs, pred_entropy_succeedingruns);

    /**************************************************************************
    * Printing out predicate entropy for succesful runs
    **************************************************************************/
    out.open("pred_entropy_succeeding-runs.txt");
    copy(pred_entropy_succeedingruns.begin(), pred_entropy_succeedingruns.end(), ostream_iterator<double>(out, "\n"));
    out.close();

    /**************************************************************************
    * Calculating signed mutual information and normalized signed mi
    **************************************************************************/
    out.open("pred_pred_MI_failing-runs.txt");
    ofstream nout("pred_pred_MI_normalized_failing-runs.txt"); 
    ofstream rout("pred_pred_redundancy_failing-runs.txt");
    ofstream outs("pred_pred_MI_succeeding-runs.txt");
    ofstream nouts("pred_pred_MI_normalized_succeeding-runs.txt"); 
    ofstream routs("pred_pred_redundancy_succeeding-runs.txt");
    Progress::Bounded progress("calculating mutual information", numPreds);
    for(unsigned int i = 0; i < numPreds; i++) {
        progress.step();
        vector <double> mi;
        FailureUniverse::getUniverse().signedMI(*failing_runs[i], failing_runs, mi);
        copy(mi.begin(), mi.end(), ostream_iterator<double>(out, "\t"));
        out << "\n";
        for(unsigned int j = 0; j < numPreds; j++) {
            nout << (normalize(pred_entropy_failingruns[i],
                               pred_entropy_failingruns[j],
                               mi[j])) << "\t";
            rout << (redundify(pred_entropy_failingruns[i],
                               pred_entropy_failingruns[j],
                               mi[j])) << "\t";
        }
        nout << "\n";
        rout << "\n";
        mi.clear();
        SuccessUniverse::getUniverse().signedMI(*succeeding_runs[i], succeeding_runs, mi);
        copy(mi.begin(), mi.end(), ostream_iterator<double>(outs, "\t"));
        outs << "\n";
        for(unsigned int j = 0; j < numPreds; j++) {
            nouts << (normalize(pred_entropy_succeedingruns[i],
                               pred_entropy_succeedingruns[j],
                               mi[j])) << "\t";
            routs << (redundify(pred_entropy_succeedingruns[i],
                               pred_entropy_succeedingruns[j],
                               mi[j])) << "\t";
        }
        nouts << "\n";
        routs << "\n";
    }
    out.close(); 
    nout.close();
    rout.close();
    outs.close();
    nouts.close();
    routs.close();

}
