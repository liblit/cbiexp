#include <argp.h>
#include <cassert>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <ext/hash_map>
#include <fstream>
#include <gsl/gsl_blas.h>
#include <queue>
#include <vector>
#include "PredStats.h"
#include "PredCoords.h"
#include "fopen.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;

const double thresh = 10e-12;

typedef hash_map<PredCoords, double> pred_hash_t;
static pred_hash_t predHash;

static vector<PredCoords> predVec;

static double * W_data;

static ofstream logfp ("predscores.log", ios_base::trunc);

////////////////////////////////////////////////////////////////////////
//
//  Input and output
//

void 
read_preds()
{
  queue<PredCoords> predQueue;

  FILE * const pfp = fopenRead(PredStats::filename);
  pred_info pi;
  while (read_pred_full(pfp, pi)) {
    predHash[pi] = 0.0;
    predQueue.push(pi);
  }
  fclose(pfp);

  predVec.resize(predQueue.size());
  int i = 0;
  while (!predQueue.empty()) {
    predVec[i++] = predQueue.front();
    predQueue.pop();
  }
}

void 
read_weights()
{
  unsigned npreds = predVec.size();

  W_data = new double[npreds*npreds];

  FILE * fp = fopenRead("WX.dat");
  int ctr;
  for (unsigned i = 0; i < npreds; ++i) {
    fscanf(fp, "  ");
    for (unsigned j = 0; j < npreds; ++j) {
      ctr = fscanf(fp, "%lg\t", W_data+i*npreds+j);
      assert(ctr == 1);
    }
    fscanf(fp, "\n");
  }
  
  fclose (fp);
}

void
print_scores()
{
  ofstream outfp ("pred_scores.txt");
  for (unsigned i = 0; i < predVec.size(); ++i) {
    PredCoords &pc = predVec[i];
    pred_hash_t::iterator found = predHash.find(pc);
    assert(found != predHash.end());
    outfp << pc << ' ' << scientific << setprecision(10) << found->second << endl;
    outfp.unsetf(ios::floatfield);
  }
  outfp.close();
}

void 
logvalues (gsl_vector * v) {
  for (unsigned i = 0; i < v->size; ++i) {
    logfp << gsl_vector_get(v, i) << ' ';
  }
  logfp << endl;
}

////////////////////////////////////////////////////////////////////////
//
//  Computation routines
//
double
compute_change(const gsl_vector *u, const gsl_vector * v, const gsl_vector *onevec)
{
  double change;
  gsl_vector * temp = gsl_vector_alloc(u->size);
  gsl_vector_memcpy (temp, u);

  gsl_vector_sub(temp, v);
  gsl_vector_mul(temp, temp);
  gsl_blas_ddot(onevec, temp, &change);

  gsl_vector_free (temp);
  return change;
}

void
compute_scores()
{
  double change = 0.0;
  unsigned npreds = predVec.size();
  gsl_vector * onevec = gsl_vector_alloc(npreds);
  gsl_vector * v = gsl_vector_alloc(npreds);
  gsl_matrix_view W = gsl_matrix_view_array(W_data, npreds, npreds);
  gsl_vector * u = gsl_vector_alloc(npreds);
  double norm = 0.0;

  gsl_vector_set_all (onevec, 1.0);
  gsl_vector_set_all (v, 1.0);
  gsl_vector_set_zero (u);

  do {
    // compute W*v
    gsl_blas_dgemv (CblasNoTrans, 1.0, &W.matrix, v, 0.0, u);
    gsl_blas_ddot (onevec, v, &norm);
    assert(norm > 0.0);
    gsl_vector_scale (u, 1.0/norm);
    change = compute_change (u, v, onevec);
    gsl_vector_memcpy (v, u);
    logvalues (v);
    logfp << "change: " << change << endl;
  } while (change > thresh);

  // transfer scores over to the hash table
  for (unsigned i = 0; i < predVec.size(); ++i) {
    double score = gsl_vector_get (v, i);
    PredCoords &pc = predVec[i];
    predHash[pc] = score;
  }

  gsl_vector_free (onevec);
  gsl_vector_free (v);
  gsl_vector_free (u);
}

////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//

static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
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

  read_preds();

  read_weights();

  compute_scores();

  print_scores();

  logfp.close();
  delete[] W_data;
  return 0;
}
