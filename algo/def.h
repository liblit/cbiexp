#ifndef DEF_H
#define DEF_H

// Set this!
#define NUM_RUNS    31999  // should be N + 1 where
                           // N = max. number in s.runs or f.runs
                           // Does not matter whether experiment begins
                           // runs from 0 or from 1.  If it begins from
                           // 1, don't specify 0 in either s.runs or f.runs

// Set these!
#define I_SITES_FILE  "/moa/sc2/cbi/rhythmbox/share/rhythmbox.sites"
#define I_REPORT_FILE "/moa/sc1/cbi/rb_ds100/%d.txt"
#define O_REPORT_FILE "/moa/sc4/mhn/rb/base_ds100/%d.txt"
#define O_PER_PRED_HTML_FILE "/moa/sc3/mhn/rb/base_ds100/r/%d_%d_%d.html"
#define O_B_HTML_FILE "/moa/sc3/mhn/rb/base_ds100/B.html"
#define O_R_HTML_FILE "/moa/sc3/mhn/rb/base_ds100/R.html"
#define O_S_HTML_FILE "/moa/sc3/mhn/rb/base_ds100/S.html"
#define O_U_HTML_FILE "/moa/sc3/mhn/rb/base_ds100/U.html"

// Need not set these
#define O_UNITS_FILE  "units.h"
#define O_SITES_FILE  "sites.txt"

struct unit_t {
    char* s;  // unit name
    int c;    // #sites in unit
};

#endif
