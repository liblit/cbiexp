/******************************************************************************
* Writes a matlab struct into a .m file.
******************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <argp.h>
#include "../RunsDirectory.h"
#include "../NumRuns.h"

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

    ofstream outfile("indexing.m");
    outfile << "function Indexing = indexing()\n";
    outfile << "\tIndexing.offset = " << NumRuns::begin << ";\n";
    outfile.close();

}
