#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include "classify_runs.h"

// global information provided by classify_runs()
int num_runs = 0, num_sruns, num_fruns;
bool *is_srun, *is_frun;


////////////////////////////////////////////////////////////////////////
//
// Command line processing
//


static const argp_option options[] = {
    {
	"success-list",
	's',
	"s.runs",
	0,
	"file listing successful runs",
	0
    },
    {
	"failure-list",
	'f',
	"f.runs",
	0,
	"file listing failed runs",
	0
    },
    { 0, 0, 0, 0, 0, 0 }
};


static int
parse_flag(int key, char *arg, argp_state *)
{
    static const char *sruns_txt_file = "s.runs";
    static const char *fruns_txt_file = "f.runs";

    switch (key) {
    case 's':
	sruns_txt_file = arg;
	return 0;
    case 'f':
	fruns_txt_file = arg;
	return 0;
    case ARGP_KEY_SUCCESS:
	classify_runs(sruns_txt_file, fruns_txt_file);
	return 0;
    default:
	return ARGP_ERR_UNKNOWN;
    }
}


const argp classify_runs_argp = {
    options, parse_flag, 0, 0, 0, 0, 0
};


void classify_runs(const char sruns_file[], const char fruns_file[])
{
    int i;

    FILE* sfp = fopen(sruns_file, "r");
    assert(sfp);
    FILE* ffp = fopen(fruns_file, "r");
    assert(ffp);

    while (1) {
	fscanf(sfp, "%d", &i);
	if (feof(sfp))
	    break;
	if (i > num_runs)
	    num_runs = i;
    }
    rewind(sfp);

    while (1) {
	fscanf(ffp, "%d", &i);
	if (feof(ffp))
	    break;
	if (i > num_runs)
	    num_runs = i;
    }
    rewind(ffp);

    num_runs += 1;

    is_srun = new bool[num_runs];
    assert(is_srun);
    for (i = 0; i < num_runs; i++) is_srun[i] = false;
    is_frun = new bool[num_runs];
    assert(is_frun);
    for (i = 0; i < num_runs; i++) is_frun[i] = false;

    while (1) {
	fscanf(sfp, "%d", &i);
	if (feof(sfp))
	    break;
	is_srun[i] = 1;
	num_sruns++;
    }

    while (1) {
	fscanf(ffp, "%d", &i);
	if (feof(ffp))
	    break;
	if (is_srun[i]) {
	    printf("Run %d is both successful and failing\n", i);
	    exit(1);
	}
	is_frun[i] = 1;
	num_fruns++;
    }

    fclose(sfp);
    fclose(ffp);
}


// Local variables:
// c-file-style: "cc-mode"
// End:
