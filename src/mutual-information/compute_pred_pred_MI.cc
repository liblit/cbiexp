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

class SignedMutualInformation : public binary_function <FRunSet *, FRunSet *, double> {
public:
   double operator()(const FRunSet * X, const FRunSet * Y) const {
       return FailureUniverse::getUniverse().signedMI(*X, *Y); 
   }
};

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

    ofstream out("pred_pred_MI.txt");
    Progress::Bounded progress("calculating mutual information", numPreds);
    for(unsigned int i = 0; i < numPreds; i++) {
        progress.step();
        transform(pred_tru_runs.begin(), pred_tru_runs.end(), ostream_iterator<double>(out, "\t"), bind1st(SignedMutualInformation(), (pred_tru_runs[i])));
        out << "\n";
    }
    out.close(); 

}
