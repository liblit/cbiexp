/**********************************************************************
 * Perform bi-clustering on the set of pre-filtered predicates using
 * the posterior truth probabilities computed by get_to_weights.cc.
 * Inputs:
 * - s.runs, f.runs: record of outcome status
 * - preds.txt: list of interesting predicates
 * - the adjacency matrices between runs and predicates and predicate
 *   complements, defaults are X.dat and notX.dat
 * Output:
 * - bicluster_votes.xml: list of predicates ranked by the number of failed
 *                    runs they account for
 * - bicluster_preds.xml: top predicate for each run cluster as determined
 *        by two-sample t-testing (not nec. same as bicluster_votes.xml);
 *        run cluster i is the set of runs that voted for the ith most
 *        popular predicate 
 * - bicluster_votes.log, qualities.log, notQualities.log: log files
 *
 **********************************************************************/

#include <argp.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include "IndexedPredInfo.h"
#include "NumRuns.h"
#include "AdjWeightsOptions.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "RunBugs.h"
#include "RunsDirectory.h"
#include "Score/Importance.h"
#include "XMLTemplate.h"
#include "classify_runs.h"
#include "fopen.h"
#include "sorts.h"
#include "utils.h"

using namespace std;

const unsigned MAX_ITER = 50; // maximum number of voting updates
const double thresh = 1e-6; // threshold for convergence test
const double smooth = 1.0; // smoothing parameters used in quality score updates

const double smooth2 = 1e-9;

typedef list<IndexedPredInfo> Stats;
static Stats predList;
static RunList * contribRuns; // list of failed runs that voted for each predicate
static int nselected = 0; // number of predicates that got one or more votes
static map<unsigned, unsigned> finalVotes; // finalVotes[r] = p if run r cast its final vote for predicate p (indexed according to preds.txt)

static double * W; // W_{ij} is prob that predicate j is true more than once in run i
static double * notW; // same as above, but for complement of pred j
static double * run_weights; // run_weights[i] is the row sum of W_i
static double * notrun_weights; // row sum of notW
static double * pred_scores[2]; // vector of quality scores for each predicate, pred_scores[F][i] is the portion of Q_i that came from the failed runs, pred_scores[S][i] is the portion that came from the successes

static ofstream logfp ("bicluster_votes.log", ios_base::trunc); // log file
static ofstream qualities_log ("qualities.log", ios_base::trunc); // quality scores for each predicate
static ofstream not_qualities_log ("notQualities.log", ios_base::trunc); // quality scores for complement predicates

enum { F, S };

enum { L0, L1, L2, MAX};

////////////////////////////////////////////////////////////////////////
//
//  Utilities
//

// Normalize a vector of doubles by dividing by its L1, L2, or L_inf
// sum.  (L0 normalization is not implemented.)
void 
normalize (double * u, unsigned n, unsigned mode) 
{
  double norm = 0.0;
  for (unsigned i = 0; i < n; ++i) {
    switch (mode) {
    case L1:
      norm += abs(u[i]);
      break;
    case L2:
      norm += u[i] * u[i];
      break;
    case MAX:
      norm = (norm > abs(u[i])) ? norm : abs(u[i]);
      break;
    default:
      cerr << "Unknown mode for normalization: " << mode << endl;
      assert(0);
    }
  }

  for (unsigned i = 0; i < n; ++i) {
    u[i] = u[i] / norm;
  }
}

// Compute the sq euclidean distance between two vectors
double
sq_l2dist(const double *u, const double * v, const unsigned n)
{
  double change = 0.0;
  double diff;
  for (unsigned i = 0; i < n; ++i) {
    diff = u[i] - v[i];
    change += diff*diff;
  }

  return change;
}

// Calculate the contribution of predicate k towards run j
// W_data is the adjacency matrix, k is the predicate index, j is the row index
// of the run in the adjacency matrix, is_s denotes whether or not the run is 
// a success, v is the current quality score
inline double
contrib(const double *W_data, const unsigned k, const unsigned j, 
	const unsigned is_s, const double *v, const unsigned npreds) 
{
  double Akj = W_data[j*npreds+k];
  double vk = (is_s) ? 1.0/v[k] : v[k];
  
  return (Akj * vk);
}

