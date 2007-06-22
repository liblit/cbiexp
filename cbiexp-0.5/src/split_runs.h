#ifndef INCLUDE_SPLIT_RUNS_H
#define INCLUDE_SPLIT_RUNS_H

#include <vector>

extern unsigned num_trainruns, num_valruns;
extern std::vector<bool> is_trainrun, is_valrun;
extern std::vector<unsigned> train_runs, val_runs;

extern void split_runs();

#endif // !INCLUDE_SPLIT_RUNS_H
