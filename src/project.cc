#include <argp.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "Matrix.h"
#include "Score/HarmonicMean.h"
#include "classify_runs.h"
#include "generate-view.h"
#include "utils.h"

using namespace std;


typedef Matrix<double> Rho;


////////////////////////////////////////////////////////////////////////
//
//  A predicate with its current (discounted) popularity
//
//  To change the core ranking function on which the projection
//  algorithm operates, change Candidate::Score.
//


typedef double score;


struct Candidate : public pred_info
{
  Candidate(const pred_info &);
  double firstImpression() const;
  double popularity;

  typedef Score::HarmonicMean Score;
};


inline
Candidate::Candidate(const pred_info &stats)
  : pred_info(stats),
    popularity(firstImpression())
{
}


inline double
Candidate::firstImpression() const
{
  return Score()(*this);
}


static vector<Candidate> predStats;


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


// may be changed using line flags
const char *predStatsName = "preds.txt";
const char *rhoName = "rho.txt";

static const char *successList = "s.runs";
static const char *failureList = "f.runs";

bool verbose = false;


static const argp_option options[] = {
  {
    "predicates",
    'p',
    "preds.txt",
    0,
    "predicate statistics file",
    0
  },
  {
    "rho",
    'r',
    "rho.txt",
    0,
    "correlation matrix file",
    0
  },
  {
    "verbose",
    'v',
    0,
    0,
    "verbosely trace selection process",
    0
  },
  {
    "success-list",
    's',
    "s.runs",
    0,
    "file listing successful runs",
    0
  },
  {
    "failure-list",
    'f',
    "f.runs",
    0,
    "file listing failed runs",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key)
    {
    case 'p':
      predStatsName = arg;
      return 0;
    case 'r':
      rhoName = arg;
      return 0;
    case 'v':
      verbose = true;
      return 0;
    case 's':
      successList = arg;
      return 0;
    case 'f':
      failureList = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


static const argp argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};


////////////////////////////////////////////////////////////////////////
//
//  File input
//


static void
readPredStats(const char *filename)
{
  cerr << "reading predicate stats: " << flush;
  FILE * const file = fopen_read(filename);

  pred_info info;
  while (read_pred_full(file, info))
    predStats.push_back(info);

  fclose(file);
  cerr << "got " << predStats.size() << '\n';
}


static void
readCorrelations(const char *filename, Rho &rho)
{
  cerr << "reading " << rho.size << "² correlation matrix\n";

  ifstream file;
  file.exceptions(ios::eofbit | ios::failbit | ios::badbit);
  file.open(filename);

  for (predIndex row = 0; row < rho.size; ++row)
    for (predIndex column = 0; column < rho.size; ++column)
      file >> rho[row][column];
}


////////////////////////////////////////////////////////////////////////
//
//  Sorting and ranking
//


static bool
lessPopularThan(predIndex a, predIndex b)
{
  return predStats[a].popularity < predStats[b].popularity;
}


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int
main(int argc, char *argv[])
{
  // command line processing and other simple initialization
  ios::sync_with_stdio(false);
  argp_parse(&argp, argc, argv, 0, 0, 0);
  classify_runs(successList, failureList);

  // load up the standard stats: Increase, Fail, Context, etc.
  readPredStats(predStatsName);
  const predIndex size = predStats.size();

  // load up the pairwise predicate correlation matrix
  cerr << "preallocating " << size << "² correlation matrix\n";
  Rho rho(size);
  readCorrelations(rhoName, rho);

  // build an efficiently prunable list of not-yet-picked predicates
  cerr << "ranking " << size << " predicates\n";
  typedef list<predIndex> Candidates;
  Candidates candidates;
  for (predIndex index = 0; index < size; ++index)
    candidates.push_back(index);

  // emit XML header; will emit predicates as we go along
  cout << "<?xml version=\"1.0\"?>"
       << "<?xml-stylesheet type=\"text/xsl\" href=\"projected-view.xsl\"?>"
       << "<!DOCTYPE projected-view SYSTEM \"projected-view.dtd\">"
       << "<projected-view sort=\"" << Candidate::Score::code << "\">";

  while (!candidates.empty())
    {
      const Candidates::iterator element = max_element(candidates.begin(), candidates.end(), lessPopularThan);
      const predIndex winner = *element;
      candidates.erase(element);
      const Candidate &stats = predStats[winner];

      PredicatePrinter printer(cout, stats);
      cout << "<popularity initial=\"" << stats.firstImpression()
	   << "\" effective=\"" << stats.popularity
	   << "\"/>";

      if (verbose)
	cerr << "winner " << winner << ":\n"
	     << "\tpopularity:\t" << stats.popularity << '\n';

      for (Candidates::const_iterator loser = candidates.begin();
	   loser != candidates.end(); ++loser)
	{
	  const score correlation = rho[winner][*loser];
	  const score dilution = abs(cos(M_PIl / 2 * correlation));
	  score &slot = predStats[*loser].popularity;
	  const score previous = slot;

	  slot *= dilution;
	  assert(slot >= 0);

	  if (verbose)
	    cerr << "loser " << *loser << ":\n"
		 << "\told popularity:\t" << previous << '\n'
		 << "\tcorrelation:\t" << correlation << '\n'
		 << "\tdilution:\t" << dilution << '\n'
		 << "\tnew popularity:\t" << slot << '\n';
	}
    }

  cout << "</projected-view>\n";

  return 0;
}
