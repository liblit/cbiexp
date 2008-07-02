#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include "RunsSplit.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "split_runs.h"
#include "fopen.h"

using namespace std;

// global information provided by split_runs.h
unsigned num_trainruns = 0;
unsigned num_valruns = 0;
vector<bool> is_trainrun, is_valrun;
vector<unsigned> train_runs, val_runs;

static void
read_split_runs(const char message[], const char filename[], vector<bool> &bits, 
		vector<unsigned> &runs, unsigned &count)
{
  ifstream file(filename);
  if (!file) {
    const int code = errno;
    cerr << "Cannot read "<< filename << ": " << strerror(code) << '\n';
    exit(code || 1);
  }

  const unsigned num_runs = NumRuns::count();
  bits.resize(num_runs);
  Progress::Bounded progress(message, num_runs);

  unsigned i;
  while (file >> i && i < num_runs) {
    progress.stepTo(i);
    bits[i] = true;
    ++count;
  }
  file.close();

  runs.resize(count);
  unsigned c = 0;
  for (i = 0; i < num_runs; i++) {
    if (bits[i] == true) {
      runs[c] = i;
      c++;
    }
  }
  assert(c == count);
}

void split_runs()
{
  read_split_runs("reading training list", RunsSplit::trainFilename, is_trainrun, train_runs, num_trainruns);
  read_split_runs("reading val list", RunsSplit::valFilename, is_valrun, val_runs, num_valruns);

  const unsigned num_runs = NumRuns::count();
  for (unsigned i = 0; i < num_runs; i++) {
    if(is_trainrun[i] && is_valrun[i]) {
      cerr << "Run " << i << " is in both the training set and the validation set.\n";
      exit(1);
    }
  }
}
