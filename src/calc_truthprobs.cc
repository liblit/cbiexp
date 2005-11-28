#include <argp.h>
#include <cassert>
#include <cmath>
#include <ext/hash_map>
#include <fstream>
#include <iostream>
#include <numeric>
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
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;

static unsigned ntestruns = 100;
static double score1 = 0.0, score2 = 0.0, score_n1 = 0.0, score_n2 = 0.0;
static ofstream logfp("truthprobs.txt", ios_base::trunc);

/****************************************************************
 * Predicate-specific information
 ***************************************************************/

struct PredInfo
{
    unsigned tru, obs;
    double rho;
    double alpha; // truth rate = tru/obs
    double beta[3];
    double lambda, gamma;
    double tp, tp_n; // tp is P(X > 0|r,y), tp_n is P(X>0|n,r,y)
    double tp2, tp_n2; // truth probabilities from model 2
    unsigned dst, dso, realt, realo;
    PredInfo() {
	tru = obs = 0;
	rho = alpha = beta[0] = beta[1] = beta[2] = lambda = gamma = 0.0;
	tp = tp_n = 0.0;
	tp2 = tp_n2 = 0.0;
	dst = dso = realt = realo = 0;
    }
    void reinit_probs() {
        tp = tp_n = 0.0;
	tp2 = tp_n2 = 0.0;
	dst = dso = realt = realo = 0;
    }
    void update_stats(unsigned _tru, unsigned _obs) {
	tru += _tru;
	obs += _obs;
    }
    void set_parms(FILE *fp);
    void calc_prob(unsigned t, unsigned o);
    void calc_prob_n(unsigned t, unsigned o, unsigned n);
    void calc_prob2(unsigned t, unsigned o);
    void calc_prob_n2(unsigned t, unsigned o, unsigned n);
    void calc_zero_prob() {  calc_prob(0,0);    }
    void calc_zero_prob2() {  calc_prob2(0,0);    }
    void compare_prob (unsigned t, unsigned o) {
        realt = t;
        realo = o;
        calc_prob_n(dst, dso, realo);
	calc_prob_n2(dst, dso, realo);
        double answer = (realt > 0) ? 1.0 : 0.0;
        score1 += abs(answer - tp);
        score2 += abs(answer - tp2);
        score_n1 += abs(answer - tp_n);
        score_n2 += abs(answer - tp_n2);
    }
};

void
PredInfo::set_parms (FILE * fp)
{
  unsigned S, N, Y, Z;
  double a, b;
  unsigned Asize, Asumtrue, Asumobs, Bsize, Csize;
  int ctr = fscanf(fp, "%u %u %u %u %lg %lg %lg %u %u %u %u %u %lg %lg %lg %lg %lg %lg\n", 
		   &S, &N, &Y, &Z, &a, &b, &rho, 
		   &Asize, &Asumtrue, &Asumobs, &Bsize, &Csize,
		   &alpha, beta, beta+1, beta+2, &lambda, &gamma);
  assert(ctr == 18);
}

void 
PredInfo::calc_prob(unsigned t, unsigned o) 
{
    dst = t;
    dso = o;
    if (t > 0)
	tp = 1.0;
    else {
	if (o == 0) {
	    double numerator = gamma * exp(-lambda*rho-lambda*alpha*(1.0-rho)) + (1.0-gamma);
	    double denom = gamma*exp(-lambda*rho) + (1.0-gamma);
	    tp = 1.0- numerator/denom;
	}
	else {
            tp = 1.0 - exp(-lambda*alpha*(1.0-rho));
	}
    }
}

void
PredInfo::calc_prob_n(unsigned t, unsigned o, unsigned n)
{
  if (t > 0)
    tp_n = 1.0;
  else 
    tp_n = 1.0 - exp((double) (n-o) * log(1.0-alpha));
}

void
PredInfo::calc_prob2(unsigned t, unsigned o)
{
    if (t > 0)
	tp2 = 1.0;
    else {
	double numer, denom;
	if (o > 0) { // t = 0, m > 0
	    numer = beta[0]*exp((double)o*log(1.0-alpha) - lambda*alpha*(1.0-rho)) + beta[1];
	    denom = beta[0]*exp((double)o*log(1.0-alpha)) + beta[1];
	}
	else { // t = 0, m = 0
	    numer = (beta[0]*exp(-lambda*alpha*(1.0-rho)) + beta[1] + beta[2]*exp(-lambda*(1.0-rho))) * gamma * exp(-lambda*rho) + 1.0 - gamma;
	    denom = gamma*exp(-lambda*rho) + 1.0 - gamma;
	}
	tp2 = 1.0 - numer / denom;
    }
}

void
PredInfo::calc_prob_n2(unsigned t, unsigned o, unsigned n)
{
    if (t > 0)
	tp_n2 = 1.0;
    else { // t = 0, n > 0
	double numerator = beta[0]*exp((double)n*log(1.0-alpha)) + beta[1]; 
	if (o > 0) { // t = 0, m > 0, n > 0
	    double denominator = beta[0]*exp((double)o*log(1.0-alpha)) + beta[1];
	    tp_n2 = 1.0 - numerator/denominator;
	}
	else // t = 0, m = 0, n > 0
	    tp_n2 = 1.0 - numerator;
    }
}

