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

const unsigned MAX_ITER = 100;
const double thresh = 10e-12;
const double smooth = 10e-12;

static list<ostringstream> contribRuns;
typedef list<IndexedPredInfo> Stats;
static Stats predList;

static double * W_data;
static double * denom_weights[2];
static double * run_weights;
static double sign[] = {1.0, -1.0};
static double mask[2][2] = {{1.0, 0.0}, {0.0, -1.0}};

static ofstream logfp ("pred_scores.log", ios_base::trunc);

enum { F, S };

enum { L0, L1, L2, MAX};

////////////////////////////////////////////////////////////////////////
//
//  Input and output
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
read_preds()
{

  FILE * const pfp = fopenRead(PredStats::filename);
  pred_info pi;
  unsigned index = 0;
  while (read_pred_full(pfp, pi)) {
    IndexedPredInfo indexed(pi, index++);
    predList.push_back(indexed);
    contribRuns.push_back("");
  }
  fclose(pfp);

}

void 
read_weights()
{
  unsigned npreds = predList.size();
  unsigned nruns = num_sruns + num_fruns;
  unsigned mode;
  double x;
  //double w, x;

  W_data = new double[nruns*npreds];
  denom_weights[F] = new double[npreds];
  denom_weights[S] = new double[npreds];
  run_weights = new double[nruns];

  memset(denom_weights[F], 0, sizeof(double)*npreds);
  memset(denom_weights[S], 0, sizeof(double)*npreds);
  memset(run_weights, 0, sizeof(double)*nruns);

  FILE * wfp = fopenRead("W.first.dat");
  FILE * xfp = fopenRead("X.dat");
  int ctr;
  unsigned i = 0;
  for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    if (!is_srun[r] && !is_frun[r])
      continue;
    
    mode = (is_srun[r]);
    for (unsigned j = 0; j < npreds; ++j) {
      //ctr = fscanf(wfp, "%lg ", &w);
      //assert(ctr == 1);
      ctr = fscanf(xfp, "%lg ", &x);
      assert(ctr == 1);
      //assert(w >= 0.0);
      assert(x >= 0.0);
      //W_data[i*npreds+j] = w*x;
      W_data[i*npreds+j] = x;
      run_weights[i] += W_data[i*npreds+j];
      denom_weights[mode][j] += W_data[i*npreds+j];
    }
    fscanf(wfp, "\n");
    fscanf(xfp, "\n");

    ++i;
  }
  
  fclose (wfp);
  fclose (xfp);

  for (unsigned j = 0; j < npreds; ++j) {
    if (denom_weights[F][j] < 0.0)
      cout << "F pred: " << j << endl;
    if (denom_weights[S][j] < 0.0)
      cout << "S pred: " << j << endl;
    //denom_weights[F][j] = (double) num_fruns;
  }
}

 
void
print_scores()
{
  // sort according to descending order
  predList.sort(Sort::Descending<Score::Importance>());

  ofstream outfp ("pred_scores.xml");
  outfp << "<?xml version=\"1.0\"?>" << endl
        << "<?xml-stylesheet type=\"text/xsl\" href=\"" 
	<< XMLTemplate::format("scores") << ".xsl\"?>" << endl
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
logvalues (const double * v, const unsigned n) {
  for (unsigned i = 0; i < n; ++i) {
    logfp << v[i] << ' ';
  }
  logfp << endl;
}

////////////////////////////////////////////////////////////////////////
//
//  Computation routines
//
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
contrib(const unsigned k, const unsigned j, const unsigned r, 
	const double *v, const unsigned npreds) 
{
  double Akj = W_data[j*npreds+k];
  //double Pk;
  //if (is_frun[r]) {
    //if (v[k] > 0)
      //Pk = v[k];
    //else Pk = 0.0;
  //}
  //else { // must be successful run 
    //if (v[k] > 0)
      //Pk = 0.0;
    //else
      //Pk = -v[k];
  //}
  
  return (Akj * v[k] * mask[is_srun[r]][(v[k]<=0.0)]);
}

void
iterate_max(double * u, double * v, unsigned npreds)
{
  double Aij, change;
  unsigned mode;
  unsigned ctr = 0;

  for (unsigned i = 0; i < npreds; ++i) {
    contribRuns[i].clear();
  }

  Progress::Bounded progress("Giving weight to max pred", MAX_ITER);
  do {
    progress.step();
    ctr++;
    memset(u, 0, sizeof(double)*npreds);
    unsigned j = 0;
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
      if (!is_srun[r] && !is_frun[r])
	continue;

      mode = (is_srun[r]);
      double disck = 0.0;
      double maxd = 0.0;
      unsigned argmax = 0;
      for (unsigned k = 0; k < npreds; ++k) {
        disck = contrib(k, j, r, v, npreds);
        if (disck > maxd) {
          maxd = disck;
          argmax = k;
        }
      }
      Aij = W_data[j*npreds+argmax];
      //if (denom_weights[mode][argmax] > 0.0)
        //u[argmax] += sign[mode] * Aij / denom_weights[mode][argmax];
      u[argmax] += sign[mode] * Aij / run_weights[j];
      contribRuns[argmax] += 
      j++;
    }

    normalize(u, npreds, MAX);

    change = compute_change(u, v, npreds);
    //copy over from u to v 
    memcpy(v, u, sizeof(double)*npreds);
    // logvalues (v, npreds);
    logfp << "iterate_max: iteration: " << ctr << " change: " << change << endl;
  } while (change > thresh && ctr <= MAX_ITER);
}

