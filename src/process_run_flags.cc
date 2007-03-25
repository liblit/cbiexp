#include <iostream>
#include <fstream>
#include <string>
#include <ext/hash_map>
#include "arguments.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
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

class ind_hash_t : public hash_map<char, unsigned int>
{
};

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    /* get a map of string indices */
    ind_hash_t flag_indices;
    char flag;
    unsigned int counter = 1;
    ifstream flags_file("flags.txt");
    while(flags_file >>flag) {
        flag_indices[flag] = counter++;
    }
    flags_file.close();

    ofstream flags_dim("flags.dimensions");
    flags_dim << counter - 1 << " " << 1; 
    flags_dim.close();

    ofstream aspect_file("flags.oal");
    ofstream matlab_file("flags.sparse");
    Progress::Bounded progress("scanning flags", NumRuns::count());

    for (unsigned i = NumRuns::begin; i < NumRuns::end; i++) {
	progress.step();
	aspect_file << i << ":";

	const string filename = RunsDirectory::format(i, "flags");
	ifstream in(filename.c_str());
	char flag;
	while(in >> flag) {
	    aspect_file << flag << ";";      

            ind_hash_t::iterator found = flag_indices.find(flag);
            if( found == flag_indices.end() ) {
                const int code = errno;
                cerr << "no index found for flag " << flag << ": " << strerror(code) << "\n";
                exit(code || 1);
            }
            matlab_file << found->second << " " << i + 1 << " " << 1 << "\n";
	}
	aspect_file << "\n";
	in.close();

    }

    aspect_file.close();
    matlab_file.close();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
