#ifndef PARAM_H
#define PARAM_H

#include "preds.h"

#define NUM_SUCCESS_RUNS 16110 // data8.1 8036  
#define NUM_FAILURE_RUNS 3848  // data8.1 1928  
#define NUM_RUNS (NUM_SUCCESS_RUNS + NUM_FAILURE_RUNS)

#define NUM_2_PREDS (2 * NUM_2_SITES)
#define NUM_3_PREDS (3 * NUM_3_SITES)
#define NUM_PREDS (NUM_2_PREDS + NUM_3_PREDS)

/*********** input files ************/
#define SUCCESS_FILE "success_data9_20000.txt"
#define FAILURE_FILE "failure_data9_20000.txt"
#define DATA_DIR     "/u/mhn/projects/cbi/src/data9"

/*********** output files ************/
#define PHASE1_CAUSES_FILE "phase1_causes.htm"
#define PHASE2_CAUSES_FILE "phase2_causes.htm"

#define MAX_PRINT_COUNT 100

#endif
