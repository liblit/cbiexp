#include <argp.h>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include "../RunsDirectory.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
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

    /**************************************************************************
    * Read in the normalized mutual info between a run and all other runs
    * Store RunSets if the normalized mutual info is greater than threshold 
    **************************************************************************/
    vector < FRunSet * > sets; 
    ifstream run_MI("run_run_MI_normalized.txt");
    const unsigned int numRuns = FailureUniverse::getUniverse().cardinality(); 
    Progress::Bounded reading("calculating runs sets", numRuns);
    for(unsigned int i = 0; i < numRuns; i++) {
        reading.step();
        string line;
        getline(run_MI, line);
        istringstream parse(line);
        vector <bool> current;
        transform(istream_iterator<double>(parse), istream_iterator<double>(), back_inserter(current), bind2nd(greater<double>(),0.9));
        assert(current.size() == numRuns);
        int cardinality = count(current.begin(), current.end(), true);
        assert(cardinality > 0);
        if(cardinality > 1) {
            FRunSet temp = FailureUniverse::getUniverse().makeFRunSet();
            vector <unsigned int> indices = FailureUniverse::getUniverse().getIndices();
            for(unsigned int j = 0; j < numRuns; j++) {
                if(current[j]) temp.set(indices[j]); 
            }
            sets.push_back(new FRunSet(temp));
        }
    }
    assert(run_MI.peek() == EOF);
    run_MI.close();

    /**************************************************************************
    * Having found these sets we need to coalesce them. If two sets have a 
    * non-empty intersection we replace them with a single set which is the
    * union of the intersecting sets.
    **************************************************************************/
    ofstream out("run_sets.txt");
    Progress::Bounded coalescing("coalescing sets", sets.size()); 
    list <FRunSet * > set_list(sets.begin(), sets.end()); 
    vector <FRunSet *> coalesced;
    FailureUniverse::getUniverse().coalesce(set_list, coalesced);
    for(unsigned int i = 0; i < coalesced.size(); i++) {
        out << *(coalesced[i]) << "\n";
    }
    out.close();

}
