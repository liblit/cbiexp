#include <cassert>
#include <fstream>
#include "Nonconst_PredStats.h"
#include "Progress/Unbounded.h"

using namespace std;

const char Nonconst_PredStats::filename[] = "nonconst_preds.txt";

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
