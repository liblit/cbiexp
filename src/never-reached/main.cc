#include <argp.h>
#include <fstream>
#include <iostream>
#include "../ClassifyRuns.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../RunsDirectory.h"
#include "../StaticSiteInfo.h"
#include "Locations.h"
#include "Reader.h"

using namespace std;


static void
processCommandLine(int argc, char *argv[])
{
  static const argp_child children[] = {
    { &NumRuns::argp, 0, 0, 0 },
    { &RunsDirectory::argp, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };

  static const argp argp = {
    0, 0, 0, 0, children, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
}


int main(int argc, char *argv[])
{
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  // start out optimistic: assume every site's location is unreached
  Locations unreached;
  StaticSiteInfo staticSiteInfo;
  unreached.insert(staticSiteInfo.sitesBegin(), staticSiteInfo.sitesEnd());

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
    Reader reader(unreached, staticSiteInfo);

    // knock any location observed in sparse report from failed runs
    while (runs >> runId && runId < NumRuns::end)
      if (runId >= NumRuns::begin)
	{
	  progress.stepTo(runId);
	  reader.read(runId);
	}
  }

  unreached.dump(std::cout);
  return 0;
}
