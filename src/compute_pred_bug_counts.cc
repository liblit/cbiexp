#include <argp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ext/hash_map>
#include <vector>
#include "fopen.h" 
#include "RunsDirectory.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "Bugs.h"
#include "RunSet.h"
#include "OutcomeRunSets.h"
#include "PredStats.h"

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

size_t getScore(RunSet & tru, const RunSet & bug) 
{
    return tru.intersectionSize(bug);
}

hash_map <int, size_t> * getScores(RunSet & tru, hash_map <int, RunSet *> & bugs) 
{
    hash_map<int, size_t> * scores = new hash_map<int, size_t> (bugs.size());
    for (hash_map<int, RunSet *>::iterator i = bugs.begin(); i != bugs.end(); ++i) {
        (*scores)[i->first] = getScore(tru, *(i->second));
    }
    return scores;
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

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
    * For each predicate we find the number of runs corresponding to any 
    * particular bug or to an unknown bug in which the predicate has been 
    * observed true and print out the table of this data.
    **************************************************************************/
    ifstream tru("tru.txt");

    const unsigned numPreds = PredStats::count();
    Progress::Bounded progress("assigning predicates to runs", numPreds);
    FILE* out = fopenWrite("pred_bug_counts.txt"); 
    for (unsigned int count = 0; count < numPreds; ++count) { 
        progress.step();
        OutcomeRunSets current;
        tru >> current; 

        /*********************************************************************
        * Print predicate/count table
        *********************************************************************/
        hash_map <int, size_t> * bug_scores = getScores(current.failure, bug_run_map); 
        size_t unknown_score = getScore(current.failure, unknown_runs);
        for(unsigned int i = 0; i < bugIds->size(); i++) {
            size_t score = (*bug_scores)[(*bugIds)[i]]; 
            fprintf(out, "%u\t", score);
        }
        fprintf(out, "%u\t", unknown_score);
        fprintf(out, "\n");
    }
    fclose(out); 
    tru.close();

}
