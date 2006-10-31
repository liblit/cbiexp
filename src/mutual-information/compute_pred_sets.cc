#include <argp.h>
#include <cassert>
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
#include "../Progress/Bounded.h"
#include "RunType.h"
#include "PredUniverse.h"
#include "../PredStats.h"

using namespace std;

#ifdef HAVE_ARGP_H

static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
        { &RunsDirectory::argp, 0, 0, 0 }, 
        { &RunType::argp, 0, 0, 0 },
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
    string suffix = RunType::type == RunType::FAILING_RUNS ? "failing-runs" :
                    RunType::type == RunType::SUCCEEDING_RUNS ?
                    "succeeding-runs" : ""; 
    assert(!suffix.empty());
    string sets_file_name = "pred_sets_" + suffix + ".txt";
    string measure_file_name = "pred_pred_MI_normalized_" + suffix + ".txt";

    /**************************************************************************
    * Read in the normalized mutual information between a pred and all other
    * preds. The normalized mutual information between a pred and itself is 1.
    * pred other than itself then we have found a set of closely associated 
    * predicates that is worth keeping. 
    **************************************************************************/
    vector < PSet * > sets; 
    const unsigned numPreds = PredStats::count();
    ifstream pred_MI(measure_file_name.c_str());
    Progress::Bounded reading("calculating predicate sets", numPreds);
    for(unsigned int i = 0; i < numPreds; i++) {
        reading.step();
        string line;
        getline(pred_MI, line);
        istringstream parse(line);
        vector <double> current;
        copy(istream_iterator<double>(parse), istream_iterator<double>(), back_inserter(current));
        assert(current.size() == numPreds);
        PSet temp = PredUniverse::getUniverse().makePredSet();
        for(unsigned int j = 0; j < current.size(); j++) {
            if(current[j] > 0.8) temp.set(j);
        }
        assert(temp.count() > 0);
        sets.push_back(new PSet(temp));
    }
    assert(pred_MI.peek() == EOF);
    pred_MI.close();

    /**************************************************************************
    * Having found these sets we need to coalesce them. If two sets have a 
    * non-empty intersection we replace them with a single set which is the
    * union of the intersecting sets.
    **************************************************************************/
    ofstream out(sets_file_name.c_str());
    Progress::Bounded coalescing("coalescing sets", sets.size()); 
    list <PSet * > set_list(sets.begin(), sets.end()); 
    vector <PSet *> coalesced; 
    PredUniverse::getUniverse().coalesce(set_list, coalesced);
    for(unsigned int i = 0; i < coalesced.size(); i++) {
        out << *(coalesced[i]) << "\n";
    }
    out.close();

}
