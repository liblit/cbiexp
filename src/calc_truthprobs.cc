#include <argp.h>
#include <cassert>
#include <ext/hash_map>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include "classify_runs.h"
#include "CompactReport.h"
#include "fopen.h"
#include "NumRuns.h"
#include "PredCoords.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "sites.h"
#include "units.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;

static ofstream logfp("truthprobs.txt", ios_base::trunc);

/****************************************************************
 * Predicate-specific information
 ***************************************************************/

struct PredInfo
{
    unsigned tru, obs;
    double alpha; // truth rate = tru/obs
    double rho, lambda, beta;
    double truthprob;
    unsigned dst, dso, realt, realo;
    PredInfo() {
	tru = obs = 0;
	alpha = rho = lambda = beta = 0.0;
	truthprob = 0.0;
	dst = dso = realt = realo = 0;
    }
    void reinit() {
        truthprob = 0.0;
	dst = dso = realt = realo = 0;
    }
    void update_stats(unsigned _tru, unsigned _obs) {
	tru += _tru;
	obs += _obs;
    }
    void set_parms(double r, double l, double b) {
	rho = r;
	lambda = l;
	beta = b;
    }
    void calc_prob(unsigned t, unsigned o);
    void calc_zero_probs() {
      calc_prob(0,0);
    }
    void compare_prob (unsigned t, unsigned o) {
        realt = t;
        realo = o;
    }
};

void 
PredInfo::calc_prob(unsigned t, unsigned o) 
{
    dst = t;
    dso = o;
    if (t > 0)
	truthprob = 1.0;
    else {
	if (o == 0) {
	    double numerator = beta * exp(-lambda*rho-lambda*alpha*(1.0-rho)) + (1.0-beta);
	    double denom = beta*exp(-lambda*rho) + (1.0-beta);
	    truthprob = 1.0- numerator/denom;
	}
	else {
            truthprob = 1.0 - exp(-lambda*alpha*(1.0-rho));
	}
    }
}

class PredInfoPair;
typedef PredInfo (PredInfoPair::* piptr);

class PredInfoPair
{
public:
    PredInfo f;
    PredInfo s;
    void update_stats(piptr pp, unsigned tru, unsigned obs) {
	(this->*pp).update_stats(tru, obs);
    }
    void set_parms(piptr pp, double rho, double lambda, double beta) {
	(this->*pp).set_parms(rho, lambda, beta);
    }
    void calc_prob(piptr pp, unsigned tru, unsigned obs) {
	(this->*pp).calc_prob(tru, obs);
    }
    void compare_prob(piptr pp, unsigned tru, unsigned obs) {
	(this->*pp).compare_prob(tru, obs);
    }
    void reinit(piptr pp) {
        (this->*pp).reinit();
    }
};

class pred_hash_t : public hash_map<PredCoords, PredInfoPair>
{
public:
   void reinit(piptr pp) {
     for (pred_hash_t::iterator c = begin(); c != end(); ++c) {
       PredInfoPair &PP = c->second;
       PP.reinit(pp);
     }
   }
};

static pred_hash_t predHash;
static piptr pptr = 0;

inline ostream &
operator<< (ostream &out, const PredInfo &pi)
{
    out << pi.tru << '\t' << pi.obs << '\t' << pi.alpha << '\t'
	<< pi.rho << '\t' << pi.lambda << '\t' << pi.beta << '\t'
	<< pi.truthprob << '\t' << pi.dst << '\t' << pi.dso << '\t'
	<< pi.realt << '\t' << pi.realo;
    return out;
}

inline ostream &
operator<< (ostream &out, const PredInfoPair &pp)
{
  out << (pp.*pptr); 
  return out;
}

inline ostream &
operator<< (ostream &out, const pred_hash_t &hash)
{
  for (pred_hash_t::const_iterator c = hash.begin(); c != hash.end(); ++c) 
  {
    out << c->first << '\n' << c->second << endl;
  }
  return out;
}

/****************************************************************
 * Information about each run
 ***************************************************************/
enum { CALC_ALPHA, READ_DS, READ_FULL };

class Reader : public ReportReader
{
public:
  Reader(int);
  void branchesSite(const SiteCoords &, unsigned, unsigned);
  void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned);
  void returnsSite(const SiteCoords &, unsigned, unsigned, unsigned);
  void scalarPairsSite(const SiteCoords &, unsigned, unsigned, unsigned);

private:
  void tripleSite(const SiteCoords &, unsigned, unsigned, unsigned) const;
  void update(const SiteCoords &, unsigned, unsigned, unsigned) const;
  const int mode;
};

inline
Reader::Reader(int _mode)
    : mode(_mode)
{
}

inline void
Reader::update(const SiteCoords &coords, unsigned p, unsigned tru, unsigned obs) const
{
    PredCoords pc(coords, p);
    const pred_hash_t::iterator found = predHash.find(pc);
    if (found != predHash.end()) {
	PredInfoPair &pp = found->second;
	switch (mode) {
	case CALC_ALPHA:
	    pp.update_stats(pptr, tru, obs);
	    break;
	case READ_DS:
	    pp.calc_prob(pptr, tru, obs);
	    break;
	case READ_FULL:
	    pp.compare_prob(pptr, tru, obs);
	    break;
	default:
	    assert(0);
	}
    }
}