////////////////////////////////////////////////////////////////////////
//
//  Input and output utilities
//

// Read in the list of interesting predicates from preds.txt
void 
read_preds()
{

  FILE * const pfp = fopenRead(PredStats::filename);
  pred_info pi;
  unsigned index = 0;
  while (read_pred_full(pfp, pi)) {
    IndexedPredInfo indexed(pi, index++);
    predList.push_back(indexed);
  }
  fclose(pfp);

  contribRuns = new RunList[predList.size()];
}

// Read the adjacency weight matrices W and notW, also computes row sums 
// run_weights and notrun_weights
void 
read_weights()
{
  unsigned npreds = predList.size();
  unsigned nruns = num_sruns + num_fruns;
  double x, notx;

  W = new double[nruns*npreds];
  notW = new double[nruns*npreds];
  run_weights = new double[nruns];
  notrun_weights = new double[nruns];

  memset(W, 0, sizeof(double)*nruns*npreds);
  memset(notW, 0, sizeof(double)*nruns*npreds);
  memset(run_weights, 0, sizeof(double)*nruns);
  memset(notrun_weights, 0, sizeof(double)*nruns);

  FILE * xfp = fopenRead(AdjWeightsOptions::adj_weights);
  FILE * notxfp = fopenRead(AdjWeightsOptions::not_adj_weights);
  int ctr;
  unsigned i = 0;
  for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    if (!is_srun[r] && !is_frun[r])  // skip the runs that were neither successful nor failing
      continue;
    
    for (unsigned j = 0; j < npreds; ++j) {
      ctr = fscanf(xfp, "%lg ", &x);
      assert(ctr == 1);
      ctr = fscanf(notxfp, "%lg ", &notx);
      assert(ctr == 1);
      assert(x >= 0.0);
      assert(notx >= 0.0);
      W[i*npreds+j] = x;
      notW[i*npreds+j] = notx;
      run_weights[i] += W[i*npreds+j];
      notrun_weights[i] += notW[i*npreds+j];
    }
    fscanf(xfp, "\n");
    fscanf(notxfp, "\n");

    ++i;
  }
  
  fclose (xfp);
  fclose (notxfp);
}

// Read the frequency of num_tru/num_obs in downsampled data
// recorded in the file truFreq.dat
// This can be used in ttest_rank() to sort the predicates for each run cluster
void
read_freqs ()
{
  unsigned npreds = predList.size();
  double w;
  FILE * wfp = fopenRead("truFreq.dat");
  int ctr; 
  unsigned i = 0;
  for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    if (!is_srun[r] && !is_frun[r])
      continue;
    for (unsigned j = 0; j < npreds; ++j) {
      ctr = fscanf(wfp, "%lg ", &w);
      assert(ctr == 1);
      W[i*npreds+j] = w;
    }
    fscanf(wfp, "\n");
    ++i;
  }
  fclose(wfp);
}

// Print the bug histogram of the list of interesting predicates
void
print_histograms()
{
  RunBugs::BugVec bugs;

  RunBugs::read_runbugs();
  ofstream outfp ("pred_histograms.xml");
  outfp << "<?xml version=\"1.0\"?>" << endl
        << "<!DOCTYPE histograms SYSTEM \"histograms.dtd\">" << endl
        << "<histograms>" << endl;

  unsigned npreds = predList.size();
  for (unsigned i = 0; i < npreds; ++i) {
    RunBugs::bug_hist(bugs, contribRuns[i]);
    outfp << "<predictor>" << endl;
    for (unsigned j = 0; j < bugs.size(); ++j) {
      outfp << "<bug count=\"" << bugs[j] << "\"/>" << endl; 
    }
    outfp << "</predictor>" << endl;
  }
  
  outfp << "</histograms>" << endl;
  outfp.close();
}

// Print which run voted for which predicate
static void
print_final_votes(const char outfile[])
{
  ofstream out(outfile);
  for (map<unsigned, unsigned>::const_iterator vote = finalVotes.begin(); vote != finalVotes.end(); ++vote)
    out << vote->first << '\t' << vote->second << '\n';
}


