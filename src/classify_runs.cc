#include <fstream>
#include <iostream>
#include "ClassifyRuns.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "classify_runs.h"
#include "fopen.h"

using namespace std;


// global information provided by classify_runs()
unsigned num_sruns, num_fruns;
vector<bool> is_srun, is_frun;


static void
read_runs(const char message[], const char filename[], vector<bool> &bits, unsigned &count)
{
    ifstream file(filename);
    if (!file) {
	const int code = errno;
	cerr << "cannot read " << filename << ": " << strerror(code) << '\n';
	exit(code || 1);
    }

    const unsigned numRuns = NumRuns::end;
    bits.resize(numRuns);
    Progress::Bounded progress(message, numRuns);

    unsigned runId;
    while (file >> runId && runId < numRuns) {
	progress.stepTo(runId);
	bits[runId] = 1;
	++count;
    }
}


void classify_runs()
{
    read_runs("reading success list", ClassifyRuns::successesFilename, is_srun, num_sruns);
    read_runs("reading failure list", ClassifyRuns:: failuresFilename, is_frun, num_fruns);

    for (unsigned runId = 0; runId < NumRuns::end; ++runId)
	if (is_srun[runId] && is_frun[runId]) {
	    cerr << "Run " << runId << " is both successful and failing\n";
	    exit(1);
	}
}


// Local variables:
// c-file-style: "cc-mode"
// End:
