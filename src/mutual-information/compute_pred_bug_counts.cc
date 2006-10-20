#include <argp.h>
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
#include "FailureUniverse.h"
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

class IntersectionSize : public binary_function <FRunSet *, FRunSet *, unsigned int> {
public:
    unsigned int operator()(const FRunSet * X, const FRunSet * Y) const {
        return FailureUniverse::getUniverse().intersectionSize(*X,*Y);  
    }
};

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

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
    * For each predicate we find the number of runs corresponding to any 
    * particular bug or to an unknown bug in which the predicate has been 
    * observed true and print out the table of this data.
    **************************************************************************/
    ifstream tru("tru.txt");

    const unsigned numPreds = PredStats::count();
    Progress::Bounded progress("assigning predicates to runs", numPreds);
    ofstream out("pred_bug_counts.txt"); 
    for (unsigned int count = 0; count < numPreds; ++count) { 
        progress.step();
        OutcomeRunSets current;
        tru >> current; 

        /*********************************************************************
        * Print predicate/count table
        *********************************************************************/
        FRunSet failures = FailureUniverse::getUniverse().makeFRunSet(); 
        failures.load(current.failure.value());
        transform(bug_runs.begin(), bug_runs.end(), ostream_iterator<unsigned int> (out, "\t"), bind1st(IntersectionSize(), new FRunSet(failures)));
        out << "\n";
    }
    assert(tru.peek() == EOF); 
    out.close(); 
    tru.close();

}
