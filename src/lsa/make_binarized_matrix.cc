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

    classify_runs();

    ifstream tru("tru.txt");
    ofstream matrix("X.sparse"); 

    const unsigned numPreds = PredStats::count();
    Progress::Bounded reading("reading predicate info", numPreds);
    for(unsigned int count = 0; count < numPreds; count++) {
        reading.step();
        OutcomeRunSets current;
        tru >> current;
        boost::dynamic_bitset<> combined = current.failure.value() | current.success.value();

        vector<bool> temp;
        /**********************************************************************
        * We set the vector bit to false or true appropriately unless its a 
        * run we don't count; either because it was unclassified or else
        * because it was a run that was used to train the sampler.
        * We don't want any of the columns of the matrix we read in to
        * correspond to runs we have chosen to ignore. 
        **********************************************************************/
        for(unsigned int index = NumRuns::begin; index < combined.size(); index++) {
            if(is_srun[index] || is_frun[index])
            temp.push_back(combined.test(index));
        }

        /**********************************************************************
        * We output the matrix _sparsely_, in matlab sparsely loaded input
        * format. 
        * Matlab column and row indices start at 1, not 0.
        **********************************************************************/
        for(unsigned int index = 0; index < temp.size(); index++) {
            if(temp.at(index)) 
                matrix << count + 1 << " " << index + 1 << " " << 1 << "\n"; 
        }
    }
    tru.close(); 
    matrix.close(); 

}
