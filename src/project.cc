#include <argp.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "Matrix.h"
#include "generate-view.h"
#include "utils.h"

using namespace std;


typedef Matrix<double> Rho;


////////////////////////////////////////////////////////////////////////
//
//  A predicate with its current (discounted) popularity
//
//  To change the core ranking function on which the projection
//  algorithm operates, change the initialization of member field
//  Candidate::popularity in the Candidate::Candidate() constructor.
//


typedef double score;


struct Candidate : public pred_info
{
  Candidate(const pred_info &);
  double popularity;
};


inline
Candidate::Candidate(const pred_info &stats)
  : pred_info(stats),
    popularity(stats.ps.lb)
{
}


static vector<Candidate> predStats;


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


// may be changed using line flags
const char *predStatsName = "preds.txt";
const char *rhoName = "rho.txt";
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
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


static const argp argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};


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


static bool
lessPopularThan(predIndex a, predIndex b)
{
  return predStats[a].popularity < predStats[b].popularity;
}


int
main(int argc, char *argv[])
{
  ios::sync_with_stdio(false);
  argp_parse(&argp, argc, argv, 0, 0, 0);

  readPredStats(predStatsName);
  const predIndex size = predStats.size();

  cerr << "preallocating " << size << "² correlation matrix\n";
  Rho rho(size);
  readCorrelations(rhoName, rho);

  cerr << "ranking " << size << " predicates\n";
  typedef list<predIndex> Candidates;
  Candidates candidates;
  for (predIndex index = 0; index < size; ++index)
    candidates.push_back(index);

  Permutation winners;
  winners.reserve(size);

  while (!candidates.empty())
    {
      const Candidates::iterator element = max_element(candidates.begin(), candidates.end(), lessPopularThan);
      const predIndex winner = *element;
      candidates.erase(element);
      winners.push_back(winner);

      if (verbose)
	cerr << "winner " << winner << ":\n"
	     << "\tpopularity:\t" << predStats[winner].popularity << '\n';

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

  generateView(cout, "all", "projected", predStats.begin(), winners);

  return 0;
}
