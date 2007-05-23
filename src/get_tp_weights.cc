/****************************************************************
 * Calculate the posterior truth probabilities of each predicate
 * in each run based on estimated probabilistic model parameters.
 * If input runs are not downsampled, then just collect the ground
 * truth counts.
 * Input:
 * - parms.txt: estimated parameter values for all predicates in preds.txt
 * - notp-parms.txt: same as above but for pred complements
 * Output:
 * - X.dat: matrix of inferred truth probabilities (rows = runs, columsn = preds)
 * - notX.dat: same as above, but for the complement preds
 * - truX.dat: matrix of binary truth counts from downsampled runs
 * - trunotX.dat: same as above, but for the complement preds
 * - truFreq.dat: matrix of truth frequencies (num tru/num observed)
 * - trunotFreq.dat: same as above, but for the complement preds
 * 
 * -- documented by alicez 4/07
 ***************************************************************/

#include <argp.h>
#include <cassert>
#include <cmath>
#include <ext/hash_map>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <vector>
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


bool full = 0;  // whether we are looking at downsampled or full reports
static double score = 0.0, score_n = 0.0; // sum of absolute difference between inferred probs and full (binarized) counts
static double score_ds = 0.0; // sum of absolute difference between full and downsampled counts (binarized)
static int score_ctr = 0; // number of runs used to compute score
static ofstream datfp("X.dat", ios_base::trunc);
static ofstream notdatfp("notX.dat", ios_base::trunc);
static ofstream gdatfp("truX.dat", ios_base::trunc);
static ofstream gnotdfp("trunotX.dat", ios_base::trunc);
static ofstream freqfp("truFreq.dat", ios_base::trunc);
static ofstream nfreqfp("trunotFreq.dat", ios_base::trunc);

static vector<PredCoords> predVec;
static vector<PredCoords> notpredVec;

/****************************************************************
 * Storage for statistics on a single predicate in a single run
 * tru = observed truth count
 * obs = number of times observed
 * rho = predicate sampling rate
 * alpha = truth rate in P(X|N)
 * beta = mixture proportions in P(X|N)
 * lamba = Poisson parameter in P(N)
 * gamma = mixture proportion in P(N)
 * tp = inferred truth probability P(X > 0 | m, y)
 * tp_n = inferred truth probability P(X > 0 | n, m, y)
 * dst = truth count in downsampled run
 * dso = num times observed in downsampled run
 * realt = truth count in full run
 * realo = num times observed in full run
 ***************************************************************/

struct PredInfo
{
    count_tp tru, obs;
    double rho;
    double alpha; // truth rate = tru/obs
    double beta[3];
    double lambda, gamma;
    double tp, tp_n; // truth probabilities
    count_tp dst, dso, realt, realo;
    PredInfo() {
	tru = obs = 0;
	rho = alpha = beta[0] = beta[1] = beta[2] = lambda = gamma = 0.0;
	tp = tp_n = 0.0;
	dst = dso = realt = realo = 0;
    }
    void reinit_probs() {
	tp = tp_n = 0.0;
	dst = dso = realt = realo = 0;
    }
    void update_stats(count_tp _tru, count_tp _obs) {
	tru += _tru;
	obs += _obs;
    }
    void set_parms(FILE *fp);  // set parameter values according to file
    void calc_prob(count_tp t, count_tp o);
    void calc_prob_n(count_tp t, count_tp o, count_tp n);
    void calc_zero_prob() {  calc_prob(0,0);    }
    void record_vals (count_tp t, count_tp o) {
        realt = t;
        realo = o;
    }
    // compare ground truth against inferred probabilities
    void compare_prob (count_tp t, count_tp o) {
        realt = t;
        realo = o;
	calc_prob_n(dst, dso, realo);
        double realt_bin = (realt > 0) ? 1.0 : 0.0;
	double dst_bin = (dst > 0) ? 1.0 : 0.0;
        score += abs(realt_bin - tp);
        score_n += abs(realt_bin - tp_n);
	score_ds += abs(realt_bin - dst_bin);
    }
    void print_groundtruth (ostream &, ostream &);
};

void
PredInfo::set_parms (FILE * fp)
{
  count_tp S, Y;
  unsigned N, Z;
  double a, b;
  unsigned Asize, Bsize, Csize;
  count_tp Asumtrue, Asumobs;
  int ctr = fscanf(fp, "%Lu %u %Lu %u %lg %lg %lg %u %Lu %Lu %u %u %lg %lg %lg %lg %lg %lg\n", 
		   &S, &N, &Y, &Z, &a, &b, &rho, 
		   &Asize, &Asumtrue, &Asumobs, &Bsize, &Csize,
		   &alpha, beta, beta+1, beta+2, &lambda, &gamma);
  assert(ctr == 18);
}

