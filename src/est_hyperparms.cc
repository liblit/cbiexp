#include <argp.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ext/hash_map>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "fopen.h"
#include "NumRuns.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "classify_runs.h"
#include "sites.h"
#include "units.h"

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
 * Site-specific information
 ***************************************************************/

struct site_info_t {
    unsigned S, N, Z;
    double a, b;
    double rho;
    double lambda, beta;
    site_info_t () {
	S = N = Z = 0;
	a = b = 0.0;
	rho = 1.0;
	lambda = 1.0;
	beta = 0.5;
    }
    void init_parms() {
	//beta = gsl_rng_uniform(generator);
	//lambda = gsl_rng_uniform(generator)*(b-a) + a;
    }	
};

class SitePair
{
public:
  site_info_t f;
  site_info_t s;
  void init_parms() {
    f.init_parms();
    s.init_parms();
  }
};

class site_hash_t : public hash_map<SiteCoords, SitePair>
{
};

static site_hash_t siteHash;

inline ostream &
operator<< (ostream &out, const site_info_t &si)
{
  out << si.S << '\t' << si.N << '\t' << si.Z << '\t'
      << si.a << '\t' << si.b << '\t' << si.rho << '\t'
      << si.lambda << '\t' << si.beta;
  return out;
}


/****************************************************************************
 * Utilities
 ***************************************************************************/
void
read_stats()
{
  FILE *fp = fopenRead("parmstats.txt");
  if (!fp) {
    cerr << "Cannot open parmstats.txt for reading\n";
    exit(1);
  }

  SiteCoords coords;
  SitePair sp;
  int res;
  
  while (true) {
    res = fscanf(fp, "%u\t%u\n", &coords.unitIndex, &coords.siteOffset);
    if (feof(fp))
      break;
    assert(res == 2);

    res = fscanf(fp, "%u\t%u\t%u\t%lg\t%lg\t%lg\n", &sp.f.S, &sp.f.N, &sp.f.Z,
                 &sp.f.a, &sp.f.b, &sp.f.rho);
    assert(res = 6); 
    sp.f.lambda = (double) sp.f.S / sp.f.N;

    res = fscanf(fp, "%u\t%u\t%u\t%lg\t%lg\t%lg\n", &sp.s.S, &sp.s.N, &sp.s.Z,
                 &sp.s.a, &sp.s.b, &sp.s.rho);
    assert(res = 6); 
    sp.s.lambda = (double) sp.s.S / sp.s.N;

    siteHash[coords] = sp;
  }

  fclose(fp);
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
newton_raphson(site_info_t &si, double lam0, double beta0, double *lam1, double *beta1)
{
    const double rho = si.rho;
    const double S = (double) si.S;
    const double N = (double) si.N;
    const double Z = (double) si.Z;
    double ex = exp(-lam0*rho);
    double C = ex - 1.0;
    double A = 1.0 + C * beta0;
    double E = -rho*ex;
    double B = beta0 * E;
    double D = B * (-rho);
    double t1 = Z/A;
    // likelihood
    double L = N*log(beta0) - lam0*rho*N + S*log(lam0)+ Z*log(A);

    // gradient
    double a = -rho*N + S/lam0 + t1*B;  // partial wrt lambda
    double b = N/beta0 + t1*C;         // partial wrt beta
    // Hessian
    double aa = -S/(lam0*lam0) - t1*(B*B/A - D);
    double bb = -t1*(B*C/A - E);
    double dd = -N/(beta0*beta0) - t1*(C*C/A);
    double det = 1.0/(aa*dd-bb*bb);

    //logfp << "a: " << a << " b: " << b << " aa: " << aa << " bb: " << bb << " dd: " << dd << " det: " << det << endl;

    *lam1 = lam0 - det *(dd*a-bb*b);
    *beta1 = beta0 - det * (-bb*a + aa*b);

    return L;
}

bool map_estimate(site_info_t &si) 
{
  if (si.N == 0) {
    si.beta = 0.0;
    return 0;
  }

  double lam0 = si.lambda;
  double beta0 = si.beta;
  double rho = si.rho;
  const double a = si.a;
  const double b = si.b;
  double lam1 = lam0;
  double beta1 = beta0;
  double L0 = (std::numeric_limits<double>::min)(), L1;
  unsigned niter = 0;

  logfp << "L0 " << L0 << " lam0: " << lam0 << " beta0: " << beta0 << endl;
  do {
      lam0 = lam1;
      beta0 = beta1;
      L1 = newton_raphson(si, lam0, beta0, &lam1, &beta1);
      //assert(L1-L0 >= 0.0);
      project(&beta1, 0.0, 1.0);
      project(&lam1, a, b/rho);
      logfp << "L1: " << L1 << " lam1: " << lam1 << " beta1: " << beta1 << endl;
      niter++;
      L0 = L1;
  } while (abs(lam0-lam1)+abs(beta0-beta1) > thresh && niter < MAXITER);

  if (niter == MAXITER) {
      logfp << "Newton-Raphson did not converge\n";
      return 1;
  }

  si.lambda = lam1;
  si.beta = beta1;
  return 0;
}

void estimate_parms()
{
  for (site_hash_t::iterator c = siteHash.begin(); c != siteHash.end(); c++)
  {
    SitePair &sp = c->second;

    //sp.init_parms();
    logfp << "Site: " << c->first << '\n' << "Info (f): " << sp.f << endl;
    if (map_estimate(sp.f))
    {
      logfp << "Map estimate of parameters of site " << c->first << '\t' <<  sp.f << " did not converge\n";
      logfp.close();
      exit(1);
    }

    logfp << "Site: " << c->first << '\n' << "Info (s): " << sp.s << endl;
    if (map_estimate(sp.s))
    {
      logfp << "Map estimate of parameters of site " << c->first << '\t' <<  sp.s << " did not converge\n";
      logfp.close();
      exit(1);
    }
  }
}

/****************************************************************************
 * Print results to file
 ***************************************************************************/


inline ostream &
operator<< (ostream &out, const site_hash_t &sh)
{
    for (site_hash_t::const_iterator c = sh.begin(); c != sh.end(); c++) {
	const SiteCoords sc = c->first;
	const SitePair sp = c->second;
	out << sc << '\n' << sp.f << '\n' << sp.s << '\n';
    }
    return out;
}

void print_results()
{
    ofstream parmsfp ("hyperparms.txt", ios_base::trunc);
    parmsfp << siteHash;
    parmsfp.close();
}


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
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
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    init_gsl_generator();

    read_stats();
    estimate_parms();
    print_results();

    logfp.close();
    free_gsl_generator();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
