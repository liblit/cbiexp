#include <argp.h>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ext/hash_map>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <vector>
#include "DiscreteDist.h"
#include "fopen.h"
#include "PredCoords.h"
#include "classify_runs.h"

using namespace std;
using __gnu_cxx::hash_map;

const unsigned MAXITER = 500;
const double thresh = 1e-10;
static ofstream logfp ("est_hyperparms.log", ios_base::trunc);

/****************************************************************************
 * Definitions of constants, structs, and classes
 ***************************************************************************/

static gsl_rng *generator;

/****************************************************************
 * Pred-specific information
 ***************************************************************/
struct pred_info_t {
    unsigned N, Z;
    count_tp S, Y;
    double a, b;
    double rho;
    unsigned Asize;
    count_tp Asumtrue, Asumobs;
    unsigned Bsize, Csize;
    double alpha, lambda, gamma;
    double beta[3];
    double t[3];
    DiscreteDist Bset;
    DiscreteDist Cset;
    void init_stats () {
	S = N = Y = Z = 0;
	a = b = 0.0;
	rho = 1.0;
	Asize = Asumtrue = Asumobs = Bsize = Csize = 0;
	Bset.clear();
	Cset.clear();
    }
    void read_stats(FILE * fp);
    void scale_t() {
      double mint = t[0];
      mint = (t[1] < mint) ? t[1] : mint;
      mint = (t[2] < mint) ? t[2] : mint;
      for (unsigned i = 0; i < 3; ++i)
        t[i] -= mint;
    }
    void init_parms() {
	alpha = (double) Y/S;
	lambda = (double) S/N/rho;
	gamma = 0.5; // (double) N/(N+Z);
	beta[0] = (double) Asize/(Asize + Bsize + Csize);
	beta[1] = (double) Bsize/(Asize + Bsize + Csize);
	beta[2] = (double) Csize/(Asize + Bsize + Csize);
	for (unsigned i = 0; i < 3; ++i) 
	  t[i] = log(beta[i]);
        scale_t();
	//t[0] = t[1] = t[2] = 1.0;
	//beta[0] = beta[1] = beta[2] = 1.0/3.0;
	//beta = gsl_rng_uniform(generator);
	//lambda = gsl_rng_uniform(generator)*(b-a) + a;
    }

    void map_estimate();
    bool est_Nparms();
    bool est_Xparms();
    double newton_raphson_N();
    double newton_raphson_X();
    void update_Xparms(double grad[4], double Hess[4][4]);
    void beta_der(double d[3][3], double D[3][3][3]);
    void B_der(double dB[4], double ddB[4][4], const double d[3][3], 
	       const double D[3][3][3] , const double m, double *Bi);
    void C_der(double dC[4], double ddC[4][4], const double d[3][3] , 
	       const double D[3][3][3] , const double m, double *Ci);
    void calc_beta() {
	double T = exp(t[0]) + exp(t[1]) + exp(t[2]);
	for (unsigned i = 0; i < 3; ++i) {
	    beta[i] = exp(t[i])/T;
	}
    }
};

ostream & operator<< (ostream &out, const pred_info_t &pi);

class PredPair
{
public:
    pred_info_t f;
    pred_info_t s;
    void init_stats() {
	f.init_stats();
	s.init_stats();
    }
    void init_parms() {
	f.init_parms();
	s.init_parms();
    }
};

static PredPair currPred;
static PredCoords currCoords;

/****************************************************************************
 * Utilities
 ***************************************************************************/
void
pred_info_t::read_stats (FILE * fp)
{
    Bset.clear();
    Cset.clear();
    int res;

    res = fscanf(fp, "%Lu %u %Lu %u %lg %lg %lg %u %Lu %Lu %u %u\n", 
		 &S, &N, &Y, &Z, &a, &b, &rho,
		 &Asize, &Asumtrue, &Asumobs, &Bsize, &Csize);
    assert(res == 12);

    count_tp val;
    unsigned count;
    unsigned total = 0;
    while (total < Bsize) {
	res = fscanf(fp, "%Lu %u ", &val, &count);
	assert(res == 2);
	Bset[val] = count;
	total += count;
    }
    fscanf(fp, "\n");
    assert(total == Bsize);

    total = 0;
    while (total < Csize) {
	res = fscanf(fp, "%Lu %u ", &val, &count);
	assert(res == 2);
	Cset[val] = count;
	total += count;
    }
    fscanf(fp, "\n");
    assert(total == Csize);
}

