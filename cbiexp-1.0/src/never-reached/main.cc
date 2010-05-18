#include <argp.h>
#include <fstream>
#include <iostream>
#include "../DatabaseFile.h"
#include "../ClassifyRuns.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "Reader.h"

using namespace std;


static void
processCommandLine(int argc, char *argv[])
{
  static const argp_child children[] = {
    { &DatabaseFile::argp, 0, 0, 0 },
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
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  Reader reader(DatabaseFile::DatabaseName);

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
