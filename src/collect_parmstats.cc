#include <argp.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ext/hash_map>
#include <queue>
#include "fopen.h"
#include "CompactReport.h"
#include "NumRuns.h"
#include "PredCoords.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "classify_runs.h"
#include "sites.h"
#include "units.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;

string sampleRateFile;

typedef queue<PredCoords> Preds;
Preds retainedPreds;

/****************************************************************
 * Information about non-constant predicates
 ***************************************************************/

typedef hash_map<unsigned, unsigned> DiscreteDist;

class pred_info_t {
private:
    unsigned S, N, Y, Z;
    double a;
    double b;
    double rho;
    unsigned Asize, Asumtrue, Asumobs;
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
    void update(const unsigned m, const unsigned y);
    void setrho(const double r) { rho = r; }
    void setmin(const unsigned nruns);
    void print(ostream &out) const;
};

void pred_info_t::update (const unsigned m, const unsigned y)
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

inline void
pred_info_t::setmin (const unsigned nruns)
{
  Z = nruns - N;
  if (Z > 0)
    a = 0.0;
}

class PredPair;
typedef pred_info_t (PredPair::* PredInfoPtr);
class PredPair
{
public:
    pred_info_t f;
    pred_info_t s;

    void init() {}
    void update(PredInfoPtr, unsigned, unsigned);
};

inline void
PredPair::update(PredInfoPtr pipt, unsigned nobs, unsigned ntrue)
{
    (this->*pipt).update(nobs, ntrue);
}

class pred_hash_t : public hash_map<PredCoords, PredPair> 
{
};
static pred_hash_t predHash;

typedef hash_map<SiteCoords, double> site_hash_t;
static site_hash_t sampleRates;

// This function reads down sampling rates off of file, if the info
// is available.  The non-uniform down-sampling rates file may not
// contain sampling rates for all sites.  (A site is omiited if it
// was not observed in the training runs.)  The default behavior of 
// the decimator is to not do anything; therefore the default down
// sampling rate is 1.
void 
read_rates()
{
  if (sampleRateFile.empty()) {
    return;
  }

  FILE *const rates = fopenRead(sampleRateFile.c_str());
  SiteCoords coords;
  double rho;

  while (true) {
    fscanf(rates, "%u\t%u\t%lg\n", &coords.unitIndex, &coords.siteOffset, &rho);
    if (feof(rates))
      break;

    assert(coords.unitIndex < num_units);
    const unit_t unit = units[coords.unitIndex];
    assert(coords.siteOffset < unit.num_sites);

    sampleRates[coords] = rho;
  }
  
  fclose(rates);
}

/* for each interesting predicate, set rho according to sampleRates hash */
void set_rates ()
{
    for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
	const PredCoords &pc = c->first;
	PredPair &pp = c->second;
	const site_hash_t::iterator found = sampleRates.find(pc);
	if (found == sampleRates.end()) {
	    cerr << "Cannot find sample rate for predicate " << pc << endl;
	    exit(1);
	}
	const double rho = found->second;
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

/****************************************************************
 * Information about each run
 ***************************************************************/

class Reader : public ReportReader
{
public:
  Reader(PredInfoPtr);
  void branchesSite(const SiteCoords &, unsigned, unsigned);
  void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned);
  void returnsSite(const SiteCoords &, unsigned, unsigned, unsigned);
  void scalarPairsSite(const SiteCoords &, unsigned, unsigned, unsigned);

private:
    void tripleSite(const SiteCoords &, unsigned, unsigned, unsigned) const;
    void update(const SiteCoords &, unsigned, unsigned, unsigned) const;
    const PredInfoPtr pipt;
};

inline
Reader::Reader(PredInfoPtr _pipt)
    : pipt(_pipt)
{
}

inline void
Reader::update(const SiteCoords &coords, unsigned p, unsigned nobs, unsigned ntrue) const
{
    const PredCoords pc(coords, p);
    pred_hash_t::iterator found = predHash.find(pc);
    if (found != predHash.end()) {
	PredPair &pp = found->second;
	pp.update(pipt, nobs, ntrue);
    }
}

void Reader::tripleSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) const
{
    assert(x || y || z);
    update(coords, 0, x+y+z, x);
    update(coords, 1, x+y+z, y+z);
    update(coords, 2, x+y+z, y);
    update(coords, 3, x+y+z, x+z);
    update(coords, 4, x+y+z, z);
    update(coords, 5, x+y+z, x+y);
}

inline void
Reader::scalarPairsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) 
{
    tripleSite(coords, x, y, z);
}


inline void
Reader::returnsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) 
{
    tripleSite(coords, x, y, z);
}

inline void
Reader::branchesSite(const SiteCoords &coords, unsigned x, unsigned y)
{
    assert(x||y);
    update(coords, 0, x+y, x);
    update(coords, 1, x+y, y);
}

inline void
Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z, unsigned w)
{
    assert(x || y || z || w);
    update(coords, 0, x+y+z+w, x);
    update(coords, 1, x+y+z+w, y);
    update(coords, 2, x+y+z+w, z);
    update(coords, 3, x+y+z+w, w);
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
    while (!retainedPreds.empty()) {
	PredCoords &coords = retainedPreds.front();
	pred_hash_t::iterator found = predHash.find(coords);
	if (found == predHash.end()) {
	    cerr << "Cannot find stats for predicate " << coords << endl;
	    exit(1);
	}

	PredPair &pp = found->second;
	parmsfp << coords << '\n' << pp;

	retainedPreds.pop();
    }
    parmsfp.close();
}


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

static const argp_option options[] = {
  {
    "sample-rates",
    'd',
    "FILE",
    0,
    "use \"FILE\" as the downsampling rate for each site",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};

static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key)
    {
    case 'd':
      sampleRateFile = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &CompactReport::argp, 0, 0, 0 },
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	options, parseFlag, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}


/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    classify_runs();

    FILE * const pfp = fopenRead(PredStats::filename);
    pred_info pi;
    while (read_pred_full(pfp, pi)) {
	PredPair &pp = predHash[pi];
	pp.init();
	retainedPreds.push(pi);
    }

    read_rates();
    set_rates();

    Progress::Bounded prog("Reading runs and collecting sufficient stats", NumRuns::count());
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
      prog.step();
      PredInfoPtr pipt = 0;
      if (is_srun[r])
	pipt = &PredPair::s;
      else if (is_frun[r])
        pipt = &PredPair::f;
      else
	continue;
  
      Reader(pipt).read(r);
    }

    set_min();

    print_results();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End: