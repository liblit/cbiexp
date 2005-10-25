#include <algorithm>
#include <argp.h>
#include <functional>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <memory>

#include "Candidate.h"
#include "Dilute.h"
#include "MappedArray.h"
#include "Matrix.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "Rho.h"
#include "RunsDirectory.h"
#include "Score/Fail.h"
#include "Score/HarmonicMeanLog.h"
#include "Score/HarmonicMeanSqrt.h"
#include "Score/Increase.h"
#include "Score/LowerBound.h"
#include "Score/LowerTimesFails.h"
#include "Score/TrueInFails.h"
#include "StaticSiteInfo.h"
#include "Stylesheet.h"
#include "Verbose.h"
#include "ViewPrinter.h"
#include "classify_runs.h"
#include "fopen.h"
#include "utils.h"

using namespace std;

const char *Stylesheet::filename = "projected-view.xsl";


////////////////////////////////////////////////////////////////////////
//
//  Assorted pieces of global state
//


// predicates grouped by scheme
// map key "all" includes all schemes
typedef list<IndexedPredInfo> PredInfos;
typedef map<string, PredInfos> Schemes;


////////////////////////////////////////////////////////////////////////
//
//  Projective ranking
//


template <class Dilution>
static Candidate
nextBest(const Rho &rho, list<Candidate> &candidates, const Dilution dilution)
{
  typedef list<Candidate> Candidates;
  const typename Candidates::iterator winner = max_element(candidates.begin(), candidates.end());
  const Candidate result = *winner;
  candidates.erase(winner);
  const double * const rhoSlice = rho[result.index];

  if (Verbose::on)
    cout << "winner: " << result << '\n';

  for (typename Candidates::iterator loser = candidates.begin(); loser != candidates.end(); ++loser)
    {
      const double correlation = min(1., abs(rhoSlice[loser->index]));
      loser->popularity *= dilution(correlation);

      if (Verbose::on)
	cout << "\tloser: " << *loser << ", correlation " << correlation << '\n';
    }

  return result;
}


template <class Score, class Dilution>
static void
buildView(const Rho &rho, const Schemes::value_type &scheme, const Score score, const Dilution dilution)
{
  // assign initial scores
  list<Candidate> candidates;
  for (PredInfos::const_iterator indexed = scheme.second.begin(); indexed != scheme.second.end(); ++indexed)
    candidates.push_back(Candidate(*indexed, score(*indexed)));

  // keep the user informed
  const string &schemeName = scheme.first;
  cout << "ranking " << candidates.size() << " predicates for scheme " << schemeName << ", score " << Score::code << ", dilution " << Dilution::name << '\n';

  // create XML output file and write initial header
  ViewPrinter view(Stylesheet::filename, "projected-view", schemeName, Score::code, Dilution::name);

  // pluck out predicates one by one, printing as we go
  while (!candidates.empty())
    view << nextBest(rho, candidates, dilution);
}


template <class Score>
static void
buildView(const Rho &rho, const Schemes::value_type &scheme, const Score score)
{
  using namespace Dilute;
  buildView(rho, scheme, score, Circular());
  buildView(rho, scheme, score, Linear());
}


static void
buildViews(const Rho &rho, const Schemes::value_type &scheme)
{
  using namespace Score;
  buildView(rho, scheme, LowerBound());
  buildView(rho, scheme, Increase());
  buildView(rho, scheme, Fail());
  buildView(rho, scheme, TrueInFails());
  buildView(rho, scheme, LowerTimesFails());
  buildView(rho, scheme, HarmonicMeanLog());
  buildView(rho, scheme, HarmonicMeanSqrt());
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static void processCommandLine(int argc, char *argv[])
{
  static const argp_child children[] = {
    { &NumRuns::argp, 0, 0, 0 },
    { &RunsDirectory::argp, 0, 0, 0 },
    { &Stylesheet::argp, 0, 0, 0 },
    { &Verbose::argp, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };

  static const argp argp = {
    0, 0, 0, 0, children, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
}


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int
main(int argc, char *argv[])
{
  // command line processing and other initialization
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);
  classify_runs();

  // predicates grouped by scheme
  // map key "all" includes all schemes
  Schemes schemes;
  PredInfos &all = schemes["all"];

  // load up predicates and note set of active schemes
  StaticSiteInfo staticSiteInfo;
  FILE * const statsFile = fopenRead(PredStats::filename);
  pred_info info;
  unsigned index = 0;
  while (read_pred_full(statsFile, info)) {
    const string &scheme = scheme_name(staticSiteInfo.site(info.siteIndex).scheme_code);
    IndexedPredInfo indexed(info, index);
    all.push_back(indexed);
    schemes[scheme].push_back(indexed);
    ++index;
  }

  // load correlation matrix
  const Rho rho(all.size());

  // generate sorted views for each individual scheme
  for(Schemes::const_iterator scheme = schemes.begin(); scheme != schemes.end(); ++scheme)
    buildViews(rho, *scheme);

  return 0;
}
