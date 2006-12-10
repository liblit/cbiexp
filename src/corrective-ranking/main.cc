#include <cmath>
#include <iostream>
#include <stdio.h>

#include "../Confidence.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "../RunsDirectory.h"
#include "../Stylesheet.h"
#include "../ViewPrinter.h"
#include "../termination.h"
#include "../StaticSiteInfo.h"
#include "../PredStats.h"
#include "../SiteCoords.h"
#include "../arguments.h"
#include "../fopen.h"
#include "../termination.h"
#include "../utils.h"


#include "Candidates.h"
#include "Complex.h"
#include "Conjunction.h"
#include "Disjunction.h"
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


  // Make a FILE pointer...
  FILE * fout;
  char * fname = new char[strlen(projection) + strlen("complex_.txt") + 1];
  strcpy( fname, "complex_" );
  strcat( fname, projection );
  strcat( fname, ".txt" );
  // fname is either complex_corrective_exact.txt, or
  // complex_corrective_approximate.txt
  fout = fopen(fname,"w");

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
      std::list<Complex> best = combine(candidates, 1, bestScore, fout);

      // If the list has an item, and its score beats bestScore, it's what
      // we use.
      if ( best.size() == 1 && best.front().score() > bestScore ) {
	
	// We need to form a conjunction of the same predicates, with their
	// original RunSets, to get the original score of this one.
	
	std::vector<unsigned> preds = best.front().getPredicateList();
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
	
	Complex *initial;
	if(best.front().getType() == 'C')
	  initial = new Conjunction(&*i, &*j);
	else
	  initial = new Disjunction(&*i, &*j);
	
	// XML it up!  (replace "cout" with "view" to output to the file)
	view << "<conjunction initial=\"" << initial->getType() << initial->score() << "\" effective=\""
	     << best.front().score() << "\">";
	for ( unsigned i = 0; i < preds.size(); i++ )
	  view << "<pred index=\"" << preds.at(i) +1 << "\"/>";
	view << initial->bugometerXML() << best.front().bugometerXML()
	     << "</conjunction>";
	
	allFailures.dilute( best.front(), best.front().tru.failures );
	if ( allFailures.count <= 0 ) {
	  break;
	}
	
	for ( Candidates::iterator loser = candidates.begin(); loser != candidates.end(); ++loser )
	  loser->dilute( best.front() );
	
	progress.step();
	candidates.rescore(progress);
      }
      else {
	view << *winner;
    cout << *winner;
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
  // Close the file...
  fclose( fout );
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

  read_pairs();
}


void
rankMain(const char projection[])
{
  ifstream failuresFile("f.runs");
  assert(failuresFile);
  failuresFile >> allFailures;

  Candidates candidates;
  candidates.load();

  /*const StaticSiteInfo staticSiteInfo;  
  FILE * const raw = fopenRead(PredStats::filename);
  pred_info stats;
  while (read_pred_full(raw, stats))
  {
    const site_t &site = staticSiteInfo.site(stats.siteIndex);
    cout << site.line;
  }*/


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
