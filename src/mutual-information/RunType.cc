#include <string>
#include "RunType.h"

unsigned int RunType::type;

#ifdef HAVE_ARGP_H

static const argp_option options[] = {
    {
        "run-type",
        't',
        "TYPE",
        0,
        "\"failing-runs\" if calculating values for failing runs, \"succeeding-runs\" otherwise",
        0
    },
    { 0, 0, 0, 0, 0, 0 }
};

static int
parseFlag(int key, char *arg, argp_state *)
{
    using namespace RunType;

    switch(key)
    {
        case 't':
            if (strcmp(arg,"failing-runs") == 0) {
                RunType::type = FAILING_RUNS;
            } else if (strcmp(arg,"succeeding-runs") == 0) {
                RunType::type = SUCCEEDING_RUNS; 
            } else {
                 return ARGP_ERR_UNKNOWN;
            }
            return 0;
       default:
           return ARGP_ERR_UNKNOWN;
    }      
}

const argp RunType::argp = {
    options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
