#include <argp.h>
#include <cassert>
#include <fenv.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "Confidence.h"
#include "CountingIterator.h"
#include "Progress/Bounded.h"
#include "Progress/Unbounded.h"
#include "Stylesheet.h"
#include "ViewPrinter.h"

#include "utils.h"

using namespace std;


const char *Stylesheet::filename = "projected-view.xsl";


////////////////////////////////////////////////////////////////////////
//
//  A set of run numbers represented by an ordered vector
//


struct RunSet : public list<unsigned>
{
  size_t count;
  void load(ifstream &, char expected);

  void dilute(const RunSet &winner);
};


static istream &
operator>>(istream &in, RunSet &runs)
{
  assert(runs.empty());
  runs.count = 0;

  unsigned runId;
  in.exceptions(ios::badbit);
  while (in >> runId)
    {
      runs.push_back(runId);
      ++runs.count;
    }

  return in;
}


void
RunSet::load(ifstream &in, char expected)
{
  string actual;
  in >> actual;
  if (in)
    {
      assert(actual.size() == 2);
      assert(actual[0] == expected);
      assert(actual[1] == ':');

      string line;
      getline(in, line);

      istringstream parse(line);
      parse >> *this;
    }
}


void
RunSet::dilute(const RunSet &winner)
{
  // in-place set difference: *this -= winner

  RunSet::iterator loserRun = begin();
  RunSet::const_iterator winnerRun = winner.begin();

  while (loserRun != end() && winnerRun != winner.end())
    if (*loserRun < *winnerRun)
      ++loserRun;
    else
      {
	if (*loserRun == *winnerRun)
	  {
	    loserRun = erase(loserRun);
	    --count;
	  }
	++winnerRun;
      }
}


////////////////////////////////////////////////////////////////////////
//
//  The global set of all failed runs
//


static RunSet allFailures;


////////////////////////////////////////////////////////////////////////
//
//  A pair of run sets: failures and successes
//


struct RunSuite
{
  RunSet failures;
  RunSet successes;

  void load(ifstream &);

  size_t count() const;
  double failRate() const;
  double errorPart() const;

  void dilute(const RunSuite &winner);
};


void
RunSuite::load(ifstream &in)
{
  failures.load(in, 'F');
  successes.load(in, 'S');
}


size_t
RunSuite::count() const
{
  return failures.count + successes.count;
}


double
RunSuite::failRate() const
{
  const size_t population = count();
  const double result =
    population
    ? double(failures.count) / population
    : 0;
  assert(result >= 0);
  assert(result <= 1);
  return result;
}


double
RunSuite::errorPart() const
{
  const size_t population = count();
  return population
    ? failures.count * successes.count / pow(double(population), 3)
    : 0;
}


void
RunSuite::dilute(const RunSuite &winner)
{
  failures.dilute(winner.failures);
  successes.dilute(winner.successes);
}


bool
operator==(const RunSuite &a, const RunSuite &b)
{
  return a.failures == b.failures
    && a.successes == b.successes;
}


////////////////////////////////////////////////////////////////////////
//
//  A numbered predicate with various run sets
//


struct Predicate
{
  Predicate(unsigned);
  const unsigned index;
  double initial, effective;

  RunSuite tru;
  RunSuite obs;

  void load(ifstream &tru, ifstream &obs);

  double badness() const;
  double context() const;
  double increase() const;
  double lowerBound() const;
  double recall() const;
  double harmonic() const;
  double score() const;

  void dilute(const Predicate &winner);
};


inline
Predicate::Predicate(unsigned index)
  : index(index)
{
}


void
Predicate::load(ifstream &truFile, ifstream &obsFile)
{
  tru.load(truFile);
  obs.load(obsFile);
  initial = effective = score();
}


inline double
Predicate::badness() const
{
  return tru.failRate();
}


inline double
Predicate::context() const
{
  return obs.failRate();
}


