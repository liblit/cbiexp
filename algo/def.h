#ifndef DEF_H
#define DEF_H

#define NUM_UNITS   234
#define NUM_S_SITES 133395
#define NUM_R_SITES 8429
#define NUM_B_SITES 3418
#define NUM_SITES   (NUM_S_SITES + NUM_R_SITES + NUM_B_SITES)
#define NUM_PREDS   (NUM_S_SITES * 6) + (NUM_R_SITES * 6) + (NUM_B_SITES * 2)
#define NUM_RUNS    25196

struct unit_t {
    char* s;  // unit name
    int c;    // #sites in unit
};

#endif
