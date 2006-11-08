#include <iostream>
#include <fstream>
#include <argp.h>
#include <boost/dynamic_bitset.hpp>
#include "../RunsDirectory.h"
#include "../NumRuns.h"
#include "../classify_runs.h"
#include "../Progress/Bounded.h"
#include "../PredStats.h"
#include "../RunSet.h"

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

    classify_runs();

    ofstream ffile("f.m");
    ofstream sfile("s.m");
     
    for(unsigned int i = NumRuns::begin; i < NumRuns::end; i++) {
        if(is_frun[i]) ffile << i - NumRuns::begin + 1 << "\n";   
        if(is_srun[i]) sfile << i - NumRuns::begin + 1 << "\n"; 
    }
    ffile.close();
    sfile.close();

}
