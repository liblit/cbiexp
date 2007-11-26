#include <cassert>
#include <fstream>
#include "Nonconst_PredStats.h"
#include "Progress/Unbounded.h"

using namespace std;

const char * Nonconst_PredStats::filename = "nonconst_preds.txt";

unsigned
Nonconst_PredStats::count()
{
  static unsigned counted;

  if (!counted) {
    Progress::Unbounded progress("counting non-constant predicates");

    ifstream in(filename);
    assert(in);

    while (true)
      switch(in.get()) {
      case '\n':
	progress.step();
	++counted;
	break;
      case EOF:
	return counted;
      }
  }

  return counted;
}

static const argp_option options[] = {
  {
    "nonconst-file",
    'n',
    "NONCONST-FILE",
    0,
    "list of nonconstant predicates to be used",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};

static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key) {
  case 'n':
    Nonconst_PredStats::filename = arg;
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}

const argp Nonconst_PredStats::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