void Reader::tripleSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) const
{
    assert(x || y || z);
    update(coords, 0, x, x+y+z);
    update(coords, 1, y+z, x+y+z);
    update(coords, 2, y, x+y+z);
    update(coords, 3, x+z, x+y+z);
    update(coords, 4, z, x+y+z);
    update(coords, 5, x+y, x+y+z);
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
    update(coords, 0, x, x+y);
    update(coords, 1, y, x+y);
}

inline void
Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z, unsigned w)
{
    assert(x || y || z || w);
    update(coords, 0, x, x+y+z+w);
    update(coords, 1, y, x+y+z+w);
    update(coords, 2, z, x+y+z+w);
    update(coords, 3, w, x+y+z+w);
}

/****************************************************************************
 * Utilities
 ***************************************************************************/

// Read the list of selected predicates
void read_preds()
{
  FILE * const pfp = fopenRead(PredStats::filename);
  pred_info pi;
  PredInfoPair pp;
  while (read_pred_full(pfp, pi)) {
      predHash[pi] = pp;
  }

  fclose(pfp);
}

void 
set_parms (SiteCoords &sc, piptr pp, double rho, double lambda, double beta)
{
    unsigned p = 0;
    switch (units[sc.unitIndex].scheme_code) {
    case 'S':
    case 'R':
	p = 6;
	break;
    case 'B':
	p = 2;
	break;
    case 'G':
	p = 4;
	break;
    default:
	assert(0);
    }

    for (unsigned i = 0; i < p; i++) {
	PredCoords pc(sc, i);
	pred_hash_t::iterator found = predHash.find(pc);
	if (found != predHash.end()) {
	    PredInfoPair &info = found->second;
	    info.set_parms(pp, rho, lambda, beta);
	}
    }
}

void
read_parms()
{
  FILE *fp = fopenRead("hyperparms.txt");
  if (!fp) {
    cerr << "Cannot open hyperparms.txt for reading\n";
    exit(1);
  }

  SiteCoords coords;
  unsigned S, N, Z;
  double a, b;
  double rho, lambda, beta;
  int ctr;
  
  while (true) {
    ctr = fscanf(fp, "%u\t%u\n", &coords.unitIndex, &coords.siteOffset);
    if (feof(fp))
      break;
    assert(ctr == 2);

    ctr = fscanf(fp, "%u\t%u\t%u\t%lg\t%lg\t%lg\t%lg\t%lg\n", &S, &N, &Z, &a, &b, &rho, &lambda, &beta);
    assert(ctr == 8);

    set_parms(coords, &PredInfoPair::f, rho, lambda, beta);

    ctr = fscanf(fp, "%u\t%u\t%u\t%lg\t%lg\t%lg\t%lg\t%lg\n", 
           &S, &N, &Z, &a, &b, &rho, &lambda, &beta);
    assert(ctr == 8);

    set_parms(coords, &PredInfoPair::s, rho, lambda, beta);
  }

  fclose(fp);
}

void calc_alpha () 
{
  for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
    PredInfoPair &pp = c->second;
    if (pp.f.tru > 0)
      pp.f.alpha = (double) pp.f.tru / pp.f.obs;
    if (pp.s.tru > 0)
      pp.s.alpha = (double) pp.s.tru / pp.s.obs;
  }
}

void calc_zero_probs(piptr pp) // calculate truthprobs of predicates we didn't see in this previous run
{
  for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
    PredInfoPair &PP = c->second;
    if ((PP.*pp).dso == 0) // it is zero if the predicate hasn't been observed in the previous run
      (PP.*pp).calc_zero_probs();
  }
}

/****************************************************************************
 * Print results to file
 ***************************************************************************/

void print_results(unsigned r)
{
  logfp << "Run " << r;
  if (pptr == &PredInfoPair::f)
    logfp << " (f)\n";
  else if (pptr == &PredInfoPair::s)
    logfp << " (s)\n";
  else
    assert(0);
  logfp << predHash;
}


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &CompactReport::argp, 0, 0, 0 },
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
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

    classify_runs();

    read_preds();
    read_parms();

    Progress::Bounded prog("Reading runs to calculate alpha", NumRuns::count());
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
	prog.step();
	if (is_srun[r])
	    pptr = &PredInfoPair::s;
	else if (is_frun[r])
	    pptr = &PredInfoPair::f;
	else
	    continue;

	Reader(CALC_ALPHA).read(r);
    }

    calc_alpha ();

    string report_suffix = CompactReport::suffix;
    unsigned ctr = 0;
    Progress::Bounded prog2("Calculating truth probabilities", 5000);
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
	if (ctr == 1000)
	    break;

	if (is_srun[r])
	    pptr = &PredInfoPair::s;
	else if (is_frun[r])
	    pptr = &PredInfoPair::f;
	else
	    continue;

	prog2.step();
	ctr++;
        predHash.reinit(pptr);

	CompactReport::suffix = report_suffix;
	Reader(READ_DS).read(r);
	calc_zero_probs(pptr);

	CompactReport::suffix = "";
	Reader(READ_FULL).read(r);

        print_results(r);
    }


    logfp.close();
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
