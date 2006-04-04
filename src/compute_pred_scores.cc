#include <argp.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include "IndexedPredInfo.h"
#include "NumRuns.h"
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

const unsigned MAX_ITER = 20;
const double thresh = 10e-12;
const double smooth = 1.0;
const double smooth2 = 1e-12;

typedef list<IndexedPredInfo> Stats;
static Stats predList;
static RunList * contribRuns;

static double * W;
//static double * tsW;
static double * run_weights;
static double * notW;
static double * notrun_weights;
//static double * truW;
//static double * trurun_weights;
static double * pred_weights[2];
//static double sign[] = {1.0, -1.0};
//static double mask[2][2] = {{1.0, 0.0}, {0.0, -1.0}};

static ofstream logfp ("pred_scores.log", ios_base::trunc);

enum { F, S };

enum { L0, L1, L2, MAX};

////////////////////////////////////////////////////////////////////////
//
//  Utilities
//

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

void 
normalize (double u[][2], unsigned type, unsigned n, unsigned mode) 
{
  double norm = 0.0;
  assert(type < 2);
  for (unsigned i = 0; i < n; ++i) {
    switch (mode) {
    case L1:
      norm += abs(u[i][type]);
      break;
    case L2:
      norm += u[i][type] * u[i][type];
      break;
    case MAX:
      norm = (norm > abs(u[i][type])) ? norm : abs(u[i][type]);
      break;
    default:
      cerr << "Unknown mode for normalization: " << mode << endl;
      assert(0);
    }
  }

  for (unsigned i = 0; i < n; ++i) {
    u[i][type] = u[i][type] / norm;
  }
}

double
compute_change(const double *u, const double * v, const unsigned n)
{
  double change = 0.0;
  double diff;
  for (unsigned i = 0; i < n; ++i) {
    diff = u[i] - v[i];
    change += diff*diff;
  }

  return change;
}

inline double
contrib(const double *W_data, const unsigned k, const unsigned j, 
	const unsigned r, const double *v, const unsigned npreds) 
{
  double Akj = W_data[j*npreds+k];
  double vk = (is_srun[r]) ? 1.0/v[k] : v[k];
  
  return (Akj * vk);
}

////////////////////////////////////////////////////////////////////////
//
//  Input and output
//

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

void 
read_weights()
{
  unsigned npreds = predList.size();
  unsigned nruns = num_sruns + num_fruns;
  unsigned type;
  double x, notx;
  //double w;
  //double trux;

  W = new double[nruns*npreds];
  //tsW = new double[nruns*npreds];
  notW = new double[nruns*npreds];
  run_weights = new double[nruns];
  notrun_weights = new double[nruns];
  //truW = new double[nruns*npreds];
  //trurun_weights = new double[nruns];

  memset(W, 0, sizeof(double)*nruns*npreds);
  //memset(tsW, 0, sizeof(double)*nruns*npreds);
  memset(notW, 0, sizeof(double)*nruns*npreds);
  memset(run_weights, 0, sizeof(double)*nruns);
  memset(notrun_weights, 0, sizeof(double)*nruns);
  //memset(trurun_weights, 0, sizeof(double)*nruns);

  //FILE * wfp = fopenRead("W.first.dat");
  FILE * xfp = fopenRead("X.dat");
  FILE * notxfp = fopenRead("notX.dat");
  //FILE * xfp = fopenRead("truFreq.dat");
  //FILE * notxfp = fopenRead("trunotFreq.dat");
  //FILE * truxfp = fopenRead("truX.dat");
  int ctr;
  unsigned i = 0;
  for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    if (!is_srun[r] && !is_frun[r])
      continue;
    
    type = (is_srun[r]);
    for (unsigned j = 0; j < npreds; ++j) {
      //ctr = fscanf(wfp, "%lg ", &w);
      //assert(ctr == 1);
      ctr = fscanf(xfp, "%lg ", &x);
      assert(ctr == 1);
      ctr = fscanf(notxfp, "%lg ", &notx);
      assert(ctr == 1);
      //ctr = fscanf(truxfp, "%lg ", &trux);
      //assert(ctr == 1);
      //assert(w >= 0.0);
      //assert(w >= 0.0);
      assert(x >= 0.0);
      assert(notx >= 0.0);
      //assert(trux >= 0.0);
      //W[i*npreds+j] = w*x;
      //tsW[i*npreds+j] = w;
      W[i*npreds+j] = x;
      notW[i*npreds+j] = notx;
      //truW[i*npreds+j] = trux;
      run_weights[i] += W[i*npreds+j];
      notrun_weights[i] += notW[i*npreds+j];
      //trurun_weights[i] += truW[i*npreds+j];
      //      denom_weights[type][j] += W[i*npreds+j];
    }
    //fscanf(wfp, "\n");
    fscanf(xfp, "\n");
    fscanf(notxfp, "\n");
    //fscanf(truxfp, "\n");

    ++i;
  }
  
  //fclose (wfp);
  fclose (xfp);
  fclose (notxfp);
  //fclose (truxfp);