class PredInfoPair;
typedef PredInfo (PredInfoPair::* piptr);

class PredInfoPair
{
public:
    PredInfo f;
    PredInfo s;
};

class pred_hash_t : public hash_map<PredCoords, PredInfoPair>
{
public:
   void reinit_probs(piptr pp) {
     for (pred_hash_t::iterator c = begin(); c != end(); ++c) {
       PredInfoPair &PP = c->second;
       (PP.*pp).reinit_probs();
     }
   }
};

static pred_hash_t predHash;
static piptr pptr = 0;

inline ostream &
operator<< (ostream &out, const PredInfo &pi)
{
    out << pi.tru << ' ' << pi.obs << ' ' << pi.rho << ' ' 
	<< pi.alpha << ' ' << pi.beta[0] << ' ' << pi.beta[1] << ' ' << pi.beta[2] << ' ' 
	<< pi.lambda << ' ' << pi.gamma << ' '
	<< pi.tp << ' ' << pi.tp_n << ' ' 
	<< pi.tp2 << ' ' << pi.tp_n2 << ' '
        << pi.dst << ' ' << pi.dso << ' '
	<< pi.realt << ' ' << pi.realo;
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
enum { READ_DS, READ_FULL };

class Reader : public ReportReader
{
public:
  Reader(int);

protected:
  void handleSite(const SiteCoords &, vector<unsigned> &);

private:
  void update(const SiteCoords &, unsigned, unsigned, unsigned) const;
  const int mode;
};

inline
Reader::Reader(int _mode)
    : mode(_mode)
{
}

inline void
Reader::update(const SiteCoords &coords, unsigned p, unsigned obs, unsigned tru) const
{
    assert(tru <= obs);
    PredCoords pc(coords, p);
    const pred_hash_t::iterator found = predHash.find(pc);
    if (found != predHash.end()) {
	PredInfoPair &pp = found->second;
	switch (mode) {
	case READ_DS:
	    (pp.*pptr).calc_prob(tru, obs);
	    (pp.*pptr).calc_prob2(tru, obs);
	    break;
	case READ_FULL:
	    (pp.*pptr).compare_prob(tru, obs);
	    break;
	default:
	    assert(0);
	}
    }
}

void Reader::handleSite(const SiteCoords &coords, vector<unsigned> &counts)
{
    const unsigned sum = accumulate(counts.begin(), counts.end(), 0);
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
 * Utilities
 ***************************************************************************/

void
read_parms()
{
  FILE *fp = fopenRead("hyperparms.txt");
  if (!fp) {
    cerr << "Cannot open hyperparms.txt for reading\n";
    exit(1);
  }

  PredCoords coords;
  PredInfoPair pp;
  int ctr;
  
  while (true) {
      ctr = fscanf(fp, "%u\t%u\t%u\n", 
		   &coords.unitIndex, &coords.siteOffset, &coords.predicate);
      if (feof(fp))
	  break;
      assert(ctr == 3);

      pp.f.set_parms(fp);
      pp.s.set_parms(fp);

      predHash[coords] = pp;
  }

  fclose(fp);
}

void calc_zero_prob(piptr pp) // calculate truthprobs of predicates we didn't see in this previous run
{
  for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
    PredInfoPair &PP = c->second;
    if ((PP.*pp).dso == 0) // it is zero if the predicate hasn't been observed in the previous run
    {
	(PP.*pp).calc_zero_prob();
	(PP.*pp).calc_zero_prob2();
    }
    
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

    read_parms();

//     Progress::Bounded prog("Reading runs to calculate alpha", NumRuns::count());
//     for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
// 	prog.step();
// 	if (is_srun[r])
// 	    pptr = &PredInfoPair::s;
// 	else if (is_frun[r])
// 	    pptr = &PredInfoPair::f;
// 	else
// 	    continue;

// 	Reader(CALC_ALPHA).read(r);
//     }

//    calc_alpha ();

    string report_suffix = CompactReport::suffix;
    unsigned ctr = 0;
    Progress::Bounded prog2("Calculating truth probabilities", ntestruns);
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
	if (ctr == ntestruns)
	    break;

	if (is_srun[r])
	    pptr = &PredInfoPair::s;
	else if (is_frun[r])
	    pptr = &PredInfoPair::f;
	else
	    continue;

	prog2.step();
	ctr++;
        predHash.reinit_probs(pptr);

	CompactReport::suffix = report_suffix;
	Reader(READ_DS).read(r);
	calc_zero_prob(pptr);

	CompactReport::suffix = "";
	Reader(READ_FULL).read(r);

        print_results(r);
    }


    logfp << "Model 1 score: " << score1 << " Given n: " << score_n1 <<endl;
    logfp << "Model 2 score: " << score2 << " Given n: " << score_n2 << endl;
    logfp.close();
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