// Print the bi-clustering ranked list of predicates
void
print_scores(const char *outfn)
{
  // sort according to descending order
  predList.sort(Sort::Descending<Score::Importance>());

  ofstream outfp (outfn);
  outfp << "<?xml version=\"1.0\"?>" << endl
        << "<?xml-stylesheet type=\"text/xsl\" href=\"" 
	<< XMLTemplate::format("pred-scores") << ".xsl\"?>" << endl
	<< "<!DOCTYPE scores SYSTEM \"pred-scores.dtd\">" << endl
	<< "<scores>" << endl;

  for (Stats::iterator c = predList.begin(); c != predList.end(); ++c) {
    outfp << "<predictor index=\"" << (*c).index+1
          << "\" score=\"" << setprecision(10) << (*c).ps.imp
          << "\" stat1=\"" << (*c).ps.fdenom
	  << "\" stat2=\"" << (*c).ps.sdenom
	  << "\"/>" << endl;
    outfp.unsetf(ios::floatfield);
  }

  outfp << "</scores>" << endl;
  outfp.close();
}

// Print a vector of doubles to the log file
void 
logvalues (const double * v, const unsigned n) {
  for (unsigned i = 0; i < n; ++i) {
    logfp << v[i] << ' ';
  }
  logfp << endl;
}

// Output to the log file the lists of runs that voted for the top n predicates
void 
logruns (const unsigned n) {
  for (unsigned i = 0; i < n; ++i) {
    if (! contribRuns[i].empty()) {
      logfp << "Pred ind " << i+1 << " Runs: " << endl;
      for (RunList::const_iterator c = contribRuns[i].begin(); 
           c != contribRuns[i].end(); ++c) {
        logfp << (*c) << ' ';
      }
      logfp << endl;
    }
  }
}

////////////////////////////////////////////////////////////////////////
//
//  Computation routines
//

// For run j, figure out which predicate deserves the vote, and maintain
// records
// j is the row index of the run in the adjacency matrix, r is the actual
// run index, the rest of the arguments are as in cast_finalvote()
inline void
update_max(const unsigned j, const unsigned r, const unsigned npreds,
           double *v, double *notv,
           double oldpvotes[][2], double oldnotpvotes[][2],
           double pvotes[][2], double notpvotes[][2])
{
  double Aij, notAij; // adjacency weights between the best pred and run j, and the best pred's complement and run j
  double c = 0.0; // contribution of a single predicate to run j
  double notc = 0.0; // contribution of the complement of that pred to run j
  unsigned argmax = 0; // index of best predicate
  double maxd = 0.0;   // record of maximum contribution
  double val = 0.0;
  unsigned type = (is_srun[r]);
  unsigned othertype = !type;
  for (unsigned k = 0; k < npreds; ++k) {
    c = contrib(W, k, j, type, v, npreds);
    notc = contrib(notW, k, j, type, notv, npreds);
    // the weighted contribution of the current predicate towards run j
    val = (c == 0) ? 0 : (c + smooth2) / (notc + smooth2)
          * (oldnotpvotes[k][othertype]+ smooth)/(oldpvotes[k][othertype]+ smooth);
    if (val > maxd) {
      maxd = val;
      argmax = k;
    }
  }

  Aij = W[j*npreds+argmax];
  notAij = notW[j*npreds+argmax];
  pvotes[argmax][type] += Aij / run_weights[j];
  notpvotes[argmax][type] += notAij / notrun_weights[j];
  if (is_frun[r])
    contribRuns[argmax].push_back(r);

  finalVotes[r] = argmax;
}