// Calculate the posterior truth probability P(X | o, t)
// t = num times predicate observed to be true
// o = num times predicate observed
void
PredInfo::calc_prob(count_tp t, count_tp o)
{
    if (t > 0)
	tp = 1.0;
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
	tp = 1.0 - numer / denom;
        if (tp < 0.0)  // small numerical errors may cause tp to be negative
          tp = 0.0;
    }
}

// Calculate the posterior probability P(X > 0 | n, o, t)
// t and o same as in calc_prob(), n = num times predicate is reached
void
PredInfo::calc_prob_n(count_tp t, count_tp o, count_tp n)
{
    if (t > 0)
	tp_n = 1.0;
    else { // t = 0, n > 0
	double numerator = beta[0]*exp((double)n*log(1.0-alpha)) + beta[1]; 
	if (o > 0) { // t = 0, m > 0, n > 0
	    double denominator = beta[0]*exp((double)o*log(1.0-alpha)) + beta[1];
	    tp_n = 1.0 - numerator/denominator;
	}
	else // t = 0, m = 0, n > 0
	    tp_n = 1.0 - numerator;
    }
}

/****************************************************************
 * Storage for a pair of predicates (one for failed runs, one for
 * the successes)
 ***************************************************************/

class PredInfoPair;
typedef PredInfo (PredInfoPair::* piptr);

class PredInfoPair
{
public:
    PredInfo f;
    PredInfo s;
};

/****************************************************************
 * Hash array of a pair of predicates
 ***************************************************************/

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

// Calculate truthprobs of predicates we didn't see in this run
void calc_zero_prob(piptr pp) 
{
  for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
    PredInfoPair &PP = c->second;
    if ((PP.*pp).dso == 0) // it is zero if the predicate hasn't been observed in this run
    {
	(PP.*pp).calc_zero_prob();
    }
    
  }
}

/****************************************************************
 * Report reader, records and makes of of information from each run
 ***************************************************************/

enum { READ_DS, READ_FULL, RECORD_FULL }; // different reader modes

class Reader : public ReportReader
{
public:
  Reader(int);

protected:
  void handleSite(const SiteCoords &, vector<count_tp> &);

private:
  void update(const SiteCoords &, unsigned, count_tp, count_tp) const;
  const int mode;
};

inline
Reader::Reader(int _mode)
    : mode(_mode)
{
}

// record the right information from this run, according to reader mode
inline void
Reader::update(const SiteCoords &coords, unsigned p, count_tp obs, count_tp tru) const
{
    assert(tru <= obs);
    PredCoords pc(coords, p);
    const pred_hash_t::iterator found = predHash.find(pc);
    if (found != predHash.end()) {
	PredInfoPair &pp = found->second;
	switch (mode) {
	case READ_DS: // record down-sampled counts and calculate post prob
	    (pp.*pptr).dst = tru;
	    (pp.*pptr).dso = obs;
	    (pp.*pptr).calc_prob(tru, obs);
	    break;
	case READ_FULL: // compare against inferred prob
            score_ctr++;
	    (pp.*pptr).compare_prob(tru, obs);
	    break;
        case RECORD_FULL: // just record full counts
            (pp.*pptr).record_vals(tru, obs);
            break;
	default:
	    assert(0);
	}
    }
}

void Reader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
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
 * I/O utilities
 ***************************************************************************/

// Read and store the list of predicates in preds.txt, as well as the
// complements of those predicates.  This function is only called when
// we are looking at full (non-downsampled) data.
void
read_preds()
{
  queue<PredCoords> predQueue;
  queue<PredCoords> notpredQueue;
  PredInfoPair pp;
  pred_info pi;

  FILE * const pfp = fopenRead(PredStats::filename);
  while (read_pred_full(pfp,pi)) {
      predQueue.push(pi);
      predHash[pi] = pp;

      PredCoords notp = notP(pi);
      notpredQueue.push(notp);
      predHash[notp] = pp;
  }
  fclose(pfp);

  predVec.resize(predQueue.size());
  int i = 0;
  while (!predQueue.empty()) {
      predVec[i++] = predQueue.front();
      predQueue.pop();
  }

  notpredVec.resize(notpredQueue.size());
  i = 0;
  while (!notpredQueue.empty()) {
      notpredVec[i++] = notpredQueue.front();
      notpredQueue.pop();
  }

  assert(notpredVec.size() == predVec.size());
}