//   for (unsigned j = 0; j < npreds; ++j) {
//     if (denom_weights[F][j] < 0.0)
//       cout << "F pred: " << j << endl;
//     if (denom_weights[S][j] < 0.0)
//       cout << "S pred: " << j << endl;
//     //denom_weights[F][j] = (double) num_fruns;
//   }
}

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

void
print_pred(ofstream &outfp, ofstream &runfp, double * v, double * notv, 
           double oldpscore[][2], double oldnotpscore[][2],
           const unsigned j, const unsigned r, const unsigned npreds)
{
  runfp << j << " " << r << endl;
  for (unsigned k = 0; k < npreds; ++k) 
    outfp << contrib(W, k, j, r, v, npreds) << ' ';
  outfp << endl;
  for (unsigned k = 0; k < npreds; ++k) 
    outfp << contrib(notW, k, j, r, notv, npreds) << ' ';
  outfp << endl;
  for (unsigned k = 0; k < npreds; ++k) 
    outfp << oldpscore[k][S] << ' ';
  outfp << endl;
  for (unsigned k = 0; k < npreds; ++k) 
    outfp << oldnotpscore[k][S] << ' ';
  outfp << endl;
  for (unsigned k = 0; k < npreds; ++k) 
    outfp << W[j*npreds+k] << ' ';
  outfp << endl;
  for (unsigned k = 0; k < npreds; ++k) 
    outfp << notW[j*npreds+k] << ' ';
  outfp << endl;
  //for (unsigned k = 0; k < npreds; ++k) 
    //outfp << tsW[j*npreds+k] << ' ';
  outfp << endl;
}

void 
logvalues (const double * v, const unsigned n) {
  for (unsigned i = 0; i < n; ++i) {
    logfp << v[i] << ' ';
  }
  logfp << endl;
}

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

inline void
update_max(const unsigned j, const unsigned r, const unsigned npreds,
           double *v, double *notv,
           double oldpscore[][2], double oldnotpscore[][2],
           double pscore[][2], double notpscore[][2])
{
  double Aij, notAij;
  double d = 0.0;
  double notd = 0.0;
  unsigned argmax = 0;
  double maxd = 0.0;
  double val = 0.0;
  unsigned type = (is_srun[r]);
  unsigned othertype = !type;
  for (unsigned k = 0; k < npreds; ++k) {
    d = contrib(W, k, j, r, v, npreds);
    notd = contrib(notW, k, j, r, notv, npreds);
    val = (d + smooth2) / (notd + smooth2)
          * (oldnotpscore[k][othertype]+ smooth)/(oldpscore[k][othertype]+ smooth);
    //if (val > maxd || (val == maxd && tsW[j*npreds+k] > tsW[j*npreds+argmax])) {
    if (val > maxd) {
      maxd = val;
      argmax = k;
    }
  }

  Aij = W[j*npreds+argmax];
  notAij = notW[j*npreds+argmax];
  pscore[argmax][type] += Aij / run_weights[j];
  notpscore[argmax][type] += notAij / notrun_weights[j];
  if (is_frun[r])
    contribRuns[argmax].push_back(r);

}

