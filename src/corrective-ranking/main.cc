#include <cmath>
#include <iostream>

#include "../Confidence.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../RunsDirectory.h"
#include "../Stylesheet.h"
#include "../ViewPrinter.h"

#include "Predicates.h"
#include "allFailures.h"
#include "zoom.h"

using namespace std;


////////////////////////////////////////////////////////////////////////
//
//  The main iterative ranking / selection / dilution loop
//


static void
buildView(Predicates &candidates, const char projection[])
{
  // create XML output file and write initial header
  ViewPrinter view(Stylesheet::filename, "corrected-view", "all", "hl", projection);

  Progress::Bounded progress("ranking predicates", candidates.count);

  // pluck out predicates one by one, printing as we go
  while (!candidates.empty())
    {
      const Predicates::iterator winner = max_element(candidates.begin(), candidates.end());
      view << *winner;;

      // cerr << "winner " << winner->index << " dilutes all failures\n";
      allFailures.dilute(*winner, winner->tru.failures);
      if (allFailures.count <= 0)
	break;

      for (Predicates::iterator loser = candidates.begin(); loser != candidates.end(); ++loser)
	if (loser != winner)
	  {
	    // cerr << "winner " << winner->index << " dilutes loser " << loser->index << '\n';
	    loser->dilute(*winner);
	  }

      progress.step();
      candidates.erase(winner);
      candidates.rescore(progress);
    }
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


const char *Stylesheet::filename = "corrected-view.xsl";

static bool zoomsWanted = false;


static int
parseFlag(int key, char *, argp_state *)
{
  using namespace Confidence;

  switch (key) {
  case 'z':
    zoomsWanted = true;
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


static void
processCommandLine(int argc, char *argv[])
{
  static const argp_option options[] = {
    {
      "zoom",
      'z',
      0,
      0,
      "generate zoom views in addition to main ranking",
      0
    },
    { 0, 0, 0, 0, 0, 0 }
  };

  static const argp_child children[] = {
    { &Confidence::argp, 0, 0, 0 },
    { &NumRuns::argp, 0, 0, 0 },
    { &RunsDirectory::argp, 0, 0, 0 },
    { &Stylesheet::argp, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };

  static const argp argp = {
    options, parseFlag, 0, 0, children, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
}


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


void
initialize(int argc, char *argv[])
{
  // command line processing and other initialization
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);
  // feenableexcept(FE_DIVBYZERO | FE_INVALID);
}


void
rankMain(const char projection[])
{
  ifstream failuresFile("f.runs");
  assert(failuresFile);
  failuresFile >> allFailures;

  Predicates candidates;
  candidates.load();

  if (zoomsWanted)
    buildZooms(candidates, projection);
  buildView(candidates, projection);
}
