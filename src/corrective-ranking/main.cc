#include <cmath>
#include <iostream>

#include "../Confidence.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../RunsDirectory.h"
#include "../Stylesheet.h"
#include "../ViewPrinter.h"
#include "../termination.h"

#include "Candidates.h"
#include "Conjunction.h"
#include "allFailures.h"
#include "zoom.h"

using namespace std;


const char *Stylesheet::filename = "corrected-view.xsl";

enum ZoomsWanted { ZoomNone, ZoomWinners, ZoomAll };
static ZoomsWanted zoomsWanted;
static string zoomAttr;


////////////////////////////////////////////////////////////////////////
//
//  The main iterative ranking / selection / dilution loop
//


static void
buildView(Candidates candidates, const char projection[], Foci *foci = 0)
{
  // Keeps track of the current best score.
  double bestScore;

  // Keeps a list of the original candidates, undiluted.  That way
  // the original score of any predictor conjunctions can be calculated.
  Candidates origCandidates;
  origCandidates.load();  //here's where we cheat a little.

  // create XML output file and write initial header
  ViewPrinter view(Stylesheet::filename, "corrected-view", "all", "hl", projection);

  Progress::Bounded progress("ranking predicates", candidates.count);

  if (allFailures.count <= 0)
    return;

  const AllFailuresSnapshot snapshot;

  cout << "Entering conjunction-enabled buildView.  This might take a while..." << endl;

  // pluck out predicates one by one, printing as we go
  while (!candidates.empty())
    {
      candidates.sort();
      candidates.reverse();
      const Candidates::iterator winner = max_element(candidates.begin(), candidates.end());

      //'winner' is the best Predicate.  We now try to find a conjunction with a better score...
      //if there is more than one such Conjunction, we keep only the best.
      bestScore = winner->score();
      std::list<Conjunction> bestConj = conjoin( candidates, 1 );
      
      // If the list has an item, and its score beats bestScore, it's what
      // we use.
      if ( bestConj.size() == 1 && bestConj.front().score() > bestScore ) {
	
	// We need to form a conjunction of the same predicates, with their
	// original RunSets, to get the original score of this one.
	
	std::vector<unsigned> preds = bestConj.front().getPredicateList();
	Candidates::iterator i = origCandidates.begin();
	Candidates::iterator j = origCandidates.begin();
	while ( i != origCandidates.end() && j != origCandidates.end() ) {
	  if ( i->index != preds[0] )
	    i++;
	  if ( j->index != preds[1] )
	    j++;
	  if ( i->index == preds[0] && j->index == preds[1] )
	    break;
	}
	assert( i != origCandidates.end() && j != origCandidates.end() );
	Conjunction c( &*i, &*j );
	
	// XML it up!  (replace "cout" with "view" to output to the file)
	view << "<conjunction initial=\"" << c.score() << "\" effective=\""
	     << bestConj.front().score() << "\">";
	for ( unsigned i = 0; i < preds.size(); i++ )
	  view << "<pred index=\"" << preds.at(i) +1 << "\"/>";
	view << c.bugometerXML() << bestConj.front().bugometerXML()
	     << "</conjunction>";

	// Can't put it in foci, since it doesn't share an indexing style...
	// Maybe someone more familiar with the code can fix this.
	
	allFailures.dilute( bestConj.front(), bestConj.front().tru.failures );
	if ( allFailures.count <= 0 ) {
	  break;
	}
	
	for ( Candidates::iterator loser = candidates.begin(); loser != candidates.end(); ++loser )
	  loser->dilute( bestConj.front() );
	
	progress.step();
	candidates.rescore(progress);
      }
      else {
	view << *winner;
	if (foci)
	  foci->insert(winner->index);
	
	allFailures.dilute(*winner, winner->tru.failures);
	if (allFailures.count <= 0)
	  break;
	
	for (Candidates::iterator loser = candidates.begin(); loser != candidates.end(); ++loser)
	  if (loser != winner)
	    loser->dilute(*winner);
	
	progress.step();
	candidates.erase(winner);
	candidates.rescore(progress);
      }
    }
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static int
parseFlag(int key, char *arg, argp_state *state)
{
  using namespace Confidence;

  switch (key) {
  case 'z':
    zoomAttr = arg;
    if (zoomAttr == "all")
      zoomsWanted = ZoomAll;
    else if (zoomAttr == "winners")
      zoomsWanted = ZoomWinners;
    else if (zoomAttr == "none")
      zoomsWanted = ZoomNone;
    else
      argp_error(state, "argument to --zoom must be one of \"all\", \"winners\", or \"none\"");
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
      "PREDS",
      0,
      "generate zoom views for selected predicates: \"all\", \"winners\", or \"none\" (default: none)",
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
  set_terminate_verbose();
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

  Candidates candidates;
  candidates.load();

  switch (zoomsWanted)
    {
    case ZoomNone:
      buildView(candidates, projection);
      break;
    case ZoomAll:
      buildView(candidates, projection);
      buildZooms(candidates, projection);
      break;
    case ZoomWinners:
      Foci foci;
      buildView(candidates, projection, &foci);
      buildZooms(candidates, projection, foci);
      break;
    }
}