void
iterate_max(double * u, double * notu, double * v, double * notv,
            double oldpscore[][2], double oldnotpscore[][2],
            unsigned npreds)
{
  double pscore[npreds][2], notpscore[npreds][2];

  memset(pscore, 0, sizeof(double)*2*npreds);
  memset(notpscore, 0, sizeof(double)*2*npreds);
  memset(u, 0, sizeof(double)*npreds);
  for (unsigned i = 0; i < npreds; ++i) {
    contribRuns[i].clear();
  }

  cout << "Giving weight to max pred..." << endl;

  //normalize(oldpscore, F, npreds, MAX);
  //normalize(oldpscore, S, npreds, MAX);
  //normalize(oldnotpscore, F, npreds, MAX);
  //normalize(oldnotpscore, S, npreds, MAX);

  unsigned j = 0;
  for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    if (!is_srun[r] && !is_frun[r])
      continue;

    update_max(j, r, npreds, v, notv, oldpscore, oldnotpscore, pscore, notpscore);

    j++;
  }

  for (unsigned i = 0; i < npreds; ++i) {
    pred_weights[F][i] = (pscore[i][F] + smooth)/(notpscore[i][F] + smooth);
    pred_weights[S][i] = (notpscore[i][S] + smooth)/(pscore[i][S] + smooth);
    u[i] = contribRuns[i].size();
    //u[i] = pred_weights[F][i] * pred_weights[S][i] * contribRuns[i].size();
    //u[i] = pred_weights[F][i];
    notu[i] = 1.0/u[i];
  }

  //normalize(u, npreds, MAX);

  //change = compute_change(u, v, npreds);
  //copy over from u to v 
  memcpy(v, u, sizeof(double)*npreds);
  // logvalues (v, npreds);
  logruns (npreds);
  //logfp << "iterate_max: iteration: " << ctr << " change: " << change << endl;
}

inline void
update_sum(const unsigned j, const unsigned r, const unsigned npreds,
           double *v, double *notv,
           double pscore[][2], double notpscore[][2])
{
  double Aij, notAij;
  unsigned type = (is_srun[r]);
  double discount = 0.0;
  double disci = 0.0;
  double notdiscount = 0.0;
  double notdisci = 0.0;
  for (unsigned k = 0; k < npreds; ++k) {
    discount += contrib(W, k, j, r, v, npreds);
    notdiscount += contrib(notW, k, j, r, notv, npreds);
  }
  for (unsigned i = 0; i < npreds; ++i) {
    Aij = W[j*npreds+i];
    disci = (discount - contrib(W,i,j,r,v,npreds)) / discount / run_weights[j];
    pscore[i][type] += Aij * ( 1.0 - disci);

    notAij = notW[j*npreds+i];
    notdisci = (notdiscount - contrib(notW,i,j,r,notv,npreds)) / notdiscount / notrun_weights[j];
    notpscore[i][type] += notAij * ( 1.0 - notdisci);
  }
}

void
iterate_sum(double * u, double * notu, double * v, double * notv,
            double pscore[][2], double notpscore[][2],
            unsigned npreds)
{
  double change;
  unsigned ctr = 0;

  for (unsigned i = 0; i < npreds; ++i) {
    v[i] = 1.0;
    notv[i] = 1.0;
  }

  Progress::Bounded progress("Iterating sum updates", MAX_ITER);
  do {
    progress.step();
    ctr++;
    memset(pscore, 0, sizeof(double)*2*npreds);
    memset(notpscore, 0, sizeof(double)*2*npreds);
    memset(u, 0, sizeof(double)*npreds);
    unsigned j = 0;
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
      if (!is_srun[r] && !is_frun[r])
	continue;

      update_sum(j, r, npreds, v, notv, pscore, notpscore);
      j++;
    }

    for (unsigned i = 0; i < npreds; ++i) {
      u[i] = (pscore[i][F] + smooth) / (pscore[i][S] + smooth) 
	* (notpscore[i][S] + smooth) / (notpscore[i][F] + smooth);
      //u[i] = (pscore[i][F] + notpscore[i][S] + smooth)
        /// (pscore[i][S] + notpscore[i][F] + smooth);
      notu[i] = 1.0 / u[i];
    }

    normalize(u, npreds, MAX);
    normalize(notu, npreds, MAX);

    change = compute_change(u, v, npreds) + compute_change(notu, notv, npreds);
    //copy over from u to v 
    memcpy(v, u, sizeof(double)*npreds);
    memcpy(notv, notu, sizeof(double)*npreds);
    // logvalues (v, npreds);
    logfp << "iterate_sum: iteration: " << ctr << " change: " << change << endl;
  } while (change > thresh && ctr < MAX_ITER);
}

