#include <argp.h>
#include <fstream>
#include <iostream>
#include "../ClassifyRuns.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../termination.h"
#include "Reader.h"

using namespace std;


static void
processCommandLine(int argc, char *argv[])
{
  static const argp_child children[] = {
    { &NumRuns::argp, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };

  static const argp argp = {
    0, 0, 0, 0, children, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
}


int main(int argc, char *argv[])
{
  set_terminate_verbose();
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  Reader reader("counts.txt");

  {
    unsigned runId;
    ifstream runs(ClassifyRuns::failuresFilename);
    runs.exceptions(ios::badbit);
    if (!runs)
      {
	cerr << "cannot read " << ClassifyRuns::failuresFilename << '\n';
	return 1;
      }

    Progress::Bounded progress("scanning failed runs", NumRuns::count());

    // knock any location observed in sparse report from failed runs
    while (runs >> runId && runId < NumRuns::end())
      if (runId >= NumRuns::begin())
	{
	  progress.stepTo(runId);
	  reader.read(runId);
	}
  }

  reader.dump();
  return 0;
}