double
Predicate::increase() const
{
  return badness() - context();
}


double
Predicate::lowerBound() const
{
  double quantile = 0;
  switch (Confidence::level)
    {
    case 90:
      quantile = 1.645;
      break;
    case 95:
      quantile = 1.96;
      break;
    case 96:
      quantile = 2.05;
      break;
    case 98:
      quantile = 2.33;
      break;
    case 99:
      quantile = 2.58;
      break;
    default:
      cerr << "confidence table is incomplete for level " << Confidence::level << "%\n";
      exit(1);
    }

  const double standardError = sqrt(tru.errorPart() + obs.errorPart());
  return increase() - quantile * standardError;
}


double
Predicate::recall() const
{
  assert(tru.failures.count > 0);
  assert(allFailures.count > 0);

  const double result = log(double(tru.failures.count)) / log(double(allFailures.count));
  assert(result >= 0);
  assert(result <= 1);

  return result;
}


double
Predicate::harmonic() const
{
  return 2 / (1 / lowerBound() + 1 / recall());
}


inline double
Predicate::score() const
{
  const double result = harmonic();
  assert(result >= 0);
  return result;
}


void
Predicate::dilute(const Predicate &winner)
{
  tru.dilute(winner.tru);
  obs.dilute(winner.tru);
}


bool
operator<(const Predicate &a, const Predicate &b)
{
  return a.effective < b.effective;
}


////////////////////////////////////////////////////////////////////////
//
//  A sequence of predicates
//


struct Predicates : public list<Predicate>
{
  Predicates();
  unsigned count;
};


Predicates::Predicates()
  : count(0)
{
  ifstream tru("tru.txt");
  ifstream obs("obs.txt");
  assert(tru);
  assert(obs);

  Progress::Unbounded progress("reading predicates");
  while (tru && obs)
    {
      // speculate that there is at least one more predicate so we can
      // create space for it first and then build it in place
      push_back(Predicate(count++));
      back().load(tru, obs);
      if (tru && obs) progress.step();
    }

  // last speculation was wrong
  pop_back();
  --count;

  assert(tru.eof());
  assert(obs.eof());
}


////////////////////////////////////////////////////////////////////////
//
//  The main iterative ranking / selection / dilution loop
//


static void
buildView(Predicates &candidates)
{
  // create XML output file and write initial header
  ViewPrinter view(Stylesheet::filename, "projected-view", "all", "hl", "corrective");

  Progress::Bounded progress("ranking predicates", candidates.count);

  // pluck out predicates one by one, printing as we go
  while (!candidates.empty())
    {
      progress.step();
      const Predicates::iterator winner = max_element(candidates.begin(), candidates.end());

      view << "<predictor index=\"" << winner->index + 1
	   << "\" initial=\"" << winner->initial
	   << "\" effective=\"" << winner->effective
	   << "\"/>";

      allFailures.dilute(winner->tru.failures);

      Predicates::iterator loser = candidates.begin();
      while (loser != candidates.end())
	if (loser != winner)
	  {
	    loser->dilute(*winner);
	    if (loser->lowerBound() <= 0)
	      {
		loser = candidates.erase(loser);
		progress.step();
	      }
	    else
	      {
		loser->effective = loser->score();
		++loser;
	      }
	  }
	else
	  ++loser;

      candidates.erase(winner);
    }
}


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


static void
processCommandLine(int argc, char *argv[])
{
  static const argp_child children[] = {
    { &Confidence::argp, 0, 0, 0 },
    { &Stylesheet::argp, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };

  static const argp argp = {
    0, 0, 0, 0, children, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
}


int
main(int argc, char *argv[])
{
  // command line processing and other initialization
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);
  // feenableexcept(FE_DIVBYZERO | FE_INVALID);

  {
    ifstream failuresFile("f.runs");
    assert(failuresFile);
    failuresFile >> allFailures;
  }

  Predicates candidates;
  buildView(candidates);

  return 0;
}
