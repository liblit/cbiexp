#include <argp.h>
#include <math.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../RunsDirectory.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../Bugs.h"
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

    /***************************************************************************
    * Read bug ids
    ***************************************************************************/
    vector <int> * bugIds = (new Bugs())->bugIndex();

    vector <FRunSet *> bug_runs;
    /***************************************************************************
    * We read the runs associated with each bug 
    ***************************************************************************/
    ifstream bug_runs_file("bug_runs.txt");
    for(unsigned int i = 0; i < bugIds->size(); i++) {
        FRunSet current = FailureUniverse::getUniverse().makeFRunSet(); 
        string line;
        getline(bug_runs_file, line);
        istringstream parse(line);
        parse >> current;
        bug_runs.push_back(new FRunSet(current));
    }
    bug_runs_file.close();

    /***************************************************************************
    * Some runs may be unknown
    ***************************************************************************/
    FRunSet unknown_runs = FailureUniverse::getUniverse().makeFRunSet(); 

    /**************************************************************************
    * We read the runs that haven't been assigned to any bug.
    **************************************************************************/
    bug_runs_file.open("unknown_runs.txt");
    string line;
    getline(bug_runs_file, line);
    istringstream parse(line);
    parse >> unknown_runs;
    bug_runs.push_back(new FRunSet(unknown_runs));
    bug_runs_file.close();

    /**************************************************************************
    * Calculate the mutual information between predicate and bug.
    **************************************************************************/
    ifstream tru("tru.txt");

    const unsigned numPreds = PredStats::count();
    Progress::Bounded progress("assigning predicates to runs", numPreds);
    ofstream out("pred_bug_MI.txt"); 
    for (unsigned int count = 0; count < numPreds; ++count) { 
        progress.step();
        OutcomeRunSets current;
        tru >> current; 

        /*********************************************************************
        * Print predicate/MI table
        *********************************************************************/
        FRunSet failures = FailureUniverse::getUniverse().makeFRunSet(); 
        failures.load(current.failure.value());
        transform(bug_runs.begin(), bug_runs.end(), ostream_iterator<double> (out, "\t"), bind1st(SignedMutualInformation(), new FRunSet(failures)));
        out << "\n";
    }
    assert(tru.peek() == EOF);
    out.close();
    tru.close();

}
