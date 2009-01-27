/****************************************************************
 * This program collects sufficient statistics for estimating the
 * parameters of the truth probability model.  Statistics are
 * collected for all predicates that passed the pre-filtering test,
 * as well as the complements of those predicates.  Statistics for
 * failed runs are collected separately from those of successful
 * runs.
 * Input:
 * 1. file containing sampling rates (defined on the commandline)
 * 2. preds.txt (predicates retained after pre-filtering)
 * 3. all the program runs
 * 4. s.runs and f.runs
 * Output:
 * parmstats.txt and notp-parmstats.txt: sufficient stats for the
 *   list of predicates in preds.txt and their complements (in the
 *   same order as preds.txt)
 *
 * -- documented by alicez 04/2007
 ***************************************************************/

#include <argp.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include "fopen.h"
#include "DiscreteDist.h"
#include "NumRuns.h"
#include "PredCoords.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "SampleRatesFile.h"
#include "ReportReader.h"
#include "SiteCoords.h"
#include "classify_runs.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;

typedef queue<PredCoords> Preds;
Preds retainedPreds;

/****************************************************************
 * Structure to store information about predicates
 * N = number of runs the predicate is reached at least once
 * Z = number of runs in which predicate is not reached
 * S = total number of times pred is observed in all (failed/succ) runs
 * Y = total number of times pred is true in all (f/s) runs
 * a = min(number of times observed in any run)
 * b = max(number of times observed in any run)
 * rho = sampling rate of the predicate
 * Asize = number of runs in set A (num_obs > num_true > 0)
 * Asumobs = total number of times predicate is observed in all runs
 * Asumtrue = total number of times predicate is true in all runs
 * Bsize = number of runs in set B (num_obs > num_true = 0)
 * Bset is a histogram of num_obs counts in the runs in set B
 * Csize = number of runs in set C (num_obs = num_true > 0)
 * Cset is a histogram of num_obs counts in the runs in set C
 ***************************************************************/

class pred_info_t {
private:
    unsigned N, Z;
    count_tp S, Y;
    double a;
    double b;
    double rho;
    unsigned Asize;
    count_tp Asumtrue, Asumobs;
    unsigned Bsize, Csize;
    DiscreteDist Bset; // a histogram of m values in set B
    DiscreteDist Cset; // a histogram of m values in set C
public:
    pred_info_t () {
	S = N = Y = Z = 0;
	a = (numeric_limits<double>::max)();
	b = 0.0;
	rho = 1.0;
	Asize = Asumtrue = Asumobs = Bsize = Csize = 0;
    }
    void update(const count_tp m, const count_tp y);
    void setrho(const double r) { rho = r; }
    void setmin(const unsigned nruns);
    void print(ostream &out) const;
};

void pred_info_t::update (const count_tp m, const count_tp y)
{
  N += 1;
  S += m; // S = total observed
  Y += y; // Y = total true
  a = (a < m) ? a : (double) m;
  b = (b > m) ? b : (double) m;
  if (m > y) {
      if (y > 0) { // set A
	  Asumtrue += y;
	  Asumobs += m;
	  ++Asize;
      } else { // m > y and y == 0 -> set B
	  if (Bset.find(m) == Bset.end())
	    Bset[m] = 1;
          else
	    Bset[m] += 1;
	  ++Bsize;
      }
  } else { // m == y > 0 -> set C
      if (Cset.find(m) == Cset.end())
        Cset[m] = 1;
      else
	Cset[m] += 1;
      ++Csize;
  }
}

/* Set the number of runs in which a predicate is not observed.
 * If there are such runs, set a (minimum num_obs) to be zero.
 */
inline void
pred_info_t::setmin (const unsigned nruns)
{
  Z = nruns - N;
  if (Z > 0)
    a = 0.0;
}

/* container for a pair of predicates */
class PredPair;
typedef pred_info_t (PredPair::* PredInfoPtr);
class PredPair
{
public:
    pred_info_t f;
    pred_info_t s;

    void init() {}
    void update(PredInfoPtr, count_tp, count_tp);
};

inline void
PredPair::update(PredInfoPtr pipt, count_tp nobs, count_tp ntrue)
{
    (this->*pipt).update(nobs, ntrue);
}

class pred_hash_t : public hash_map<PredCoords, PredPair>
{
};
static pred_hash_t predHash;

/****************************************************************
 * Structure for processing information about each run
 ***************************************************************/

class Reader : public ReportReader
{
public:
  Reader(const char* filename) : ReportReader(filename) {}
  void setptr(unsigned);

protected:
  void handleSite(const SiteCoords &, vector<count_tp> &);

private:
    void update(const SiteCoords &, unsigned, count_tp, count_tp) const;
    PredInfoPtr pipt;
};