// Each run casts final vote for the predicate with the highest overall contribution
// u and notu are final scores for each predicate (number of votes from failed
// runs), v and notv are previous quality scores, oldpvotes and oldnotpvotes 
// are broken-down proportions of votes from failed and successful runs
void
cast_finalvote(double * u, double * notu, double * v, double * notv,
            double oldpvotes[][2], double oldnotpvotes[][2],
            unsigned npreds)
{
  double pvotes[npreds][2], notpvotes[npreds][2];

  memset(pvotes, 0, sizeof(double)*2*npreds);
  memset(notpvotes, 0, sizeof(double)*2*npreds);
  memset(u, 0, sizeof(double)*npreds);
  for (unsigned i = 0; i < npreds; ++i) {
    contribRuns[i].clear();
  }

  cout << "Giving weight to max pred..." << endl;

  finalVotes.clear();

  unsigned j = 0;
  for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    if (!is_srun[r] && !is_frun[r])
      continue;

    update_max(j, r, npreds, v, notv, oldpvotes, oldnotpvotes, pvotes, notpvotes);

    j++;
  }

  for (unsigned i = 0; i < npreds; ++i) {
    pred_scores[F][i] = (pvotes[i][F] + smooth)/(notpvotes[i][F] + smooth);
    pred_scores[S][i] = (notpvotes[i][S] + smooth)/(pvotes[i][S] + smooth);
    u[i] = contribRuns[i].size();
    notu[i] = 1.0/u[i];
    if (u[i] > 0.0) 
      nselected++;
  }

  //copy over from u to v 
  memcpy(v, u, sizeof(double)*npreds);
  // logvalues (v, npreds);
  logruns (npreds);
  //logfp << "cast_finalvote: iteration: " << ctr << " change: " << change << endl;
}

// update the sums of votes from run j
// j is the run's row index in the adjacency matrix, is_s denotes whether 
// or not the run is a success, v and notv are current quality scores, 
// pvotes and notpvotes are voting summaries to be updated 
inline void
update_votes(const unsigned j, const unsigned is_s, const unsigned npreds,
           double *v, double *notv,
           double pvotes[][2], double notpvotes[][2])
{
  double Aij, notAij;
  double contribj = 0.0; // contribution towards run j from all predicates
  double cij = 0.0; // contribution towards run j from predicate i
  double notp_contribj = 0.0; // contribution towards run j from all predicate complements
  double notp_cij = 0.0; // contribution towards run j from pred i's complement
  for (unsigned k = 0; k < npreds; ++k) {
    contribj += contrib(W, k, j, is_s, v, npreds); 
    notp_contribj += contrib(notW, k, j, is_s, notv, npreds);
  }
  for (unsigned i = 0; i < npreds; ++i) {
    if (contribj != 0 && run_weights[j] != 0) {
        Aij = W[j*npreds+i];
        cij = contrib(W,i,j,is_s,v,npreds) / contribj / run_weights[j];
        pvotes[i][is_s] += Aij * (1.0 - cij);
    }

    if (notp_contribj != 0 && notrun_weights[j] != 0) {
    	notAij = notW[j*npreds+i];
    	notp_cij = contrib(notW,i,j,is_s,notv,npreds) / notp_contribj / notrun_weights[j];
    	notpvotes[i][is_s] += notAij * (1.0 - notp_cij);
    }
 }
}

// Iterate the voting and quality score updates until convergence.
// u and notu hold the current quality scores for the predicates and
// their complements; v and notv hold the previous ones
void
iterate_votes(double * u, double * notu, double * v, double * notv,
            double pvotes[][2], double notpvotes[][2],
            unsigned npreds)
{
  double change;
  unsigned ctr = 0;
  unsigned is_s;

  // initialize quality scores to uniform
  for (unsigned i = 0; i < npreds; ++i) {
    v[i] = 1.0;
    notv[i] = 1.0;
  }

  Progress::Bounded progress("Iterating sum updates", MAX_ITER);
  do {
    progress.step();
    ctr++;
    memset(pvotes, 0, sizeof(double)*2*npreds);
    memset(notpvotes, 0, sizeof(double)*2*npreds);
    memset(u, 0, sizeof(double)*npreds);
    unsigned j = 0;
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
      if (!is_srun[r] && !is_frun[r])
	continue;

      is_s = is_srun[r];
      update_votes(j, is_s, npreds, v, notv, pvotes, notpvotes);
      j++;
    }

    for (unsigned i = 0; i < npreds; ++i) {
      u[i] = (pvotes[i][F] + smooth) / (pvotes[i][S] + smooth) 
	* (notpvotes[i][S] + smooth) / (notpvotes[i][F] + smooth);
      notu[i] = 1.0 / u[i];
      qualities_log << u[i] << ' ';
      not_qualities_log << notu [i] << ' '; 
      logfp << "Predicate: " << i << " Q: " << u[i] << " notQ: " << notu[i] << endl;
    }
    qualities_log << endl;
    not_qualities_log << endl;

    normalize(u, npreds, MAX);
    normalize(notu, npreds, MAX);

    change = sq_l2dist(u, v, npreds) + sq_l2dist(notu, notv, npreds);
    //copy over from u to v 
    memcpy(v, u, sizeof(double)*npreds);
    memcpy(notv, notu, sizeof(double)*npreds);
    // logvalues (v, npreds);
    logfp << "iterate_votes: iteration: " << ctr << " change: " << change << endl;
  } while (change > thresh && ctr < MAX_ITER);
}

