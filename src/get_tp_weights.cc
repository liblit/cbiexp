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

bool full = 0;
static int score_ctr = 0;
static double score2 = 0.0, score_n2 = 0.0;
static double score3 = 0.0;
static ofstream datfp("X.dat", ios_base::trunc);
static ofstream notdatfp("notX.dat", ios_base::trunc);
static ofstream gdatfp("truX.dat", ios_base::trunc);
static ofstream gnotdfp("trunotX.dat", ios_base::trunc);
static ofstream freqfp("truFreq.dat", ios_base::trunc);
static ofstream nfreqfp("trunotFreq.dat", ios_base::trunc);

static vector<PredCoords> predVec;
static vector<PredCoords> notpredVec;

/****************************************************************
 * Predicate-specific information
 ***************************************************************/

struct PredInfo
{
    count_tp tru, obs;
    double rho;
    double alpha; // truth rate = tru/obs
    double beta[3];
    double lambda, gamma;
    double tp2, tp_n2; // truth probabilities from model 2
    count_tp dst, dso, realt, realo;
    PredInfo() {
	tru = obs = 0;
	rho = alpha = beta[0] = beta[1] = beta[2] = lambda = gamma = 0.0;
	tp2 = tp_n2 = 0.0;
	dst = dso = realt = realo = 0;
    }
    void reinit_probs() {
	tp2 = tp_n2 = 0.0;
	dst = dso = realt = realo = 0;
    }
    void update_stats(count_tp _tru, count_tp _obs) {
	tru += _tru;
	obs += _obs;
    }
    void set_parms(FILE *fp);
    void calc_prob2(count_tp t, count_tp o);
    void calc_prob_n2(count_tp t, count_tp o, count_tp n);
    void calc_zero_prob2() {  calc_prob2(0,0);    }
    void record_vals (count_tp t, count_tp o) {
        realt = t;
        realo = o;
    }
    void compare_prob (count_tp t, count_tp o) {
        realt = t;
        realo = o;
	calc_prob_n2(dst, dso, realo);
        double answer = (realt > 0) ? 1.0 : 0.0;
	double obstru = (dst > 0) ? 1.0 : 0.0;
        score2 += abs(answer - tp2);
        score_n2 += abs(answer - tp_n2);
	score3 += abs(answer - obstru);
    }
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

void
PredInfo::calc_prob2(count_tp t, count_tp o)
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
        if (tp2 < 0.0) 
          tp2 = 0.0;
    }
}

void
PredInfo::calc_prob_n2(count_tp t, count_tp o, count_tp n)
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
enum { READ_DS, READ_FULL, RECORD_FULL };

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

inline void
Reader::update(const SiteCoords &coords, unsigned p, count_tp obs, count_tp tru) const
{
    assert(tru <= obs);
    PredCoords pc(coords, p);
    const pred_hash_t::iterator found = predHash.find(pc);
    if (found != predHash.end()) {
	PredInfoPair &pp = found->second;
	switch (mode) {
	case READ_DS:
	    (pp.*pptr).dst = tru;
	    (pp.*pptr).dso = obs;
	    (pp.*pptr).calc_prob2(tru, obs);
	    break;
	case READ_FULL:
            score_ctr++;
	    (pp.*pptr).compare_prob(tru, obs);
	    break;
        case RECORD_FULL:
            score_ctr++;
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
 * Utilities
 ***************************************************************************/

PredCoords
notP (PredCoords &p) {
  PredCoords notp(p);
  if (p.predicate % 2 == 0) {
    notp.predicate = p.predicate + 1;
  }
  else {
    notp.predicate = p.predicate - 1;
  }

  return notp;
}

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

void
read_parms()
{
  FILE *fp = fopenRead("hyperparms.txt");
  FILE *notpfp = fopenRead("notp-hyperparms.txt");
  if (!fp) {
    cerr << "Cannot open hyperparms.txt for reading\n";
    exit(1);
  }
  if (!notpfp) {
    cerr << "Cannot open notp-hyperparms.txt for reading\n";
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

      // read info for predicate ~p
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

void calc_zero_prob(piptr pp) // calculate truthprobs of predicates we didn't see in this previous run
{
  for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
    PredInfoPair &PP = c->second;
    if ((PP.*pp).dso == 0) // it is zero if the predicate hasn't been observed in the previous run
    {
	(PP.*pp).calc_zero_prob2();
    }
    
  }
}

/****************************************************************************
 * Print results to file
 ***************************************************************************/
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
	  out << (found->second.*pptr).tp2 << ' ';
        }
    }
    return out;
}

void
print_groundtruth()
{
  double freq;
  for (unsigned i = 0; i < predVec.size(); ++i) {
    pred_hash_t::iterator found = predHash.find(predVec[i]);
    assert(found != predHash.end());
    gdatfp << (found->second.*pptr).dst << ' ';
    freq = 0.0;
    if ((found->second.*pptr).dso > 0)
      freq = (double) (found->second.*pptr).dst/(found->second.*pptr).dso;
    freqfp << freq << ' ';

    found = predHash.find(notpredVec[i]);
    assert(found != predHash.end());
    gnotdfp << (found->second.*pptr).dst << ' ';
    freq = 0.0;
    if ((found->second.*pptr).dso > 0)
      freq = (double) (found->second.*pptr).dst/(found->second.*pptr).dso;
    nfreqfp << freq << ' ';
  }
  gdatfp << endl;
  freqfp << endl;
  gnotdfp << endl;
  nfreqfp << endl;
}

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

    Progress::Bounded prog("Calculating truth probabilities", NumRuns::count());
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
	if (is_srun[r])
	    pptr = &PredInfoPair::s;
	else if (is_frun[r])
	    pptr = &PredInfoPair::f;
	else
	    continue;

	prog.step();
        predHash.reinit_probs(pptr);

        if (!full) { // looking at downsampled data
	  Reader(READ_DS).read(r);
	  calc_zero_prob(pptr);
  
	  CompactReport::suffix = "";
	  Reader(READ_FULL).read(r);
	  CompactReport::suffix = report_suffix;
        }
        else {
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
