#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ext/hash_map>
#include "arguments.h"
#include "fopen.h"
#include "Bugs.h"
#include "ClassifyRuns.h"
#include "NumRuns.h"
#include "Progress/Unbounded.h"
#include "RunsDirectory.h"

using namespace std;
using __gnu_cxx::hash_map;


#ifdef HAVE_ARGP_H

static void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}

#else // !HAVE_ARGP_H

inline void process_cmdline(int, char *[]) { }

#endif // !HAVE_ARGP_H

class UnknownBugException
{
};

/*********************************************************************************
* This procedure must be called only in the case where the run has already been
* identified as a failing run. It throws an UnknownBugException in the case
* where the run is not explained by the string in stderr.
* It assumes that the bug identification message has been written on the first 
* line of the file and with no preceding characters. 
*********************************************************************************/
int getBug(FILE* fp) {
    char s[100];
    int result = fscanf(fp, "About to trigger bug #%s\n", s);
    if(result != 1) throw UnknownBugException(); 
    return atoi(s);  
}

/*********************************************************************************
* Writes out run ids; each id is followed by a newline.
*********************************************************************************/
void write_unknown_runs(FILE* fp, const vector <unsigned> * runs) {
    for (vector<unsigned>::const_iterator i = runs->begin(); i != runs->end(); ++i) {
     	fprintf(fp, "%u\n", *i); 
    }
  
}

/*********************************************************************************
* Writes out run ids; each id is followed by a tab.
*********************************************************************************/
void write_run_set(FILE* fp, const vector <unsigned> * runs)
{
    for (vector<unsigned>::const_iterator i = runs->begin(); i != runs->end(); ++i) {
     	fprintf(fp, "%u\t", *i); 
    }
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    vector <int> * bugIds = (new Bugs())->bugIndex(); 

    /*****************************************************************************
    * Build map from bug ids to run ids
    *****************************************************************************/
    hash_map <int, vector <unsigned> *> bug_run_map;
    for(unsigned int i = 0; i < bugIds->size (); i++) {
      bug_run_map[(*bugIds)[i]] = new vector <unsigned> ();
    }

    /************************************************************************ 
     * We prefer to record failing runs that have gone unattributed to any
     * particular bug, rather than throw them away.
     ***********************************************************************/
    vector <unsigned> * unknown_runs = new vector <unsigned> ();


    ifstream failures(ClassifyRuns::failuresFilename); 
    if (!failures) {
        const int code = errno;
        cerr << "cannot read " << ClassifyRuns::failuresFilename << ": " << strerror(code) << '\n';
        exit(code || 1);
    }

    Progress::Unbounded progress("attributing failed runs");

    /*****************************************************************************
    * For each failing run we determine what bug caused the run or if it's 
    * unexplained.
    *****************************************************************************/
    unsigned runId;
    while (failures >> runId) {
        progress.step();
        const string filename = RunsDirectory::format(runId, "stderr");
        FILE* fp = fopenRead(filename);
	try {
            int bugId = getBug(fp);
	    bug_run_map[bugId]->push_back(runId);
        }
	catch(UnknownBugException & e) {
	    unknown_runs->push_back(runId);
	}
        fclose(fp);
    }
    failures.close();

    /****************************************************************************
    * We write out the runs associated with each bug in the order in which the 
    * bugs were read in.
    ****************************************************************************/
    FILE* bug_runs = fopenWrite("bug_runs.txt");
    for (unsigned int i = 0; i < bugIds->size(); i++) {
        write_run_set(bug_runs, bug_run_map[(*bugIds)[i]]);
        fprintf(bug_runs, "\n");
    }
    fclose(bug_runs);

    /****************************************************************************
    * We write out any unexplained runs.
    ****************************************************************************/
    bug_runs = fopenWrite("unknown_runs.txt");
    write_unknown_runs(bug_runs, unknown_runs);
    fclose(bug_runs);

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
