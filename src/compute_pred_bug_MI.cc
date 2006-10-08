#include <argp.h>
#include <math.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "RunsDirectory.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "Bugs.h"
#include "RunSet.h"
#include "OutcomeRunSets.h"
#include "PredStats.h"
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

double
log2(double val)
{
    return log(val)/M_LN2;
}

double
term(double joint, double margX, double margY)
{
    return joint == 0.0 ? 0.0 :
                    joint * log2(joint / (margX * margY));
}

/*******************************************************************************
* Here we compute the mutual information between two binary values.
* The joint probability distribution thus has only four possibilities.
*******************************************************************************/
double
MI(const FailureUniverse & univ, const RunSet & X, const RunSet & Y)
{
    double Xtrue_Ytrue = univ.p_Xtrue_Ytrue(X,Y);
    double Xtrue_Yfalse = univ.p_Xtrue_Yfalse(X,Y);
    double Xfalse_Ytrue = univ.p_Xtrue_Yfalse(Y,X);
    double Xfalse_Yfalse = univ.p_Xfalse_Yfalse(X,Y);
    double Xtrue = univ.p_Xtrue(X);
    double Xfalse = univ.p_Xfalse(X);
    double Ytrue = univ.p_Xtrue(Y);
    double Yfalse = univ.p_Xfalse(Y);

    return term(Xtrue_Ytrue, Xtrue, Ytrue) +
           term(Xtrue_Yfalse, Xtrue, Yfalse) +
           term(Xfalse_Ytrue, Xfalse, Ytrue) +
           term(Xfalse_Yfalse, Xfalse, Yfalse);
}

double
signedMI(const FailureUniverse & univ, const RunSet & X, const RunSet & Y)
{
    double cov = univ.covariance(X, Y);
    double mi = MI(univ, X, Y); 
    return mi * (cov >= 0 ? 1.0 : -1.0);
}

class SignedMutualInformation : public unary_function <RunSet *, double> {
public:
   SignedMutualInformation(const FailureUniverse * univ, const RunSet * X) {
       this->univ = univ;
       this->X = X;
   }

   double operator()(const RunSet * Y) const {
       return signedMI(*univ, *X, *Y); 
   }

private:
    const RunSet * X;
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
    /***************************************************************************
    * We read the runs associated with each bug 
    ***************************************************************************/
    ifstream bug_runs_file("bug_runs.txt");
    for(unsigned int i = 0; i < bugIds->size(); i++) {
        RunSet* current = new RunSet();  
        string line;
        getline(bug_runs_file, line);
        istringstream parse(line);
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
    string line;
    getline(bug_runs_file, line);
    istringstream parse(line);
    parse >> *unknown_runs;
    bug_runs.push_back(unknown_runs);
    bug_runs_file.close();

    /**************************************************************************
    * Calculate the mutual information between predicate and bug.
    **************************************************************************/
    ifstream tru("tru.txt");

    const unsigned numPreds = PredStats::count();
    Progress::Bounded progress("assigning predicates to runs", numPreds);
    ofstream out("pred_bug_MI.txt"); 
    for (unsigned int count = 0; count < numPreds; ++count) { 
        progress.step();
        OutcomeRunSets current;
        tru >> current; 

        /*********************************************************************
        * Print predicate/count table
        *********************************************************************/
        transform(bug_runs.begin(), bug_runs.end(), ostream_iterator<double> (out, "\t"), SignedMutualInformation(&univ, &current.failure));
        out << "\n";
    }
    assert(tru.peek() == EOF);
    out.close();
    tru.close();

}