// This is the main driver for the bi-clustering algorithm.
// It calls iterate_votes() to iterate the voting and quality score updates,
// then it calls cast_finalvote() to decide which predicate accounts for which
// run.
void
compute_scores()
{
  unsigned npreds = predList.size();
  double pvotes[npreds][2];
  double notpvotes[npreds][2];
  double v[npreds], u[npreds];
  double notv[npreds], notu[npreds];
  pred_scores[F] = new double[npreds];
  pred_scores[S] = new double[npreds];
  memset(pred_scores[F], 0, sizeof(double)*npreds);
  memset(pred_scores[S], 0, sizeof(double)*npreds);

  // voting algorithm
  iterate_votes(u, notu, v, notv, pvotes, notpvotes, npreds);
  cast_finalvote(u, notu, v, notv, pvotes, notpvotes, npreds);

  // record stats that will be output in bicluster_votes.xml
  unsigned i = 0;
  for (Stats::iterator c = predList.begin(); c != predList.end(); ++c) {
    (*c).ps.fdenom = pred_scores[F][i];
    (*c).ps.sdenom = pred_scores[S][i];
    (*c).ps.imp = v[i++];
  }

}

// Collect means and variances of the predicates on a subset of runs 
// specified by the vector COLLECT
unsigned
collect_stats(double *m, double *v, unsigned npreds, const vector<bool> &collect)
{
  cout << "Collecting statistics ..." << endl;
  memset(m, 0, sizeof(double)*npreds);
  memset(v, 0, sizeof(double)*npreds);
  double val;
  unsigned n = 0, j = 0; 
  for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    if (!is_srun[r] && !is_frun[r])
      continue;
    if (collect[r]) {
      n++;
      for (unsigned i = 0; i < npreds; ++i) {
        val = W[j*npreds+i];
        m[i] += val;
        v[i] += val*val;
      }
    }
    j++;
  }

  for (unsigned i = 0; i < npreds; ++i) {
    if (n > 0)
      m[i] /= (double) n;
    if (n > 1)
      v[i] = (v[i] - (double) n*m[i]*m[i])/(double) (n-1.0);
    else v[i] = 0.0;
  }

  logfp << n << " number of runs used." << endl;
  logfp << "Truth prob means" << endl;
  logvalues(m, npreds);
  logfp << "Truth prob vars" << endl;
  logvalues(v, npreds);

  return n;
}

// Compute the T statistic for the two-sample T-test
bool
compute_tstats(double *tstat, double *m1, double *s1, 
               double *m2, double *s2, double n1,
               double n2, unsigned len)
{
  cout << "n1: " << n1 << " n2: " << n2 << endl;
  if (n1 < 1.0 || n2 < 1.0 || n1+n2 < 2.0) {
    return false;
  }

  double c1;
  double c2;
  double sigma;
  c1 = sqrt(1.0/n1 + 1.0/n2);
  c2 = sqrt(n1+n2-2.0);
  for (unsigned i = 0; i < len; ++i) {
    sigma = sqrt(s1[i]*(n1-1.0) + s2[i]*(n2-1.0));
    tstat[i] = m2[i] - m1[i];
    if (tstat[i] != 0.0) // make sure the numerator is not zero
      tstat[i] = tstat[i] / sigma * c2 / c1;
  }
  logfp << "T-test stats" << endl;
  logvalues(tstat, len);
  return true;
}

