#include <argp.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include "IndexedPredInfo.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "RunsDirectory.h"
#include "Score/Importance.h"
#include "XMLTemplate.h"
#include "classify_runs.h"
#include "fopen.h"
#include "sorts.h"
#include "utils.h"

using namespace std;

const unsigned numwatch = 4;
const unsigned watch[] = {2577, 2434, 2594, 2595};
static ofstream watchfp[numwatch];
static ofstream runsfp[numwatch];

const unsigned MAX_ITER = 20;
const double thresh = 10e-12;
const double smooth = 1.0;
const double smooth2 = 1e-12;

typedef list<IndexedPredInfo> Stats;
static Stats predList;
typedef list<unsigned> RunList;
static RunList * contribRuns;
static RunList * runbugs;
static int numbugs = 10;

static double * W;
static double * tsW;
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
  double w;
  //double trux;

  W = new double[nruns*npreds];
  tsW = new double[nruns*npreds];
  notW = new double[nruns*npreds];
  run_weights = new double[nruns];
  notrun_weights = new double[nruns];
  //truW = new double[nruns*npreds];
  //trurun_weights = new double[nruns];

  memset(W, 0, sizeof(double)*nruns*npreds);
  memset(tsW, 0, sizeof(double)*nruns*npreds);
  memset(notW, 0, sizeof(double)*nruns*npreds);
  memset(run_weights, 0, sizeof(double)*nruns);
  memset(notrun_weights, 0, sizeof(double)*nruns);
  //memset(trurun_weights, 0, sizeof(double)*nruns);

  FILE * wfp = fopenRead("W.first.dat");
  FILE * xfp = fopenRead("X.dat");
  FILE * notxfp = fopenRead("notX.dat");
  //FILE * truxfp = fopenRead("truX.dat");
  int ctr;
  unsigned i = 0;
  for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    if (!is_srun[r] && !is_frun[r])
      continue;
    
    type = (is_srun[r]);
    for (unsigned j = 0; j < npreds; ++j) {
      ctr = fscanf(wfp, "%lg ", &w);
      assert(ctr == 1);
      ctr = fscanf(xfp, "%lg ", &x);
      assert(ctr == 1);
      ctr = fscanf(notxfp, "%lg ", &notx);
      assert(ctr == 1);
      //ctr = fscanf(truxfp, "%lg ", &trux);
      //assert(ctr == 1);
      //assert(w >= 0.0);
      assert(w >= 0.0);
      assert(x >= 0.0);
      assert(notx >= 0.0);
      //assert(trux >= 0.0);
      //W[i*npreds+j] = w*x;
      tsW[i*npreds+j] = w;
      W[i*npreds+j] = x;
      notW[i*npreds+j] = notx;
      //truW[i*npreds+j] = trux;
      run_weights[i] += W[i*npreds+j];
      notrun_weights[i] += notW[i*npreds+j];
      //trurun_weights[i] += truW[i*npreds+j];
      //      denom_weights[type][j] += W[i*npreds+j];
    }
    fscanf(wfp, "\n");
    fscanf(xfp, "\n");
    fscanf(notxfp, "\n");
    //fscanf(truxfp, "\n");

    ++i;
  }
  
  fclose (wfp);
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

// read in the list of failed runs and their bugs
void
read_runbugs ()
{
  const unsigned numRuns = NumRuns::end;
  runbugs = new RunList[numRuns];
  for (unsigned i = 0; i < numRuns; ++i) {
    runbugs[i].clear();
  }

  FILE * runfp = fopenRead("run-bugs.txt");
  assert(runfp);
  char buf[1024];
  char *pos;
  int r;
  while (true) {
    fgets(buf, 1024, runfp);
    if (feof(runfp))
      break;

    pos = strtok(buf, "\t");
    r = atoi(pos);
    pos = strtok(NULL, "\t\n");
    while (pos != NULL) {
      runbugs[r].push_back(atoi(pos));
      pos = strtok(NULL, "\t\n");
    }
  }

  fclose(runfp);
}