bool
read_stats(FILE * fp)
{
  int res;
  
  res = fscanf(fp, "%u\t%u\t%u\n", &currCoords.unitIndex, &currCoords.siteOffset, &currCoords.predicate);
  if (feof(fp))
      return false;
  assert(res == 3);
  
  currPred.f.read_stats(fp);
  currPred.s.read_stats(fp);

  return true;
}

void 
checkstatus(bool retval, char * errstr)
{
    if (retval) {
	logfp << "Pred " << currCoords << ": " << errstr << endl;
	logfp.close();
	exit(1);
    }
}

/****************************************************************************
 * Main computational routines
 ***************************************************************************/
inline void
project(double *val, const double min, const double max)
{
    if (*val < min) {
      logfp << "Value " << *val << " less than minimum " << min << endl;
      *val = min+1e-5;
      //logfp.close();
      //exit(1);
    }
    if (*val > max) {
      logfp << "Value " << *val << " greater than maximum " << max << endl;
      *val = max-1e-5;
    }
}

double
pred_info_t::newton_raphson_N()
{
    double ex = exp(-lambda*rho);
    double C = ex - 1.0;
    double A = 1.0 + C * gamma;
    double E = -rho*ex;
    double B = gamma * E;
    double D = B * (-rho);
    double t1 = (Z > 0) ? ((double) Z/A) : 0.0;
    double zloga = (Z > 0)? ((double) Z * log(A)) : 0.0;
    // likelihood
    double L = N*log(gamma) - lambda*rho*N + S*log(lambda)+ zloga;

    // logfp << "log(A): " << log(A) << " Z/A: " << t1 << endl;

    // gradient
    double ga = -rho*N + S/lambda + t1*B;  // partial wrt lambda
    double gb = N/gamma + t1*C;         // partial wrt gamma
    // Hessian
    double t2 = (Z > 0) ? (t1 * (B*B/A-D)) : 0.0;
    double aa = -(double) S/(lambda*lambda) - t2;
    double bb = (Z > 0) ? (-t1*(B*C/A - E)) : 0.0;
    double t3 = (Z > 0) ? (t1*(C*C/A)) : 0.0;
    double dd = -(double) N/(gamma*gamma) - t3;
    double det = 1.0/(aa*dd-bb*bb);

    double lchange = det*(dd*ga-bb*gb);
    double gchange = det *(-bb*ga + aa*gb);

     //logfp << "ex: " << ex << " C: " << C << " A: " << A 
 	  //<< " E: " << E << " B: " << B << " D: " << D
 	  //<< " t1: " << t1 << endl;
     //logfp << S << ' ' << -S/(lambda*lambda) << ' ' << B*B/A-D << endl;
     //logfp << "a: " << ga << " b: " << gb << " aa: " << aa << " bb: " << bb << " dd: " << dd << " det: " << det << " lchange: " << lchange << " gchange: " << gchange << endl;

    lambda -= lchange;
    gamma -= gchange;

    return L;
}

bool
pred_info_t::est_Nparms()
{
  if (N == 0) {
    gamma = 0.0;
    return 0;
  }

  double lam0, gamma0, change;
  double L0 = (std::numeric_limits<double>::min)(), L1;
  unsigned niter = 0;

  logfp << "L0 " << L0 << " lam0: " << lambda << " gamma0: " << gamma << endl;
  do {
      lam0 = lambda;
      gamma0 = gamma;
      L1 = newton_raphson_N();
      //assert(L1-L0 >= 0.0);
      project(&gamma, 0.0, 1.0);
      project(&lambda, a, b/rho);
      change = abs(lam0-lambda)+abs(gamma0-gamma);
      logfp << "L1: " << L1 << " lam1: " << lambda << " gamma1: " << gamma 
	    << " change: " << change << endl;
      niter++;
      L0 = L1;
  } while ( change > thresh && niter < MAXITER);

  if (niter == MAXITER) {
      logfp << "Newton-Raphson did not converge\n";
      return 1;
  }

  return 0;
}

