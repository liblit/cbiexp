#include <argp.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "RunsDirectory.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "Bugs.h"
#include "RunSet.h"
#include "OutcomeRunSets.h"
#include "FailureUniverse.h"
#include "PredStats.h"

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

class IntersectionSize : public binary_function <RunSet *, RunSet *, unsigned int> {
public:
    IntersectionSize(const FailureUniverse * univ) {
        this->univ = univ;
    }

    unsigned int operator()(const RunSet * X, const RunSet * Y) const {
        return univ->c_Xtrue_Ytrue(*X,*Y);  
    }
private:
    const FailureUniverse * univ;
};

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    vector <int> * bugIds = (new Bugs())->bugIndex();

    const FailureUniverse univ;

    vector <RunSet *> bug_runs;
    /***************************************************************************
    * We read the runs associated with each bug 
    ***************************************************************************/
    ifstream bug_runs_file("bug_runs.txt");
    for(unsigned int i = 0; i < bugIds->size(); i++) {
        RunSet* current = new RunSet();  
        string line;
        getline(bug_runs_file, line);
        istringstream parse(line);
        parse >> *current;
        bug_runs.push_back(current);
    }
    bug_runs_file.close();

    /***************************************************************************
    * Some runs may be unknown
    ***************************************************************************/
    RunSet * unknown_runs = new RunSet();

    /**************************************************************************
    * We read the runs that haven't been assigned to any bug.
    **************************************************************************/
    bug_runs_file.open("unknown_runs.txt");
    string line;
    getline(bug_runs_file, line);
    istringstream parse(line);
    parse >> *unknown_runs;
    bug_runs.push_back(unknown_runs);
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
        transform(bug_runs.begin(), bug_runs.end(), ostream_iterator<unsigned int> (out, "\t"), bind1st(IntersectionSize(&univ), &current.failure));
        out << "\n";
    }
    assert(tru.peek() == EOF); 
    out.close(); 
    tru.close();

}
