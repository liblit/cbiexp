#ifndef PARAM_H
#define PARAM_H

#define NUM_SUCCESS_RUNS 8036  
#define NUM_FAILURE_RUNS 1928 
#define NUM_RUNS (NUM_SUCCESS_RUNS + NUM_FAILURE_RUNS)

#define NUM_2_SITES 2145
#define NUM_3_SITES 7896
#define NUM_2_PREDS (2 * NUM_2_SITES)
#define NUM_3_PREDS (3 * NUM_3_SITES)
#define NUM_PREDS (NUM_2_PREDS + NUM_3_PREDS)

/*********** input files ************/
#define SUCCESS_FILE "success_data8.1_10000.txt"
#define FAILURE_FILE "failure_data8.1_10000.txt"
#define PATH_PREFIX  "data8.1"
#define SITES_2_FILE "sites2_data8.1.txt"
#define SITES_3_FILE "sites3_data8.1.txt"

/*********** output files ************/
#define CAUSES_FILE "causes.txt"
#define STATS_FILE  "stats.txt"

#endif