void pred_info_t::beta_der(double d[3][3], double D[3][3][3])
{
    double et[] = {exp(t[0]), exp(t[1]), exp(t[2])};
    double T = et[0] + et[1] + et[2];
    double ij = 0.0;
    double jk = 0.0;
    double ik = 0.0;
    for (unsigned i = 0; i < 3; ++i) {
	for (unsigned j = 0; j < 3; ++j) {
	    ij = (i == j) ? 1.0 : 0.0;
	    d[i][j] = (T * et[i] * ij - et[i] * et[j])/(T*T);
	    for (unsigned k = j; k < 3; ++k) {
		ik = (i == k) ? 1.0 : 0.0;
		jk = (j == k) ? 1.0 : 0.0;
		D[i][j][k] = 
		    et[i]*(T*T*ij*jk - T*(et[j]*(ik+jk)+et[k]*ij) + 2.0*et[j]*et[k])
		    / ( T*T*T);
		D[i][k][j] = D[i][j][k]; // symmetric
	    }
	}
    }
}

// in order: t[0], t[1], t[2], alpha
void 
pred_info_t::B_der(double dB[4], double ddB[4][4], const double d[3][3], 
		   const double D[3][3][3] , const double m, double *Bi)
{
    double onea = 1.0-alpha;
    double log1a = log(onea);
    double oneam = exp(m*log1a);
    *Bi = beta[0]*oneam + beta[1];
    dB[3] = -beta[0]*m*oneam/onea; // dB/d alpha
    ddB[3][3] = -dB[3]*(m-1.0)/onea;  // d^2B/d alpha^2

    for (unsigned j = 0; j < 3; ++j) {
	dB[j] = d[0][j]*oneam + d[1][j];
	ddB[3][j] = dB[3]/beta[0]*d[0][j];
	ddB[j][3] = ddB[3][j];
	for (unsigned k = j; k < 3; ++k) {
	    ddB[j][k] = D[0][j][k] * oneam + D[1][j][k];
	    ddB[k][j] = ddB[j][k];
	}
    }

    //logfp << "Bi: " << *Bi << endl;
    //logfp << "dB: " << dB[0] << ' ' << dB[1] << ' ' << dB[2] << ' ' << dB[3] << endl;
    //logfp << "ddB: " << endl;
    //logfp << ddB[0][0] << ' ' << ddB[0][1] << ' ' << ddB[0][2] << ' ' << ddB[0][3] << endl
          //<< ddB[1][0] << ' ' << ddB[1][1] << ' ' << ddB[1][2] << ' ' << ddB[1][3] << endl
          //<< ddB[2][0] << ' ' << ddB[2][1] << ' ' << ddB[2][2] << ' ' << ddB[2][3] << endl
          //<< ddB[3][0] << ' ' << ddB[3][1] << ' ' << ddB[3][2] << ' ' << ddB[3][3] << endl;
}

void 
pred_info_t::C_der(double dC[4], double ddC[4][4], const double d[3][3] , 
		   const double D[3][3][3] , const double m, double *Ci)
{
    double loga = log(alpha);
    double am = exp(m*loga);
    *Ci = beta[0]*am + beta[2];
    dC[3] = beta[0]*m*am/alpha; // dC/d alpha
    ddC[3][3] = dC[3]*(m-1.0)/alpha;  // d^2 C/d alpha^2

    for (unsigned j = 0; j < 3; ++j) {
	dC[j] = d[0][j]*am + d[2][j];
	ddC[3][j] = dC[3]/beta[0]*d[0][j];
	ddC[j][3] = ddC[3][j];
	for (unsigned k = j; k < 3; ++k) {
	    ddC[j][k] = D[0][j][k] * am + D[2][j][k];
	    ddC[k][j] = ddC[j][k];
	}
    }

    //logfp << "m: " << m << " am: " << am << " beta[0]: " << beta[0] << " beta[2]: " << beta[2] << endl;
    //logfp << "Ci: " << *Ci << endl;
    //logfp << "dC: " << dC[0] << ' ' << dC[1] << ' ' << dC[2] << ' ' << dC[3] << endl;
    //logfp << "ddC: " << endl;
    //logfp << ddC[0][0] << ' ' << ddC[0][1] << ' ' << ddC[0][2] << ' ' << ddC[0][3] << endl
          //<< ddC[1][0] << ' ' << ddC[1][1] << ' ' << ddC[1][2] << ' ' << ddC[1][3] << endl
          //<< ddC[2][0] << ' ' << ddC[2][1] << ' ' << ddC[2][2] << ' ' << ddC[2][3] << endl
          //<< ddC[3][0] << ' ' << ddC[3][1] << ' ' << ddC[3][2] << ' ' << ddC[3][3] << endl;
}

