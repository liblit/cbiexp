#include <iostream>
#include <string>
#include "arguments.h"
#include "fopen.h"
#include "ClassifyRuns.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "RunsDirectory.h"

using namespace std;


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


int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    FILE* sfp = fopenWrite(ClassifyRuns::successesFilename);
    FILE* ffp = fopenWrite(ClassifyRuns::failuresFilename);

    Progress::Bounded progress("scanning labels", NumRuns::count());

    for (unsigned i = NumRuns::begin; i < NumRuns::end; i++) {
	progress.step();

	char s[100];
	const string filename = RunsDirectory::format(i, "label");
	FILE* fp = fopenRead(filename);
	fscanf(fp, "%s", s);
	fclose(fp);

	if (!strcmp(s, "success")) {
	    fprintf(sfp, "%u\n", i);
            continue;
        }
	if (!strcmp(s, "failure")) {
	    fprintf(ffp, "%u\n", i);
            continue;
        }
	if (strcmp(s, "ignore")) {
	    cerr << "malformed label in " << filename << ": " << s << '\n';
	    return 1;
	}
    }

    fclose(sfp);
    fclose(ffp);
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