// aggregate the bug histogram correponsing to a list of runs
void
bug_hist (int * bugs, int numbugs, const RunList &runs)
{
  memset(bugs, 0, sizeof(int)*numbugs);
  for (RunList::const_iterator c = runs.begin(); 
       c != runs.end(); ++c) {
    const RunList &buglist = runbugs[(*c)];
    for (RunList::const_iterator d = buglist.begin();
         d != buglist.end(); ++d) {
      int b = (*d);
      assert(b >= 1 && b <= numbugs);
      bugs[b-1] += 1;
    }
  }
}

void
print_histograms()
{
  int bugs[numbugs];

  read_runbugs();
  ofstream outfp ("pred_histograms.xml");
  outfp << "<?xml version=\"1.0\"?>" << endl
        << "<!DOCTYPE histograms SYSTEM \"histograms.dtd\">" << endl
        << "<histograms>" << endl;

  unsigned npreds = predList.size();
  for (unsigned i = 0; i < npreds; ++i) {
    bug_hist(bugs, numbugs, contribRuns[i]);
    outfp << "<predictor>" << endl;
    for (int j = 0; j < numbugs; ++j) {
      outfp << "<bug count=\"" << bugs[j] << "\"/>" << endl; 
    }
    outfp << "</predictor>" << endl;
  }
  
  outfp << "</histograms>" << endl;
  outfp.close();
}
 
void
print_scores()
{
  // sort according to descending order
  predList.sort(Sort::Descending<Score::Importance>());

  ofstream outfp ("pred_scores.xml");
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
  for (unsigned k = 0; k < npreds; ++k) 
    outfp << tsW[j*npreds+k] << ' ';
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
    val = (d + smooth2)/(notd + smooth2)
          *(oldnotpscore[k][othertype]+smooth)/(oldpscore[k][othertype]+smooth);
    if (val > maxd || (val == maxd && tsW[j*npreds+k] > tsW[j*npreds+argmax])) {
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

  for (unsigned i = 0; i < numwatch; i++) {
    if (argmax == watch[i])
      print_pred(watchfp[i], runsfp[i], v, notv, oldpscore, oldnotpscore, j, r, npreds);
  }

}

void
iterate_max(double * u, double * notu, double * v, double * notv,
            double oldpscore[][2], double oldnotpscore[][2],
            unsigned npreds)
{
  double pscore[npreds][2], notpscore[npreds][2];
  double change;
  unsigned ctr = 0;

  memset(pscore, 0, sizeof(double)*2*npreds);
  memset(notpscore, 0, sizeof(double)*2*npreds);
  memset(u, 0, sizeof(double)*npreds);
  for (unsigned i = 0; i < npreds; ++i) {
    contribRuns[i].clear();
  }

  cout << "Giving weight to max pred..." << endl;

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

  normalize(u, npreds, MAX);

  change = compute_change(u, v, npreds);
  //copy over from u to v 
  memcpy(v, u, sizeof(double)*npreds);
  // logvalues (v, npreds);
  logruns (npreds);
  logfp << "iterate_max: iteration: " << ctr << " change: " << change << endl;
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
      //u[i] = (pscore[i][F] * notpscore[i][S] + smooth)
        /// (pscore[i][S] * notpscore[i][F] + smooth);
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

  for (unsigned i = 0; i < numwatch; i++) {
    char fname[128];
    sprintf(fname, "pred_%d.log", watch[i]);
    watchfp[i].open (fname, ios_base::trunc);
    sprintf(fname, "pred_%d.runs", watch[i]);
    runsfp[i].open (fname, ios_base::trunc);
  }

  compute_scores();

  // must print out histograms before the scores,
  // because print_scores alters the ordering of predicates
  // through sorting
  if (XMLTemplate::prefix == "moss")
    print_histograms();

  print_scores();

  for (unsigned i = 0; i < numwatch; i++) {
    watchfp[i].close();
    runsfp[i].close();
  }
  logfp.close();
  delete[] W;
  delete[] notW;
  delete[] run_weights;
  delete[] notrun_weights;
  delete[] pred_weights[F];
  delete[] pred_weights[S];
  delete[] contribRuns;
  delete[] runbugs;
  return 0;
}
