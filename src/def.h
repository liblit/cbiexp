#ifndef DEF_H
#define DEF_H

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


// Local variables:
// c-file-style: "cc-mode"
// End:

#endif
