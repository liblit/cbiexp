#include <argp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../RunsDirectory.h"
#include "../NumRuns.h"
#include "../Progress/Unbounded.h"
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

    ofstream out("pred_set_runs.csv");
    /***************************************************************************
    * We construct the vector of runIds in the failure universe and print out
    * the header column.
    ***************************************************************************/
    vector <unsigned int> runIds = FailureUniverse::getUniverse().getIndices();
    for(unsigned int i = 0; i < runIds.size(); i++) {
        out << ";" << runIds[i];
    }
    out << "\n";

    /***************************************************************************
    * We read the run sets decided by our predicate sets. We print out the
    * run sets in csv format 
    ***************************************************************************/
    ifstream runs_file("pred_set_run_sets.txt");
    Progress::Unbounded progress("reading run sets");
    unsigned int set_index = 0;
    while(runs_file.peek() != EOF) {
        progress.step();
        out << set_index++;
        string line;
        getline(runs_file, line);
        istringstream parse(line);
        FRunSet current = FailureUniverse::getUniverse().makeFRunSet();
        parse >> current;
        for(unsigned int i = 0; i < runIds.size(); i++) {
            out << ";" << (current.test(runIds[i]) ? 1 : 0);
        }
        out << "\n";
    }
    out.close();
    runs_file.close();
}
