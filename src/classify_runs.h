#ifndef CLASSIFY_RUNS_H
#define CLASSIFY_RUNS_H

#include <argp.h>


extern int num_runs, num_sruns, num_fruns;
extern bool *is_srun, *is_frun;

extern const argp classify_runs_argp;

extern void classify_runs(const char [], const char []);


#endif // !CLASSIFY_RUNS_H
