#include <cassert>
#include <fstream>
#include "PredStats.h"
#include "Progress/Unbounded.h"

using namespace std;


const char PredStats::filename[] = "preds.txt";


unsigned
PredStats::count()
{
  static unsigned counted;

  if (!counted)
    {
      Progress::Unbounded progress("counting interesting predicates");

      ifstream in(filename);
      assert(in);

      while (true)
	switch (in.get())
	  {
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
