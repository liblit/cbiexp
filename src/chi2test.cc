#include <argp.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <queue>
#include <math.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include "DiscreteDist.h"
#include "fopen.h"
#include "SiteCoords.h"

using namespace std;
using __gnu_cxx::hash_map;

/****************************************************************
 * Global variables and constants
 ***************************************************************/
const double alpha = 0.95;  // hypothesis test significance level
ofstream logfp("chi2test.log",ios_base::trunc);

/****************************************************************
 * Site-specific information
 ***************************************************************/

struct site_info_t {
    double rho;
    double lambda, beta;
    unsigned nvals;
    double chi2stat, chi2critval, chi2resid;
    site_info_t () {
	rho = lambda = beta = 0.0;
	nvals = 0;
	chi2stat = chi2critval = chi2resid = 0.0;
    }
};

class SiteInfoPair
{
public:
  site_info_t f;
  site_info_t s;
};

typedef site_info_t (SiteInfoPair::* SiteInfo);

class site_hash_t : public hash_map<SiteCoords, SiteInfoPair>
{
};

static site_hash_t siteHash;
static queue<SiteCoords> siteSel;
static DiscreteDist dist;

inline ostream &
operator<< (ostream &out, const site_info_t &si)
{
  out << si.rho << '\t' << si.lambda << '\t' << si.beta << '\t'
      << si.nvals << '\t'
      << si.chi2stat << '\t' << si.chi2critval << '\t' << si.chi2resid;
  return out;
}

/****************************************************************************
 * Utilities
 ***************************************************************************/
double rexp(const site_info_t &si, count_tp r, count_tp n) 
{
  const double lambda = si.lambda;
  const double beta = si.beta;
  const double rho = 1.0;//si.rho;
  const double mu = lambda*rho;
  double expectation = 0.0;

  if (r == 0)
    expectation = (double) n*exp(-mu)*beta + (double) n*(1.0-beta);
  else
    expectation = (double) n*gsl_ran_poisson_pdf(r,mu)*beta;

  return expectation;
}

void
read_parms()
{
  FILE *fp = fopenRead("parms.txt");
  if (!fp) {
    cerr << "Cannot open parms.txt for reading\n";
    exit(1);
  }

  SiteCoords coords;
  SiteInfoPair sp;
  unsigned N, Z;
  count_tp S;
  double a, b;
  int ctr;
  
  while (true) {
    ctr = fscanf(fp, "%u\t%u\n", &coords.unitIndex, &coords.siteOffset);
    if (feof(fp))
      break;
    assert(ctr == 2);

    ctr = fscanf(fp, "%Lu\t%u\t%u\t%lg\t%lg\t%lg\t%lg\t%lg\n", &S, &N, &Z, &a, &b, &sp.f.rho, &sp.f.lambda, &sp.f.beta);
    assert(ctr == 8);

    ctr = fscanf(fp, "%Lu\t%u\t%u\t%lg\t%lg\t%lg\t%lg\t%lg\n", 
           &S, &N, &Z, &a, &b, &sp.s.rho, &sp.s.lambda, &sp.s.beta);
    assert(ctr == 8);

    siteHash[coords] = sp;
  }

  fclose(fp);
}

bool read_dist (FILE * const fp, SiteInfo si, bool push) 
{
  SiteCoords coords;
  unsigned nvals, of;
  count_tp val, N=0;
  double ef;
  double chi2stat = 0.0;
  int ctr;

  ctr = fscanf(fp, "%u\t%u\n", &coords.unitIndex, &coords.siteOffset);
  if (feof(fp))
    return true;
  assert(ctr == 2);

  // find the estimated parameters for that site
  site_hash_t::iterator found = siteHash.find(coords);
  if (found == siteHash.end()) {
    cerr << "Cannot find estimated params for site " << coords << endl;
    exit(1);
  }
  SiteInfoPair &sp = found->second;

  ctr = fscanf(fp, "%u\n", &nvals);
  assert(ctr == 1);
  (sp.*si).nvals = nvals;
  logfp << coords << endl << (sp.*si) << endl;

  dist.clear();
  // read the observed frequencies
  for (unsigned i = 0; i < nvals; i++) {
    ctr = fscanf(fp, "%Lu %u ", &val, &of);
    assert(ctr == 2);
    dist[val] = of;
    N += of;
  }
  
  // calculate expected frequencies and compute chi2 stats
  for (DiscreteDist::iterator c = dist.begin(); c != dist.end(); ++c)
  {
    val = c->first;
    of = c->second;
    ef = rexp(sp.*si, val, N);
    chi2stat += (of-ef)*(of-ef)/ef;
    logfp << val << ' ' << of << ' ' << ef << ' ';
  }
  logfp << endl;

  (sp.*si).chi2critval = gsl_cdf_chisq_Pinv(alpha, nvals-3); // two parameters + 1
  (sp.*si).chi2stat = chi2stat;
  (sp.*si).chi2resid = (sp.*si).chi2stat - (sp.*si).chi2critval;
   
  if (push) 
    siteSel.push(coords);

  return false;
}

// Read in the distributions of sites selected in stage 1
// compute chi-square statistic as we go along
void read_true_dists()
{
  FILE * const ffp = fopenRead("fpriors-full.dat");
  FILE * const sfp = fopenRead("spriors-full.dat");
  
  while (true) {
    if (read_dist(ffp, &SiteInfoPair::f, true))
      break;
    if (read_dist(sfp, &SiteInfoPair::s, false)) {
      cerr << "Expecting more distributions in spriors-full.dat\n";
      exit(1);
    }
  }

  fclose(ffp);
  fclose(sfp);
}

/****************************************************************************
 * Print results to file
 ***************************************************************************/

void print_results()
{
    ofstream ofp ("chi2stats.txt", ios_base::trunc);
    while (!siteSel.empty()) {
	const SiteCoords &coords = siteSel.front();
        site_hash_t::const_iterator found = siteHash.find(coords);
        if (found == siteHash.end()) {
          cerr << "Cannot find info for site " << coords << endl;
          exit(1);
        }
        const SiteInfoPair sp = found->second;
        ofp << coords << '\n' << sp.f << '\n' << sp.s << '\n';
        siteSel.pop();
    }
    ofp.close();
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
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    read_parms();

    read_true_dists();

    print_results();

    logfp.close();
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
