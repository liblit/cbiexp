#ifndef PARAM_H
#define PARAM_H

#include "preds.h"

#define NUM_SUCCESS_RUNS 8036  // 8154
#define NUM_FAILURE_RUNS 1928  // 1814
#define NUM_RUNS (NUM_SUCCESS_RUNS + NUM_FAILURE_RUNS)

#define NUM_2_PREDS (2 * NUM_2_SITES)
#define NUM_3_PREDS (3 * NUM_3_SITES)
#define NUM_PREDS (NUM_2_PREDS + NUM_3_PREDS)

/*********** input files ************/
#define SUCCESS_FILE "success_data8.1_10000.txt"
#define FAILURE_FILE "failure_data8.1_10000.txt"
#define PATH_PREFIX  "/u/mhn/projects/cbi/src/data8.1"

/*********** output files ************/
#define PHASE1_CAUSES_FILE "phase1_causes.txt"
#define PHASE2_CAUSES_FILE "phase2_causes.txt"
#define STATS_FILE  "stats.txt"

#endif