void
iterate_sum(double * u, double * v, unsigned npreds)
{
  double Aij, change;
  double tempscore[2][npreds];
  unsigned mode;
  unsigned ctr = 0;

  for (unsigned i = 0; i < npreds; ++i) {
    v[i] = 1.0;
  }

  Progress::Bounded progress("Iterating sum updates", MAX_ITER);
  do {
    progress.step();
    ctr++;
    memset(tempscore, 0, sizeof(double)*2*npreds);
    unsigned j = 0;
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
      if (!is_srun[r] && !is_frun[r])
	continue;

      mode = (is_srun[r]);
      double discount = 0.0;
      double disci = 0.0;
      for (unsigned k = 0; k < npreds; ++k) {
        discount += contrib(k, j, r, v, npreds);
      }
      for (unsigned i = 0; i < npreds; ++i) {
	Aij = W_data[j*npreds+i];
	disci = (discount - contrib(i,j,r,v,npreds)) / run_weights[j];
	tempscore[mode][i] += Aij * ( 1.0 - disci);
      }

      j++;
    }

    for (unsigned i = 0; i < npreds; ++i) {
      u[i] = (tempscore[F][i] + smooth) / (tempscore[S][i] + smooth);
    }

    normalize(u, npreds, MAX);

    change = compute_change(u, v, npreds);
    //copy over from u to v 
    memcpy(v, u, sizeof(double)*npreds);
    logvalues (v, npreds);
    logfp << "iterate_sum: iteration: " << ctr << " change: " << change << endl;
  } while (change > thresh && ctr <= MAX_ITER);
}

void
compute_scores()
{
  unsigned npreds = predList.size();
  double v[npreds];
  double u[npreds];

  iterate_sum(u, v, npreds);
  iterate_max(u, v, npreds);

  unsigned i = 0;
  for (Stats::iterator c = predList.begin(); c != predList.end(); ++c) {
    (*c).ps.fdenom = denom_weights[F][i];
    (*c).ps.sdenom = denom_weights[S][i];
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

  compute_scores();

  print_scores();

  logfp.close();
  delete[] W_data;
  delete[] denom_weights[F];
  delete[] denom_weights[S];
  return 0;
}