// do two-sample t-test to determine the ranking of predicates (i.e., cluster
// predicates) for each run cluster
void
ttest_rank()
{
  unsigned npreds = predList.size();
  double smean[npreds], svar[npreds];
  double fmean[npreds], fvar[npreds];
  double tstat[npreds];
  vector<bool> collect;
  vector<unsigned> check;
  unsigned ns, nf;
  unsigned maxlen = nselected, ctr = 0;
  char filename[128];

  //read_freqs();  // read in the real dst/dso frequencies

  // bicluster_preds.xml contains the best predicate for each run cluster
  // as determined by the t-test
  ofstream bestfp ("bicluster_preds.xml");
  bestfp << "<?xml version=\"1.0\"?>" << endl
         << "<?xml-stylesheet type=\"text/xsl\" href=\"" 
	 << XMLTemplate::format("pred-scores") << ".xsl\"?>" << endl
	 << "<!DOCTYPE scores SYSTEM \"pred-scores.dtd\">" << endl
	 << "<scores>" << endl;
  
  // do two-sample t-test for the failed runs accounted for by 
  // the top MAXLEN predicates
  check.resize(maxlen);
  for (Stats::iterator c = predList.begin(); c != predList.end(); ++c) {
    check[ctr] = c->index;
    if (++ctr >= maxlen)
      break;
  }

  // first run t-test on all predicates and runs (this is the approach of,
  // e.g., sober)
  cout << "num_sruns: " << num_sruns << " num_fruns: " << num_fruns << endl;
  ns = collect_stats(smean, svar, npreds, is_srun);
  assert(ns == num_sruns);
  nf = collect_stats(fmean,fvar, npreds, is_frun);
  assert(nf == num_fruns);
  if (compute_tstats(tstat, smean, svar, fmean, fvar, ns, nf, npreds)) {
    for (Stats::iterator c = predList.begin(); c != predList.end(); ++c)
      c->ps.imp = tstat[c->index];
    print_scores("list0.xml");
  }

  for (unsigned i = 0; i < maxlen; ++i) {
    unsigned pind = check[i];
    if (contribRuns[pind].size() == 0)
      continue;
    collect.clear(); // collect is a binary vector that contains a 1 for every failed run that voted for the current predicate
    collect.resize(NumRuns::end);
    for (RunList::const_iterator c = contribRuns[pind].begin();
         c != contribRuns[pind].end(); ++c) {
      collect[(*c)] = 1;
    }
    nf = collect_stats(fmean, fvar, npreds, collect); // collect statistics on the failed runs that voted for this predicate
    if (compute_tstats(tstat, smean, svar, fmean, fvar, ns, nf, npreds)) {
      for (Stats::iterator c = predList.begin(); c != predList.end(); ++c) 
        c->ps.imp = tstat[c->index];
      sprintf(filename, "list%d.xml", i+1);
      print_scores(filename); // this modifies the ordering of predicates in predList
    }

    // output the first ranked predicate to the final list
    bestfp << "<predictor index=\"" << predList.front().index+1
           << "\" score=\"" << setprecision(10) << predList.front().ps.imp
           << "\" stat1=\"" << predList.front().ps.fdenom
           << "\" stat2=\"" << predList.front().ps.sdenom
           << "\"/>" << endl;
    bestfp.unsetf(ios::floatfield);
  }

  bestfp << "</scores>" << endl;
  bestfp.close();
}

////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//

static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ &XMLTemplate::argp, 0, 0, 0},
	{ &AdjWeightsOptions::argp, 0, 0, 0},
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
};

////////////////////////////////////////////////////////////////
//
// The main function
//

int main (int argc, char** argv)
{
  process_cmdline (argc, argv);
  
  classify_runs();

  read_preds();

  if (predList.size() > 0) {
    read_weights();

    compute_scores();

    // must print out bug histograms before the scores,
    // because print_scores alters the ordering of predicates
    // through sorting
    if (XMLTemplate::prefix == "moss")
      print_histograms();
  
    print_final_votes("final-votes.txt"); // print which run voted for which predicate
    print_scores("bicluster_votes.xml"); // print the final bi-clustering ranking
  
    ttest_rank(); // run two-sample t-test to obtain final ranking of predicates within each run cluster
    delete[] W;
    delete[] notW;
    delete[] run_weights;
    delete[] notrun_weights;
    delete[] pred_scores[F];
    delete[] pred_scores[S];
  }
  else {
    print_scores("bicluster_votes.xml");  // print out skeleton pred_scores.xml file
  }

  logfp.close();
  qualities_log.close();
  not_qualities_log.close();
  delete[] contribRuns;
  return 0;
}