void
pred_info_t::update_Xparms(double grad[4], double Hess[4][4])
{
    double H_data[] = { Hess[0][0], Hess[0][1], Hess[0][2], Hess[0][3], 
			Hess[1][0], Hess[1][1], Hess[1][2], Hess[1][3], 
			Hess[2][0], Hess[2][1], Hess[2][2], Hess[2][3], 
			Hess[3][0], Hess[3][1], Hess[3][2], Hess[3][3]};

    gsl_vector_view vec = gsl_vector_view_array(grad, 4);
    gsl_matrix_view Mat = gsl_matrix_view_array(H_data, 4, 4);
    //gsl_matrix * V = gsl_matrix_alloc(4,4);
    gsl_vector * tau = gsl_vector_alloc (4);
    gsl_vector * change = gsl_vector_alloc (4);

    gsl_linalg_QR_decomp (&Mat.matrix, tau);
    gsl_linalg_QR_solve (&Mat.matrix, tau, &vec.vector, change);
    
    logfp << "change: " << gsl_vector_get(change,0) << " "
    << gsl_vector_get(change,1) << " "
    << gsl_vector_get(change,2) << " "
    << gsl_vector_get(change,3) << endl;

    for (unsigned i = 0; i < 3; ++i) {
        t[i] -= gsl_vector_get(change, i);
    }

    scale_t();
    calc_beta();
    alpha -= gsl_vector_get(change, 3);

    gsl_vector_free (change);
    gsl_vector_free (tau);
    //gsl_matrix_free (V);
}