// Read in the estimated parameters of all interesting predicates
void
read_parms()
{
  FILE *fp = fopenRead("parms.txt");
  FILE *notpfp = fopenRead("notp-parms.txt");
  if (!fp) {
    cerr << "Cannot open parms.txt for reading\n";
    exit(1);
  }
  if (!notpfp) {
    cerr << "Cannot open notp-parms.txt for reading\n";
    exit(1);
  }

  queue<PredCoords> predQueue;
  queue<PredCoords> notpredQueue;
  PredCoords coords;
  PredInfoPair pp;
  int ctr;
  
  while (true) {
      // read info for predicate p
      ctr = fscanf(fp, "%u\t%u\t%u\n", 
		   &coords.unitIndex, &coords.siteOffset, &coords.predicate);
      if (feof(fp))
	  break;
      assert(ctr == 3);
      predQueue.push(coords);
      pp.f.set_parms(fp);
      pp.s.set_parms(fp);
      predHash[coords] = pp;

      // read info for the predicate's complement
      ctr = fscanf(notpfp, "%u\t%u\t%u\n", 
		   &coords.unitIndex, &coords.siteOffset, &coords.predicate);
      assert(ctr == 3);
      notpredQueue.push(coords);
      pp.f.set_parms(notpfp);
      pp.s.set_parms(notpfp);
      predHash[coords] = pp;

  }

  fclose(fp);
  fclose(notpfp);

  predVec.resize(predQueue.size());
  int i = 0;
  while (!predQueue.empty()) {
      predVec[i++] = predQueue.front();
      predQueue.pop();
  }

  notpredVec.resize(notpredQueue.size());
  i = 0;
  while (!notpredQueue.empty()) {
      notpredVec[i++] = notpredQueue.front();
      notpredQueue.pop();
  }

  assert(notpredVec.size() == predVec.size());
}

static ostream &
operator<< (ostream &out, const vector<PredCoords> &pv)
{
    for (unsigned i = 0; i < pv.size(); ++i) {
	pred_hash_t::iterator found = predHash.find(pv[i]);
	assert(found != predHash.end());
        if (full) {
          int t = (found->second.*pptr).realt > (count_tp) 0 ? 1 : 0;
          out << t << ' ';
        }
        else {
	  out << (found->second.*pptr).tp << ' ';
        }
    }
    return out;
}

// Print the groundtruth counts from the down-sampled runs
void 
PredInfo::print_groundtruth(ostream &gfp, ostream &ffp)
{
  double freq = (dso > 0) ? (double) dst/dso : 0.0;
  gfp << dst << ' ';
  ffp << freq << ' ';
  return;
}

// Wrapper function for PredInfo::print_groundtruth()
void
print_groundtruth()
{
  for (unsigned i = 0; i < predVec.size(); ++i) {
    pred_hash_t::iterator found = predHash.find(predVec[i]);
    assert(found != predHash.end());
    (found->second.*pptr).print_groundtruth(gdatfp, freqfp);

    found = predHash.find(notpredVec[i]);
    assert(found != predHash.end());
    (found->second.*pptr).print_groundtruth(gnotdfp, nfreqfp);
  }
  gdatfp << endl;
  freqfp << endl;
  gnotdfp << endl;
  nfreqfp << endl;
}

// print collected truth counts or probabilities, if we're doing
// probabilistic inference, also print groundtruth of downsampled
// counts to compare against the inferred probabilities later on
void print_results()
{
  datfp << predVec << endl;
  notdatfp << notpredVec << endl;
  if (!full)
    print_groundtruth();
}


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &CompactReport::argp, 0, 0, 0 },
	{ &NumRuns::argp, 0, 0, 0 },
	{ &ReportReader::argp, 0, 0, 0 },
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

    string report_suffix = CompactReport::suffix;
    full = report_suffix.empty();  // are we using non-downsampled data

    classify_runs();

    if (full) 
      read_preds();
    else 
      read_parms();

    datfp << scientific << setprecision(12);
    notdatfp << scientific << setprecision(12);
    freqfp << scientific << setprecision(12);
    nfreqfp << scientific << setprecision(12);

    Progress::Bounded prog("Calculating truth probabilities", NumRuns::count());
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
	if (is_srun[r])
	    pptr = &PredInfoPair::s;
	else if (is_frun[r])
	    pptr = &PredInfoPair::f;
	else
	    continue;

	prog.step();
        predHash.reinit_probs(pptr);  // reinitialize predicate probabilities for each new run

        if (!full) { // looking at downsampled data
	  Reader(READ_DS).read(r); // read in down-sampled counts and compute posterior truth probabilities for all observed predicates
	  calc_zero_prob(pptr); // calculate the posterior truth probability for predicates that were not observed
  
	  CompactReport::suffix = "";
	  Reader(READ_FULL).read(r);  // read in the full data for comparison
	  CompactReport::suffix = report_suffix;
        }
        else { // record full data
          Reader(RECORD_FULL).read(r);
        }

        print_results();
    }

    datfp.close();
    notdatfp.close();
    gdatfp.close();
    gnotdfp.close();
    freqfp.close();
    nfreqfp.close();
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