inline void
Reader::setptr(unsigned run)
{
  if (is_frun[run]) pipt = &PredPair::f;
  else
    if (is_srun[run]) pipt = &PredPair::s;
    else
    {
        ostringstream message;
        message << "Ill-formed run " << run;
        throw runtime_error(message.str());
    }
}


inline void
Reader::update(const SiteCoords &coords, unsigned p, count_tp nobs, count_tp ntrue) const
{
    const PredCoords pc(coords, p);
    pred_hash_t::iterator found = predHash.find(pc);
    if (found != predHash.end()) {
	PredPair &pp = found->second;
	pp.update(pipt, nobs, ntrue);
    }
}

void
Reader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
{
    const count_tp sum = accumulate(counts.begin(), counts.end(), (count_tp) 0);
    assert(sum > 0);

    const size_t size = counts.size();
    if (size == 2)
	for (unsigned predicate = 0; predicate < size; ++predicate)
	    update(coords, predicate, sum, counts[predicate]);
    else
	for (unsigned predicate = 0; predicate < size; ++predicate) {
	    update(coords, 2 * predicate,     sum,       counts[predicate]);
	    update(coords, 2 * predicate + 1, sum, sum - counts[predicate]);
	}
}

/****************************************************************************
 * Utility functions
 ***************************************************************************/

/* For each interesting predicate, set rho according to the sites rate. 
 * rho is initialized to have a default value of 1.0. (See definition of
 * pred_info_t)
 */
void set_rates ()
{
    for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
	const PredCoords &pc = c->first;
	PredPair &pp = c->second;
        const double rho = SampleRatesFile::rates[pc.siteIndex];
        pp.f.setrho(rho);
        pp.s.setrho(rho);
    }
}

void set_min()
{
  for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
    PredPair &pp = c->second;
    pp.f.setmin(num_fruns);
    pp.s.setmin(num_sruns);
  }
}


/****************************************************************************
 * Print results to file
 ***************************************************************************/

inline ostream &
operator<< (ostream &out, const DiscreteDist &dist)
{
  for (DiscreteDist::const_iterator c = dist.begin(); c != dist.end(); ++c)
  {
    out << c->first << ' ' << c->second << ' ';
  }
  return out;
}

void
pred_info_t::print (ostream &out)  const
{
    out << S << ' ' << N << ' ' << Y << ' ' << Z << ' '
	<< a << ' ' << b << ' ' << rho << ' '
	<< Asize << ' ' << Asumtrue << ' ' << Asumobs << ' '
	<< Bsize << ' ' << Csize << endl;
    out << Bset << endl;
    out << Cset << endl;
}

inline ostream &
operator<< (ostream &out, const pred_info_t &pi)
{
  pi.print(out);
  return out;
}

inline ostream &
operator<< (ostream &out, const PredPair &pp)
{
    out << pp.f << pp.s;
    return out;
}

void print_results()
{
    ofstream parmsfp ("parmstats.txt", ios_base::trunc);
    ofstream notpparmsfp ("notp-parmstats.txt", ios_base::trunc);
    while (!retainedPreds.empty()) {
	PredCoords &coords = retainedPreds.front();
	PredCoords notp = notP(coords);
	pred_hash_t::iterator found = predHash.find(coords);
	pred_hash_t::iterator found2 = predHash.find(notp);
	if (found == predHash.end()) {
	    cerr << "Cannot find stats for predicate " << coords << endl;
	    exit(1);
	}
	if (found2 == predHash.end()) {
	    cerr << "Cannot find stats for predicate " << notp << endl;
	    exit(1);
	}

	PredPair &pp = found->second;
	parmsfp << coords << '\n' << pp;

	PredPair &notpp = found2->second;
	notpparmsfp << notp << '\n' << notpp;

	retainedPreds.pop();
    }
    parmsfp.close();
    notpparmsfp.close();
}


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &NumRuns::argp, 0, 0, 0 },
        { &SampleRatesFile::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}


/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    classify_runs(); // classify runs as either successful or failing

    FILE * const pfp = fopenRead(PredStats::filename);
    pred_info pi;
    // read in the list of pre-filtered predicates, insert both the
    // predicate and its complement into the hashtable if they don't
    // already exist
    while (read_pred_full(pfp, pi)) {
	PredPair &pp = predHash[pi];
	PredPair &notpp = predHash[notP(pi)];
	pp.init();
	notpp.init();
	retainedPreds.push(pi);
    }
    fclose(pfp);

    SampleRatesFile::read_rates();  // read the sampling rates from file
    set_rates();   // set sampling rates in predHash according to values read from file

    Reader reader("counts.txt");
    Progress::Bounded prog("Reading runs and collecting sufficient stats", NumRuns::count());
    for (unsigned r = NumRuns::begin(); r < NumRuns::end(); ++r) {
      prog.step();
      reader.setptr(r);
      reader.read(r);
    }

    set_min();

    print_results();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
