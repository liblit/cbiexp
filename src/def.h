#ifndef DEF_H
#define DEF_H

#define SRUNS_TXT_FILE "s.runs"
#define FRUNS_TXT_FILE "f.runs"
#define SITES_TXT_FILE "sites.txt"
#define UNITS_HDR_FILE "units.h"
#define PREDS_TXT_FILE "preds.txt"
#define PREDS_HDR_FILE "preds.h"
#define RESULT_SUMMARY_FILE "summary.html"

#define OBS_FILE "obs.txt"
#define TRU_FILE "tru.txt"

#define I_REPORT_PATH_FMT "data/%d/reports"
#define O_REPORT_PATH_FMT "data/%d/reports.new"
#define LABEL_PATH_FMT "data/%d/label"

#define ALL_CLUSTER_FILE "summary.txt"
#define PER_CLUSTER_FILE "cluster%d.txt"

#define NUM_SCHEMES 4
#define NUM_SORTBYS 4

static const char* scheme_codes[NUM_SCHEMES] = { "B", "R", "S", "preds" };
static const char* scheme_names[NUM_SCHEMES] = { "branch", "return", "scalar", "all" };
static const char* sortby_codes[NUM_SORTBYS] = { "lb", "is", "fs", "nf" };
static const char* sortby_names[NUM_SORTBYS] = { "lower bound of confidence interval",
                                                 "increase score",
                                                 "fail score", 
                                                 "true in # F runs" };
static const int sortby_indices[NUM_SORTBYS] = { 5, 6, 7, 10 };

#endif

