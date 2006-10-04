#include <argp.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ext/hash_map>
#include <vector>
#include "fopen.h" 
#include "classify_runs.h"
#include "RunsDirectory.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "Bugs.h"
#include "RunSet.h"
#include "OutcomeRunSets.h"
#include "PredStats.h"
#include "FailureUniverse.h"

using namespace std;
using __gnu_cxx::hash_map;

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
    fprintf(stderr, "%g\t%g\t%g\n", joint, margX, margY); 
    return joint == 0.0 ? 0.0 :
                    joint * log2(joint / (margX * margY));
}

/*******************************************************************************
* Here we compute the mutual information between two binary values.
* The joint probability distribution thus has only four possibilities.
*******************************************************************************/
double
MI(FailureUniverse & univ, RunSet & X, RunSet & Y)
{
    double Xtrue_Ytrue = univ.p_Xtrue_Ytrue(X,Y);
    double Xtrue_Yfalse = univ.p_Xtrue_Yfalse(X,Y);
    double Xfalse_Ytrue = univ.p_Xtrue_Yfalse(Y,X);
    double Xfalse_Yfalse = univ.p_Xfalse_Yfalse(X,Y);
    double Xtrue = univ.p_Xtrue(X);
    double Xfalse = univ.p_Xfalse(X);
    double Ytrue = univ.p_Xtrue(Y);
    double Yfalse = univ.p_Xfalse(Y);

    double true_true = term(Xtrue_Ytrue, Xtrue, Ytrue);
        fprintf(stderr, "%g\n", true_true);
    double true_false = term(Xtrue_Yfalse, Xtrue, Yfalse);
        fprintf(stderr, "%g\n", true_false);
    double false_true = term(Xfalse_Ytrue, Xfalse, Ytrue);
        fprintf(stderr, "%g\n", false_true);
    double false_false = term(Xfalse_Yfalse, Xfalse, Yfalse);
        fprintf(stderr, "%g\n", false_false);
    fprintf(stderr, "Done one calc\n");

    return term(Xtrue_Ytrue, Xtrue, Ytrue) +
           term(Xtrue_Yfalse, Xtrue, Yfalse) +
           term(Xfalse_Ytrue, Xfalse, Ytrue) +
           term(Xfalse_Yfalse, Xfalse, Yfalse);
}

hash_map <int, double> * getMIs(FailureUniverse & univ, RunSet & tru, hash_map <int, RunSet *> & bugs) 
{
    hash_map<int, double> * mis = new hash_map<int, double> (bugs.size());
    for (hash_map<int, RunSet *>::iterator i = bugs.begin(); i != bugs.end(); ++i) {
        double mi = MI(univ, tru, *(i->second));
        (*mis)[i->first] = mi; 
    }
    return mis;
}

double
signedMI(FailureUniverse & univ, RunSet & X, RunSet & Y)
{
    double cov = univ.covariance(X, Y);
    double mi = MI(univ, X, Y); 
    return mi * (cov >= 0 ? 1.0 : -1.0);
}

/********************************************************************************
* The value calculated for each RunSet pair is the mutual information times the
* _sign_ of the correlation coefficient. Really, it's the sign of the
* covariance, since the sign of the correlation coefficient is the same.
********************************************************************************/
hash_map <int, double> * getsignedMIs(FailureUniverse & univ, RunSet & tru, hash_map <int, RunSet *> & bugs) 
{
    hash_map<int, double> * mis = new hash_map<int, double> (bugs.size());
    for (hash_map<int, RunSet *>::iterator i = bugs.begin(); i != bugs.end(); ++i) {
        double mi = signedMI(univ, tru, *(i->second));
        (*mis)[i->first] = mi; 
    }
    return mis;
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    FailureUniverse univ;

    /***************************************************************************
    * Read bug ids
    ***************************************************************************/
    vector <int> * bugIds = (new Bugs())->bugIndex();

    /***************************************************************************
    * build map from bug ids to runs. We expect the runs to be ordered. 
    ***************************************************************************/
    hash_map <int, RunSet *> bug_run_map;

    /***************************************************************************
    * Some runs may be unknown
    ***************************************************************************/
    RunSet unknown_runs;

    /***************************************************************************
    * We read the runs associated with each bug 
    ***************************************************************************/
    ifstream bug_runs("bug_runs.txt");
    for(unsigned int i = 0; i < bugIds->size(); i++) {
        RunSet* current = new RunSet();  
        string line;
        getline(bug_runs, line);
        istringstream parse(line);
        parse >> *current;
        bug_run_map[(*bugIds)[i]] = current;
    }
    bug_runs.close();

    /**************************************************************************
    * We read the runs that haven't been assigned to any bug.
    **************************************************************************/
    bug_runs.open("unknown_runs.txt");
    string line;
    getline(bug_runs, line);
    istringstream parse(line);
    parse >> unknown_runs;
    bug_runs.close();


    /**************************************************************************
    * Calculate the mutual information between predicate and bug.
    **************************************************************************/
    ifstream tru("tru.txt");

    const unsigned numPreds = PredStats::count();
    Progress::Bounded progress("assigning predicates to runs", numPreds);
    FILE* out = fopenWrite("pred_bug_MI.txt"); 
    for (unsigned int count = 0; count < numPreds; ++count) { 
        progress.step();
        OutcomeRunSets current;
        tru >> current; 

        /*********************************************************************
        * Print predicate/count table
        *********************************************************************/
        hash_map <int, double> * bug_MIs = getsignedMIs(univ, current.failure, bug_run_map); 
        double unknown_MI = signedMI(univ, current.failure, unknown_runs);
        for(unsigned int i = 0; i < bugIds->size(); i++) {
            double MI = (*bug_MIs)[(*bugIds)[i]]; 
            fprintf(out, "%g\t", MI);
        }
        fprintf(out, "%g\t", unknown_MI);
        fprintf(out, "\n");
    }
    fclose(out); 
    tru.close();

}
