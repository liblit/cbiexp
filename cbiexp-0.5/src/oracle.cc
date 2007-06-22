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
#include "RunSet.h"
#include "Progress/Unbounded.h"
#include "RunsDirectory.h"

using namespace std;
using __gnu_cxx::hash_map;

/******************************************************************************
* Attempts to assign a bug id to every failing run.
******************************************************************************/

#ifdef HAVE_ARGP_H

static void process_cmdline(int argc, char** argv)
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

#endif // !HAVE_ARGP_H

class UnknownBugException
{
};

/*******************************************************************************
* This procedure must be called only in the case where the run has already been
* identified as a failing run. It throws an UnknownBugException in the case
* where the run is not explained by the string in stderr.
* It assumes that the bug identification message has been written on the first 
* line of the file and with no preceding characters. 
*******************************************************************************/
int getBug(FILE* fp) {
    char description[100];
    char number[2];
    int result = fscanf(fp, "Error %s: %s\n", number, description);
    if(result != 1) throw UnknownBugException(); 
    return atoi(number);
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    ifstream failures(ClassifyRuns::failuresFilename); 
    if (!failures) {
        const int code = errno;
        cerr << "cannot read " << ClassifyRuns::failuresFilename << ": " << strerror(code) << '\n';
        exit(code || 1);
    }

    ofstream oraclefile("bugs.sparse");

    Progress::Unbounded progress("attributing failed runs");

    /***************************************************************************
    * For each failing run we determine what bug caused the run or if it's 
    * unexplained.
    ***************************************************************************/
    unsigned runId;
    while (failures >> runId) {
        progress.step();
        const string filename = RunsDirectory::format(runId, "bad/stderr");
        FILE* fp = fopenRead(filename);
	try {
            int bugId = getBug(fp);
            if (bugId == 0) throw UnknownBugException();
	    oraclefile << bugId << " " << runId + 1 << " 1\n"; 
        }
	catch(UnknownBugException & e) {
            cerr << "Bug at " << runId << " is unknown.\n";
	}
        fclose(fp);
    }
    failures.close();
    oraclefile.close();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