void
compute_scores()
{
  unsigned npreds = predList.size();
  double pscore[npreds][2];
  double notpscore[npreds][2];
  double v[npreds], u[npreds];
  double notv[npreds], notu[npreds];
  pred_weights[F] = new double[npreds];
  pred_weights[S] = new double[npreds];
  memset(pred_weights[F], 0, sizeof(double)*npreds);
  memset(pred_weights[S], 0, sizeof(double)*npreds);

  iterate_sum(u, notu, v, notv, pscore, notpscore, npreds);
  iterate_max(u, notu, v, notv, pscore, notpscore, npreds);

  unsigned i = 0;
  for (Stats::iterator c = predList.begin(); c != predList.end(); ++c) {
    (*c).ps.fdenom = pred_weights[F][i];
    (*c).ps.sdenom = pred_weights[S][i];
    (*c).ps.imp = v[i++];
  }

}

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

void
compute_tstats(double *tstat, double *m1, double *s1, 
               double *m2, double *s2, double n1,
               double n2, unsigned len)
{
  cout << "n1: " << n1 << " n2: " << n2 << endl;
  assert(n1 >= 1.0);
  assert(n2 >= 1.0);
  assert(n1+n2 >= 2.0);
  double c1;
  double c2;
  double sigma;
  c1 = sqrt(1.0/n1 + 1.0/n2);
  c2 = sqrt(n1+n2-2.0);
  for (unsigned i = 0; i < len; ++i) {
    sigma = sqrt(s1[i]*(n1-1.0) + s2[i]*(n2-1.0));
    //tstat[i] = (m2[i]-m1[i])/c1;
    tstat[i] = m2[i] - m1[i];
    if (sigma > 0.0)
      tstat[i] = tstat[i] / sigma * c2;
  }
  logfp << "T-test stats" << endl;
  logvalues(tstat, len);
}

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
  unsigned maxlen = 10, ctr = 0;
  char filename[128];

  //read_freqs();  // read in the real dst/dso frequencies

  // do two-sample t-test for the failed runs accounted for by 
  // the 10 most important predicates
  check.resize(maxlen);
  for (Stats::iterator c = predList.begin(); c != predList.end(); ++c) {
    check[ctr] = c->index;
    if (ctr++ >= maxlen)
      break;
  }

  cout << "num_sruns: " << num_sruns << " num_fruns: " << num_fruns << endl;
  ns = collect_stats(smean, svar, npreds, is_srun);
  assert(ns == num_sruns);
  nf = collect_stats(fmean,fvar, npreds, is_frun);
  assert(nf == num_fruns);
  compute_tstats(tstat, smean, svar, fmean, fvar, ns, nf, npreds);
  for (Stats::iterator c = predList.begin(); c != predList.end(); ++c)
    c->ps.imp = tstat[c->index];
  print_scores("list0.xml");

  for (unsigned i = 0; i < maxlen; ++i) {
    unsigned pind = check[i];
    if (contribRuns[pind].size() == 0)
      continue;
    collect.clear();
    collect.resize(NumRuns::end);
    for (RunList::const_iterator c = contribRuns[pind].begin();
         c != contribRuns[pind].end(); ++c) {
      collect[(*c)] = 1;
    }
    nf = collect_stats(fmean, fvar, npreds, collect);
    compute_tstats(tstat, smean, svar, fmean, fvar, ns, nf, npreds);
    for (Stats::iterator c = predList.begin(); c != predList.end(); ++c) 
      c->ps.imp = tstat[c->index];
    sprintf(filename, "list%d.xml", i+1);
    print_scores(filename);
  }
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
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
};

////////////////////////////////////////////////////////////////
//
// The main event
//

int main (int argc, char** argv)
{
  process_cmdline (argc, argv);
  
  classify_runs();

  read_preds();

  read_weights();

  compute_scores();

  // must print out histograms before the scores,
  // because print_scores alters the ordering of predicates
  // through sorting
  if (XMLTemplate::prefix == "moss")
    print_histograms();

  print_scores("pred_scores.xml");

  ttest_rank();

  logfp.close();
  delete[] W;
  delete[] notW;
  delete[] run_weights;
  delete[] notrun_weights;
  delete[] pred_weights[F];
  delete[] pred_weights[S];
  delete[] contribRuns;
  return 0;
}