double
pred_info_t::newton_raphson_X()
{
    double d[3][3];    // first order beta derivatives
    double D[3][3][3]; // second order beta derivatives
    double dB[4];      // first order derivatives of B_i
    double ddB[4][4];  // second order derivatives of B_i
    double dC[4];
    double ddC[4][4];
    double as = (double) S-Y;
    double at = (double) Y;
    double log1a = log(1.0-alpha);
    double loga = log(alpha);
    double c1 = (double) Asize;
    double c2 = (double) Bsize;
    double c3 = (double) Csize;
    double L = as*log1a + at*loga 
	+ 2.0*c1*log(beta[0]) + c2 * log(beta[1]) + c3 * log(beta[2])
        + (double) Asumtrue * loga + (double) (Asumobs - Asumtrue) * log1a;

    beta_der(d, D);
    //logfp << "d: " << endl;
    //logfp << d[0][0] << ' ' << d[0][1] << ' ' << d[0][2] << endl
          //<< d[1][0] << ' ' << d[1][1] << ' ' << d[1][2] << endl
          //<< d[2][0] << ' ' << d[2][1] << ' ' << d[2][2] << endl;
    //logfp << "D[0]: " << endl;
    //logfp << D[0][0][0] << ' ' << D[0][0][1] << ' ' << D[0][0][2] << endl
          //<< D[0][1][0] << ' ' << D[0][1][1] << ' ' << D[0][1][2] << endl
          //<< D[0][2][0] << ' ' << D[0][2][1] << ' ' << D[0][2][2] << endl;
    //logfp << "D[1]: " << endl;
    //logfp << D[1][0][0] << ' ' << D[1][0][1] << ' ' << D[1][0][2] << endl
          //<< D[1][1][0] << ' ' << D[1][1][1] << ' ' << D[1][1][2] << endl
          //<< D[1][2][0] << ' ' << D[1][2][1] << ' ' << D[1][2][2] << endl;
    //logfp << "D[2]: " << endl;
    //logfp << D[2][0][0] << ' ' << D[2][0][1] << ' ' << D[2][0][2] << endl
          //<< D[2][1][0] << ' ' << D[2][1][1] << ' ' << D[2][1][2] << endl
          //<< D[2][2][0] << ' ' << D[2][2][1] << ' ' << D[2][2][2] << endl;


    // derivative ordering: t[0], t[1], t[2], alpha
    double grad[4];
    double Hess[4][4];

    // initialize the derivatives with things we can compute in bulk
    for (unsigned j = 0; j < 3; ++j) {
	grad[j] = 2.0* c1/beta[0]*d[0][j] + c2/beta[1]*d[1][j] + c3/beta[2]*d[2][j];
	Hess[j][3] = 0.0;
	for (unsigned k = j; k < 3; ++k) {
	    Hess[j][k] = 
		2.0*c1 / beta[0] * (D[0][j][k] - d[0][j]*d[0][k]/beta[0])
		+ c2/beta[1] * (D[1][j][k] - d[1][j]*d[1][k]/beta[1])
		+ c3/beta[2] * (D[2][j][k] - d[2][j]*d[2][k]/beta[2]);
	}
    }

    // initialize the alpha derivatives
    grad[3] = (at+ (double) Asumtrue)/alpha - (as + (double) (Asumobs - Asumtrue))/(1.0-alpha);
    Hess[3][3] = -(at + (double) Asumtrue)/(alpha*alpha) 
	- (as + (double) (Asumobs - Asumtrue))/((1.0-alpha)*(1.0-alpha));

    //logfp << "grad (before): " << grad[0] << ' ' << grad[1] << ' ' << grad[2] << ' ' << grad[3] << endl;

    // Add the terms involving set B
    for (DiscreteDist::iterator c = Bset.begin(); c != Bset.end(); ++c) {
	double m = (double) c->first;
	double count = (double) c->second;
	double Bi;
	B_der(dB, ddB, d, D, m, &Bi);
	L += count * log(Bi);
	for (unsigned j = 0; j < 4; ++j) {
	    grad[j] += count * dB[j] / Bi;
	    for (unsigned k = j; k < 4; ++k) {
		Hess[j][k] += count * (ddB[j][k]/Bi - dB[j]*dB[k]/(Bi*Bi));
	    }
	}
    }

    // Add the terms involving set C
    for (DiscreteDist::iterator c = Cset.begin(); c != Cset.end(); ++c) {
	double m = (double) c->first;
	double count = (double) c->second;
	double Ci;
	C_der(dC, ddC, d, D, m, &Ci);
	L += count * log (Ci);
	for (unsigned j = 0; j < 4; ++j) {
	    grad[j] += count * dC[j] / Ci;
	    for (unsigned k = j; k < 4; ++k) {
		Hess[j][k] += count * (ddC[j][k]/Ci - dC[j]*dC[k]/(Ci*Ci));
	    }
	}
    }

    // make Hess symmetric
    for (unsigned j = 0; j < 4; ++j) 
	for (unsigned k = j+1; k < 4; ++k)
	    Hess[k][j] = Hess[j][k];

    logfp << "Grad: " << grad[0] << ' ' << grad[1] << ' ' << grad[2] << ' ' << grad[3] << endl;
    logfp << "Hess: " << endl <<
          Hess[0][0] << ' ' << Hess[0][1] << ' ' << Hess[0][2] << ' ' << Hess[0][3] << endl <<
   	  Hess[1][0] << ' ' << Hess[1][1] << ' ' << Hess[1][2] << ' ' << Hess[1][3] << endl <<
	  Hess[2][0] << ' ' << Hess[2][1] << ' ' << Hess[2][2] << ' ' << Hess[2][3] << endl << 
	  Hess[3][0] << ' ' << Hess[3][1] << ' ' << Hess[3][2] << ' ' << Hess[3][3] << endl;

    update_Xparms(grad, Hess);

    return L;
}

