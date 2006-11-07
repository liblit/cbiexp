#include <iostream>
#include <fstream>
#include <argp.h>
#include <boost/dynamic_bitset.hpp>
#include "../RunsDirectory.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../PredStats.h"
#include "../RunSet.h"
#include "../OutcomeRunSets.h"

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
    ofstream matrix("X.txt"); 

    const unsigned numPreds = PredStats::count();
    Progress::Bounded reading("reading predicate info", numPreds);
    for(unsigned int count = 0; count < numPreds; count++) {
        reading.step();
        OutcomeRunSets current;
        tru >> current;
        boost::dynamic_bitset<> combined = current.failure.value() | current.success.value();
        RunSet temp;
        temp.load(combined);
        matrix << temp << "\n";
    }
    tru.close(); 
    matrix.close(); 
}
