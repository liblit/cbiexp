#include <argp.h>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include "../Progress/Bounded.h"
#include "../Progress/Unbounded.h"
#include "PredUniverse.h"
#include "util.h"
#include "../PredStats.h"

using namespace std;

#ifdef HAVE_ARGP_H

static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
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
    * Read in each pred sets associated with failure 
    **************************************************************************/
    vector < PSet * > fsets; 
    ifstream pred_sets;
    pred_sets.open("pred_sets_failing-runs.txt");
    Progress::Unbounded reading("reading predicate sets");
    while(pred_sets.peek() != EOF) {
        reading.step();
        string line;
        getline(pred_sets, line);
        istringstream parse(line);
        PSet current = PredUniverse::getUniverse().makePredSet(); 
        parse >> current;
        fsets.push_back(new PSet(current));
    }
    pred_sets.close();

    /**************************************************************************
    * Read in each pred sets associated with success 
    **************************************************************************/
    vector < PSet * > ssets; 
    pred_sets.clear();
    pred_sets.open("pred_sets_succeeding-runs.txt");
    Progress::Unbounded reading1("reading predicate sets");
    while(pred_sets.peek() != EOF) {
        reading1.step();
        string line;
        getline(pred_sets, line);
        istringstream parse(line);
        PSet current = PredUniverse::getUniverse().makePredSet(); 
        parse >> current;
        ssets.push_back(new PSet(current));
    }
    pred_sets.close();

    /**************************************************************************
    * For each failing run set calculate entropy
    **************************************************************************/
    vector <double> entropy_fruns;
    PredUniverse::getUniverse().entropy(fsets, entropy_fruns); 
    ofstream out("pred_sets_entropy_failing-runs.txt");
    copy(entropy_fruns.begin(), entropy_fruns.end(), ostream_iterator<double>(out, "\n"));
    out.close();

    /**************************************************************************
    * For each succeeding run set calculate entropy
    **************************************************************************/
    vector <double> entropy_sruns;
    PredUniverse::getUniverse().entropy(ssets, entropy_sruns); 
    out.open("pred_sets_entropy_succeeding-runs.txt");
    copy(entropy_sruns.begin(), entropy_sruns.end(), ostream_iterator<double>(out, "\n"));
    out.close();

    /**************************************************************************
    * For each failing run set calculate dependencies between it and succeeding
    * run sets
    ***************************************************************************/
    out.open("pred_set_MI.txt");
    ofstream nout("pred_set_MI_normalized.txt");
    ofstream rout("pred_set_redundancy.txt");
    Progress::Bounded progress("calculating dependencies", fsets.size());
    for(unsigned int i = 0; i < fsets.size(); i++) {
        progress.step();
        vector <double> mi;
        PredUniverse::getUniverse().signedMI(*fsets[i], ssets, mi);
        copy(mi.begin(), mi.end(), ostream_iterator<double>(out, "\t"));
        out << "\n";
        for(unsigned int j = 0; j < ssets.size(); j++) {
            nout << (normalizemin(entropy_fruns[i],
                               entropy_sruns[j],
                               mi[j])) << "\t";
            rout << (redundify(entropy_fruns[i],
                               entropy_sruns[j],
                               mi[j])) << "\t";
        }
        nout << "\n";
        rout << "\n";
    }
    out.close();
    nout.close();
    rout.close();

}
