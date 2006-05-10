#include <cassert>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>
#include <gsl/gsl_rng.h>
#include "CompactReport.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "StaticSiteInfo.h"
#include "classify_runs.h"
#include "utils.h"

using namespace std;

/****************************************************************************
 * Definition of necessary constants, functions, structures, and classes
 ***************************************************************************/
static unsigned MAX_INT = 1<<31;
unsigned cur_run;
static unsigned num_train_runs = 0;
static unsigned num_val_runs = 0;
static vector<bool> is_trainrun;
static vector<bool> is_valrun;

static StaticSiteInfo staticSiteInfo;

/***********************
 * site_info
 **********************/
struct site_info_t {
  count_tp min[4], max[4];
  double mean[4], var[4];
  bool retain[4];
  unsigned ntallied;
  site_info_t ()
  {
    for (int i = 0; i < 4; i++) {
      mean[i] = 0.0;
      var[i] = 0.0;
      min[i] = MAX_INT;
      max[i] = 0;
      retain[i] = false;
    }
    ntallied = 0;
  }
};

static vector<vector<site_info_t> > site_info;

/**********************
 * Reader
 *********************/
class Reader : public ReportReader
{
protected:
  void handleSite(const SiteCoords &, vector<count_tp> &);
};

inline void obs (const SiteCoords &coords) 
{
    site_info_t &site = site_info[coords.unitIndex][coords.siteOffset];
    site.ntallied += 1;
}

inline void inc (const SiteCoords &coords, unsigned p, count_tp count)
{
  site_info_t &site = site_info[coords.unitIndex][coords.siteOffset];
  unsigned n = site.ntallied;  // n should have just been increased by obs()
  site.mean[p] = site.mean[p] * ((double) (n-1.0) / n) 
      + (double) count / n;
  site.var[p] = site.var[p] * ((double) (n-1.0)/n) 
      + (double) count / n * count;
  site.min[p] = (count < site.min[p]) ? count : site.min[p];
  site.max[p] = (count > site.max[p]) ? count : site.max[p];
}

void Reader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
{
  obs(coords);
  for (unsigned predicate = 0; predicate < counts.size(); ++predicate)
      inc(coords, predicate, counts[predicate]);
}

/****************************************************************************
 * Procedures for deciding whether to retain/discard
 * each instrumented predicate
 ***************************************************************************/
inline void adj_for_zeros(int u, int c, int p) {
    site_info_t &site = site_info[u][c];
    unsigned n = site.ntallied;
    // (num_trainruns - ntallied) is the number of omitted zeros
    double adj = (double)  n/num_train_runs;
    double adj2 = (double) num_train_runs/(num_train_runs-1.0);
    site.mean[p] = site.mean[p] * adj;
    site.var[p] = site.var[p] * adj;
    site.var[p] = (site.var[p] - site.mean[p]*site.mean[p])*adj2;
    site.min[p] = (0 < site.min[p]) ? 0 : site.min[p];
    // the maximum is set to zero by default
}

inline void cull(int u, int c, int p) {
  // adjust the stats for the zeros omitted in the compact report
  adj_for_zeros(u,c,p);

  double var = site_info[u][c].var[p];
  if (var < 0.0) {
    cerr << "Variance is negative: " << u << ' ' << c << ' ' << p << ' ' << var << '\n';
  }

  if (isinf(var) || isnan(var)) {
    cerr << "Variance is infinite or nan: " << u << ' ' << c << ' ' << p << ' ' << var << '\n';
  }

  if (var > 0.0)
    site_info[u][c].retain[p] = true;
}

static void cull_preds()
{
    for (unsigned u = 0; u < staticSiteInfo.unitCount; u++) {
	const unit_t &unit = staticSiteInfo.unit(u);
	for (unsigned c = 0; c < unit.num_sites; c++) {
	    int numPreds;
	    switch (unit.scheme_code) {
	    case 'B':
		numPreds = 2;
		break;
	    case 'F':
		numPreds = 9;
		break;
	    case 'G':
		numPreds = 4;
		break;
	    case 'R':
	    case 'S':
		numPreds = 3;
		break;
	    default:
		assert(0);
	    }
	    for (int p = 0; p < numPreds; p++)
		cull(u, c, p);
	}
    }
}

/****************************************************************************
 * Printing information
 ***************************************************************************/

void print_retained_preds()
{
  ofstream fp("nonconst_preds.txt");
  for (unsigned u = 0; u < site_info.size(); u++) {
    for (unsigned c = 0; c < site_info[u].size(); c++) {
      const site_info_t &site = site_info[u][c];
      for (int p = 0; p < 4; p++) {
	if (site.retain[p]) {
	  fp << u << ' ' << c << ' ' << p << ' ' << site.mean[p] << ' '
	     << site.var[p] << ' ' << site.min[p] << ' ' << site.max[p] 
	     << ' ' << site.ntallied 
	     << '\n';
	}
      }
    }
  }

  fp.close();
}

void print_runsplit ()
{
  ofstream tfp ("train.runs");
  ofstream vfp ("val.runs");
  for (unsigned i = NumRuns::begin; i < NumRuns::end; i++) {
    assert(! (is_trainrun[i] && is_valrun[i]));

    if (is_trainrun[i])
      tfp << i << '\n';

    if (is_valrun[i])
      vfp << i << '\n';
  }

  tfp.close();
  vfp.close();
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
 * GSL Random Number Generator
 ***************************************************************************/

static gsl_rng *generator;

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

/***********************************************************
 * split runs into training and validation sets
 **********************************************************/
void split_runs()
{
  is_trainrun.resize(NumRuns::end);
  is_valrun.resize(NumRuns::end);

  for (unsigned r = NumRuns::begin; r < NumRuns::end; r++) {
    if (!is_srun[r] && !is_frun[r])  // skip discarded runs
      continue;

    double prob = gsl_rng_uniform(generator);
    if (prob < 0.9) {
      is_trainrun[r] = true;
      num_train_runs++;
    }
    else {
      is_valrun[r] = true;
      num_val_runs++;
    }
  }
}

/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    init_gsl_generator();
    classify_runs();
    split_runs();

    site_info.resize(staticSiteInfo.unitCount);
    for (unsigned u = 0; u < staticSiteInfo.unitCount; u++)
	site_info[u].resize(staticSiteInfo.unit(u).num_sites);

    Progress::Bounded progress("computing non-constant predicates", NumRuns::count());
    for (cur_run = NumRuns::begin; cur_run < NumRuns::end; cur_run++) {
	progress.step();
	if (!is_trainrun[cur_run])
	  continue;

	Reader().read(cur_run);
    }

    cull_preds();

    print_retained_preds();
    print_runsplit();

    free_gsl_generator();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
