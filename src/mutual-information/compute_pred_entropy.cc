#include <argp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../fopen.h" 
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

    ifstream tru("tru.txt");

    const unsigned numPreds = PredStats::count();
    Progress::Bounded progress("calculating predicate entropy", numPreds);
    FILE* out = fopenWrite("pred_entropy.txt"); 
    for (unsigned int count = 0; count < numPreds; ++count) { 
        progress.step();
        OutcomeRunSets current;
        tru >> current; 

        /*********************************************************************
        * Print predicate entropy 
        *********************************************************************/
        FRunSet failures = FailureUniverse::getUniverse().makeFRunSet();
        failures.load(current.failure.value());
        fprintf(out, "%g\n", FailureUniverse::getUniverse().entropy(failures));
    }
    assert(tru.peek() == EOF);
    fclose(out); 
    tru.close();

}