bool
pred_info_t::est_Xparms() 
{
    double alphap;
    double betap[3];
    double L0 = (std::numeric_limits<double>::min)(), L1;
    unsigned niter = 0;
    double change;
    unsigned nbzero = 0;

    if (Asize == 0) {  // didn't observe and runs where m > y > 0, just use initial values
       return (0);
    }

    for (unsigned i = 0; i < 3; ++i)
      if (beta[i] == 0.0)
        nbzero++;

    if (nbzero == 1) {
        beta[0] = (double) (Asize+1.0)/(Asize + Bsize + Csize + 3.0);
        beta[1] = (double) (Bsize+1.0)/(Asize + Bsize + Csize + 3.0);
        beta[2] = (double) (Csize+1.0)/(Asize + Bsize + Csize + 3.0);
        for (unsigned i = 0; i < 3; ++i)
          t[i] = log(beta[i]);
        scale_t();
    }
    else if (nbzero > 1) 
      return 0;
     
    if (alpha == 0.0)
      alpha = 0.5;

    logfp << "L0 " << L0 << " alpha0: " << alpha << 
	" t0: " << t[0] << ' ' << t[1] << ' ' << t[2] << endl;
    do {
	alphap = alpha;
	betap[0] = beta[0];
	betap[1] = beta[1];
	betap[2] = beta[2];
	L1 = newton_raphson_X();
	project(&alpha, 0.0, 1.0);
	logfp << "L1: " << L1 << " alpha1: " << alpha << " t1: " 
	      << t[0] << ' ' << t[1] << ' ' << t[2] << " beta: " 
              << beta[0] << ' ' << beta[1] << ' ' << beta[2] << endl;
	change = abs(alphap - alpha) + abs(betap[0] - beta[0]) + abs(betap[1] - beta[1]) + abs(betap[2] - beta[2]) ;
	niter++;
	L0 = L1;
    } while (change > thresh && niter < MAXITER);

    if (niter == MAXITER) {
	logfp << "Newton-Raphson did not converge\n";
	return 1;
    }

    return 0;
}

inline void
pred_info_t::map_estimate() {
    checkstatus(est_Nparms(), "Map estimates of N prior parms did not converge");
    checkstatus(est_Xparms(), "Map estimates of X prior parms did not converge");
}

void estimate_parms()
{
    currPred.init_parms();

    logfp << "Pred: " << currCoords << '\n' << "Info (f): " << currPred.f << endl;
    currPred.f.map_estimate();

    logfp << "Pred: " << currCoords << '\n' << "Info (s): " << currPred.s << endl;
    currPred.s.map_estimate();
}

/****************************************************************************
 * Print results to file
 ***************************************************************************/

inline ostream &
operator<< (ostream &out, const pred_info_t &pi)
{
    out << pi.S << ' ' << pi.N << ' ' << pi.Y << ' ' << pi.Z << ' '
	<< pi.a << ' ' << pi.b << ' ' << pi.rho << ' '
	<< pi.Asize << ' ' << pi.Asumtrue << ' ' << pi.Asumobs << ' ' 
	<< pi.Bsize << ' ' << pi.Csize << ' ' 
	<< scientific << setprecision(12) 
	<< pi.alpha << ' ' << pi.beta[0] << ' ' << pi.beta[1] << ' ' << pi.beta[2] << ' ' 
	<< pi.lambda << ' ' << pi.gamma;
    out.unsetf(ios::floatfield);
    return out;
}

inline ostream &
operator << (ostream &out, const PredPair &pp)
{
    out << pp.f << endl << pp.s;
    return out;
}

/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}


/****************************************************************************
 * GSL Random Number Generator
 ***************************************************************************/

void init_gsl_generator ()
{
  FILE * const entropy = fopen("/dev/urandom", "rb");
  if (!entropy) {
    cerr << "Cannot open system entropy" << '\n';
    exit(1);
  }
  if (fread(&gsl_rng_default_seed, sizeof(gsl_rng_default_seed), 1, entropy) != 1) {
    cerr << "Cannot read system entropy" << '\n';
    exit(1);
  }
  fclose(entropy);

  generator = gsl_rng_alloc(gsl_rng_taus);
}

inline void free_gsl_generator ()
{
  gsl_rng_free(generator);
}


/****************************************************************************
 * Driver routine
 ***************************************************************************/
void
driver (char *infn, char *outfn)
{
    ofstream parmsfp (outfn, ios_base::trunc);

    FILE *fp = fopenRead(infn);
    if (!fp) {
	cerr << "Cannot open " << infn << " for reading\n";
	exit(1);
    }
    
    while (true) {
	if(! read_stats(fp))
	    break;
	estimate_parms();
	parmsfp << currCoords << endl << currPred << endl;
    }

    fclose(fp);
    parmsfp.close();

}

/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    init_gsl_generator();

    driver("parmstats.txt", "hyperparms.txt");
    driver("notp-parmstats.txt", "notp-hyperparms.txt");

    logfp.close();
    free_gsl_generator();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
