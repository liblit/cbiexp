#include <argp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include "../RunsDirectory.h"
#include "../NumRuns.h"
#include "../Progress/Unbounded.h"
#include "../Bugs.h"
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

class IntersectionSize : public binary_function <FRunSet *, FRunSet *, unsigned int> {
public:
    unsigned int operator()(const FRunSet * left, const FRunSet * right) const {
        return FailureUniverse::getUniverse().intersectionSize(*left, *right);
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
    string line;
    istringstream parse;
    /***************************************************************************
    * We read the runs associated with each bug 
    ***************************************************************************/
    ifstream bug_runs_file("bug_runs.txt");
    for(unsigned int i = 0; i < bugIds->size(); i++) {
        FRunSet current = FailureUniverse::getUniverse().makeFRunSet(); 
        getline(bug_runs_file, line);
        parse.clear();
        parse.str(line);
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
    getline(bug_runs_file, line);
    parse.clear();
    parse.str(line);
    parse >> unknown_runs;
    bug_runs.push_back(new FRunSet(unknown_runs));
    bug_runs_file.close();

    /***************************************************************************
    * We calculate the entropy for the runs
    ***************************************************************************/
    vector <double> bug_runs_entropy;
    FailureUniverse::getUniverse().entropy(bug_runs, bug_runs_entropy);

    /***************************************************************************
    * We read the run sets decided by our predicate sets. 
    * We print out intersection counts. 
    ***************************************************************************/
    Progress::Unbounded progress("reading run sets");
    while(cin.peek() != EOF) {
        progress.step();
        getline(cin, line);
        parse.clear();
        parse.str(line);
        FRunSet current = FailureUniverse::getUniverse().makeFRunSet(); 
        parse >> current;
        vector <unsigned int> counts;
        transform(bug_runs.begin(), 
                  bug_runs.end(), 
                  ostream_iterator<unsigned int>(cout, "\t"),
                  bind1st(IntersectionSize(), &current));
        cout << "\n";
    }
}
