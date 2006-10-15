#include <argp.h>
#include <math.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include "../RunsDirectory.h"
#include "../NumRuns.h"
#include "../Progress/Unbounded.h"
#include "../Bugs.h"
#include "../RunSet.h"
#include "../FailureUniverse.h"

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

class Entropy : public unary_function <RunSet *, double> {
public:
   Entropy(const FailureUniverse & univ) {
       this->univ = &univ;
   }

   double operator()(const RunSet * X) const {
       return univ->entropy(*X); 
   }

private:
    const FailureUniverse * univ;
};

class SignedMutualInformation : public binary_function <RunSet *, RunSet *, double> {
public:
   SignedMutualInformation(const FailureUniverse & univ) {
       this->univ = &univ;
   }

   double operator()(const RunSet * X, const RunSet * Y) const {
       return univ->signedMI(*X, *Y); 
   }

private:
    const FailureUniverse * univ;
};

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    const FailureUniverse univ;

    /***************************************************************************
    * Read bug ids
    ***************************************************************************/
    vector <int> * bugIds = (new Bugs())->bugIndex();

    vector <RunSet *> bug_runs;
    string line;
    istringstream parse;
    /***************************************************************************
    * We read the runs associated with each bug 
    ***************************************************************************/
    ifstream bug_runs_file("bug_runs.txt");
    for(unsigned int i = 0; i < bugIds->size(); i++) {
        RunSet* current = new RunSet();  
        getline(bug_runs_file, line);
        parse.clear();
        parse.str(line);
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
    getline(bug_runs_file, line);
    parse.clear();
    parse.str(line);
    parse >> *unknown_runs;
    bug_runs.push_back(unknown_runs);
    bug_runs_file.close();

    /****************************************************************************
    * We calculate the entropy for the runs
    ****************************************************************************/
    vector <double> bug_runs_entropy;
    transform(bug_runs.begin(), bug_runs.end(), back_inserter(bug_runs_entropy), Entropy(univ));

    /****************************************************************************
    * We read the run sets decided by our predicate sets. We print out the
    * mutual information between the ones and the others.
    ****************************************************************************/
    ifstream runs_file("pred_set_runs.txt");
    Progress::Unbounded progress("reading run sets");
    ofstream out("calc_oracle_MI.txt");
    while(runs_file.peek() != EOF) {
        progress.step();
        getline(runs_file, line);
        parse.clear();
        parse.str(line);
        RunSet current;
        parse >> current;
        for(unsigned int i = 0; i < bug_runs.size(); i++) {
            double signedMI = univ.signedMI(current, *bug_runs[i]);  
            double entropy = univ.entropy(current);
            entropy = entropy > bug_runs_entropy[i] ?  entropy : bug_runs_entropy[i]; 
            signedMI = signedMI == 0.0 ? 0.0 : signedMI/entropy;
            out << signedMI << "\t";
        }
        out << "\n";
    }
    out.close();
    runs_file.close();
}
