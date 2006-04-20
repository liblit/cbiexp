/****************************************************************************
 * Cluster the runs using K-means
 */

#include <argp.h>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ext/hash_map>
#include <vector>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_permute.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fopen.h"
#include "CompactReport.h"
#include "NumRuns.h"
#include "Progress/Unbounded.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "classify_runs.h"
#include "sites.h"
#include "units.h"
#include "PredCoords.h"
#include "Nonconst_PredStats.h"

using namespace std;
using __gnu_cxx::hash_map;

/****************************************************************************
 * Definitions of constants, structs, and classes
 ***************************************************************************/
const unsigned K = 3;
static gsl_rng *generator;

/****************************************************************
 * Information about non-constant predicates
 ***************************************************************/
struct pred_info_t {
    float mean, var, std;
    unsigned min, max, ntallied;
    double val;
    pred_info_t () {
	mean = var = std = 0.0;
	min = max = ntallied = 0;
	val = 0.0;
    }
    pred_info_t (const pred_info_t &p) {
      mean = p.mean; var = p.var; std = p.std;
      min = p.min; max = p.max; ntallied = p.ntallied;
      val = p.val;
    }
    pred_info_t & operator=(const pred_info_t &p) {
      mean = p.mean; var = p.var; std = p.std;
      min = p.min; max = p.max; ntallied = p.ntallied;
      val = p.val;
      return (*this);
    }
};

class pred_hash_t : public hash_map<PredCoords, pred_info_t>
{
};


static pred_hash_t PredHash;
vector<pred_hash_t> centers(K);

inline ostream &
operator<< (ostream &out, const pred_hash_t &ph)
{
    for (pred_hash_t::const_iterator c = ph.begin(); c != ph.end(); c++) {
	const PredCoords pc = c->first;
	const pred_info_t pi = c->second;
        //if (pi.var > 0.0)
	  out << pc.unitIndex << ' ' << pc.siteOffset << ' ' << pc.predicate << ' '
              << pi.mean << ' ' << pi.var << ' ' << pi.min << ' '
              << pi.max << ' ' << pi.ntallied << endl;
    }
    return out;
}

inline void 
adjust_val(const pred_info_t &pstat, double &val) 
{
  val = (val - pstat.mean)/pstat.std;
}

void 
update(pred_hash_t &oldhash, const pred_hash_t &newhash)
{
  // go through the list of all retained predicates
  for (pred_hash_t::const_iterator c = PredHash.begin(); c != PredHash.end(); ++c)
  {
    const PredCoords &pc = c->first;
    pred_hash_t::const_iterator fnew = newhash.find(pc);
    if (fnew != newhash.end()) {  // nothing to do if new hash doesn't have this predicate
      const pred_info_t &newinfo = fnew->second;
      pred_hash_t::iterator fold = oldhash.find(pc);
      if (fold == oldhash.end()) {  // insert it if it doesn't already exist
        oldhash[pc] = newinfo;
      }
      else {  // otherwise add the new value
        pred_info_t &oldinfo = fold->second;
        oldinfo.val += newinfo.val;
      }
    }
  }
}

void divideby(pred_hash_t &hash, double v)
{
  for (pred_hash_t::iterator c = hash.begin(); c != hash.end(); c++) {
    pred_info_t &pi = c->second;
    pi.val /= v;
  }
}

/****************************************************************
 * Information about each run
 ***************************************************************/

struct run_info_t {
    pred_hash_t preds;
    int runId;
    unsigned mode;
    run_info_t() {
	preds.clear();
	runId = -1;
        mode = 0;
    }
};

static run_info_t curr_run;

enum {CHECK, NO_CHECK};

class Reader : public ReportReader
{
public:
  Reader(unsigned, unsigned);
  void branchesSite(const SiteCoords &, unsigned, unsigned);
  void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned);
  void returnsSite(const SiteCoords &, unsigned, unsigned, unsigned);
  void scalarPairsSite(const SiteCoords &, unsigned, unsigned, unsigned);

private:
  void tripleSite(const SiteCoords &, unsigned, unsigned, unsigned) const;
  void insert_val(const SiteCoords &, unsigned, unsigned) const;
};

inline
Reader::Reader(unsigned r, unsigned m)
{
    curr_run.preds.clear();
    curr_run.runId = r;
    curr_run.mode = m;
}

void Reader::insert_val(const SiteCoords &coords, unsigned offset, unsigned val) const
{
    PredCoords pcoords(coords,offset);
    pred_hash_t::iterator found = PredHash.find(pcoords);
    switch (curr_run.mode) {
    case NO_CHECK:  // don't check in PredHash, just collect the statistics
      if (found == PredHash.end()) {
        pred_info_t pi;
        pi.ntallied = 1;
        pi.mean = (double) val;
        pi.var = (double) val * val;
        pi.min = pi.max = val;
        PredHash[pcoords] = pi;
      }
      else {
        pred_info_t &pi = found->second;
        unsigned n = pi.ntallied;
        double frac = (double) n / (n+1.0);
        pi.ntallied++;
        pi.mean = pi.mean * frac + (double) val / (n+1.0);
        pi.var = pi.var * frac + (double) val / (n+1.0) * val;
        pi.min = (val < pi.min) ? val : pi.min;
        pi.max = (val > pi.max) ? val : pi.max;
      }
      break;
    case CHECK:  // only keep the predicates listed in PredHash (those with nonconst variance)
      if (found != PredHash.end()) {
        const pred_info_t &pstat = found->second;
        pred_info_t pinfo;
        pinfo.val = val;
        adjust_val(pstat, pinfo.val);
        curr_run.preds[pcoords] = pinfo;
      }
      break;
    default:
      cerr << "Unknown mode: " << curr_run.mode << endl;
      break;
    }
}

void Reader::tripleSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) const
{
    assert(x || y || z);
    if (x > 0)
	insert_val(coords, 0, x);
    if (y > 0)
	insert_val(coords, 1, y);
    if (z > 0)
	insert_val(coords, 2, z);
}

inline void
Reader::scalarPairsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) 
{
    tripleSite(coords, x, y, z);
}


inline void
Reader::returnsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) 
{
    tripleSite(coords, x, y, z);
}

inline void
Reader::branchesSite(const SiteCoords &coords, unsigned x, unsigned y)
{
    assert(x||y);
    if (x > 0)
	insert_val(coords, 0, x);
    if (y > 0)
	insert_val(coords, 1, y);
}

inline void
Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z, unsigned w)
{
    assert(x || y || z || w);
    if (x > 0)
	insert_val(coords, 0, x);
    if (y > 0)
	insert_val(coords, 1, y);
    if (z > 0)
	insert_val(coords, 2, z);
    if (w > 0)
	insert_val(coords, 3, w);
}

/****************************************************************************
 * Reading and collecting pred stats
 ***************************************************************************/
bool read_nonconst_pred (FILE *fp, PredCoords &pc, pred_info_t &pi) 
{
    unsigned ntallied;
    const int got = fscanf(fp, "%u %u %u %g %g %u %u %u\n", &pc.unitIndex, &pc.siteOffset,
			   &pc.predicate, &pi.mean, &pi.var, &pi.min, &pi.max, &ntallied);
    
    pi.std = sqrt(pi.var);

    if (got == 8)
	return true;
    else
	return false;
}

void verify_stats () 
{
    for (pred_hash_t::iterator c = PredHash.begin(); c != PredHash.end(); c++) {
	const pred_info_t &pi = c->second;
	assert(pi.var > 0.0);
	assert(pi.mean > 0.0);
	assert(pi.max - pi.min > 0);
    }
}

void 
adjust_stats()
{
  // account for all the implicit zeros in the sparse reports,
  // and normalize the stats
  for (pred_hash_t::iterator c = PredHash.begin(); c != PredHash.end(); ++c) {
    pred_info_t &pi = c->second;
    unsigned n = pi.ntallied;
    double adj = (double) n/num_fruns;
    double adj2 = (double) num_fruns / (num_fruns - 1.0);
    pi.mean *= adj;
    pi.var *= adj;
    pi.var = (pi.var - pi.mean * pi.mean)*adj2;
    pi.min = (0 < pi.min) ? 0 : pi.min; 
  }
}

void
collect_stats () 
{
  PredHash.clear();
  Progress::Bounded prog ("Collecting stats about nonconst preds", num_fruns);
  for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    if (!is_frun[r])
      continue;
    prog.step();
    Reader(r, NO_CHECK).read(r);
  }

  adjust_stats();

  // output stats
  ofstream ofp (Nonconst_PredStats::filename);
  ofp << PredHash;
  ofp.close();
}

void init_preds ()
{
    struct stat fst;
    if ((stat (Nonconst_PredStats::filename, &fst)) == -1) // file doesn't exist
      collect_stats();

    const unsigned num_preds = Nonconst_PredStats::count();
    Progress::Bounded predProg("reading list of non-constant predicates", num_preds);
    FILE * const pfp = fopenRead(Nonconst_PredStats::filename);
    PredCoords pc;
    pred_info_t pi;
    unsigned nread = 0;
    PredHash.clear();
    while (read_nonconst_pred(pfp, pc, pi)) {
	predProg.step();
	nread++;
	PredHash[pc] = pi;
    }
    fclose(pfp);

    verify_stats();  // verify that the predicate statistics are valid
    assert(nread == num_preds);
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
 * K-means related routines
 ***************************************************************************/
void init_centers() {
    vector<unsigned> runlist;
    runlist.resize(num_fruns);

    unsigned n = 0;
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
      if (is_frun[r]) 
        runlist[n++] = r;
    }
    assert(n == num_fruns);

    // init gsl permutation
    init_gsl_generator();
    gsl_permutation * order = gsl_permutation_alloc ((size_t) num_fruns);
    gsl_permutation_init(order);
    gsl_ran_shuffle (generator, order->data, num_fruns, sizeof(size_t));
    gsl_permute_uint (gsl_permutation_data(order), &(runlist.front()), 1, (size_t) num_fruns);
    gsl_permutation_free(order);
    free_gsl_generator();

    // generate initial centers
    // this setting fails during adjust_centers
    //unsigned runlist[] = { 5077, 8465, 11530, 2664, 11552, 30310, 16797, 23361 };
    // this setting fails at run 5160/5333 of the failed runs
    //unsigned runlist[] = { 25775, 11276, 16890, 1316, 31777, 1750, 11005, 12525 };
    centers.clear();
    for (unsigned i = 0; i < K; ++i) {
      unsigned r = runlist[i];
      Reader(r, CHECK).read(r);
      cout << "Using run " << r << " for center " << i << endl;
      centers.push_back(curr_run.preds);
    }
    
}

double L2dist (const pred_hash_t &h1, const pred_hash_t &h2) 
{
    double dist = 0.0;
    for (pred_hash_t::const_iterator c = PredHash.begin(); c != PredHash.end(); c++) {
	const PredCoords &pc = c->first;
        double v1 = 0.0;
        double v2 = 0.0;
	const pred_hash_t::const_iterator f1 = h1.find(pc);
	const pred_hash_t::const_iterator f2 = h2.find(pc);
        if (f1 != h1.end())
            v1 = f1->second.val;
        if (f2 != h2.end())
            v2 = f2->second.val;

	dist += (v1-v2)*(v1-v2);
    }
    return dist;
}

void assign_group (unsigned &label, double &dist)
{
  label = K;
  dist = (numeric_limits<double>::max)();
  for (unsigned k = 0; k < K; ++k) {
    double d = L2dist (centers[k], curr_run.preds);
    if (d < dist) {
      dist = d;
      label = k;
    }
  }
}

void adjust_centers(vector<pred_hash_t> &newcenters, 
                    const vector<unsigned> &group_sizes)
{
  for (unsigned k = 0; k < K; ++k) {
    divideby(newcenters[k], (double) group_sizes[k]);
  }
}

/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &CompactReport::argp, 0, 0, 0 },
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
        { &Nonconst_PredStats::argp, 0, 0, 0 },
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
    classify_runs();
    init_preds();
    init_centers();
    
    unsigned nchange = 0;
    double total_dist = 0.0;
    vector<unsigned> group_sizes(K);
    vector<pred_hash_t> newcenters(K);
    vector<unsigned> groups;
    groups.resize(NumRuns::end);
    Progress::Unbounded iterProg("Clustering runs using K-means");
    do {
        fill(group_sizes.begin(), group_sizes.end(), 0);
        for (unsigned k = 0; k < K; k++)
          newcenters[k].clear();
        nchange = 0;
        total_dist = 0.0;

	iterProg.step();
	cout << '\n';

	Progress::Bounded gProg("reading runs", num_fruns);
	for (unsigned r = NumRuns::begin; r < NumRuns::end; r++) {
          if (!is_frun[r])
            continue;
 
	  gProg.step();
	  Reader(r, CHECK).read(r);

          unsigned label;
          double dist;
          assign_group(label, dist);
          assert(label < K);
          total_dist += sqrt(dist);
          if (groups[r] != label)
            nchange++;
          groups[r] = label;
          group_sizes[label]++;
          update(newcenters[label], curr_run.preds);
	}

        cout << " nchanged: " << nchange << " distance: " << setprecision(16) << total_dist << endl;
        ofstream ofp ("kmeans_groups.txt", ios_base::trunc);
        ofp << "distance: " << setprecision(16) << total_dist << endl;
        copy(group_sizes.begin(), group_sizes.end(), ostream_iterator<unsigned>(ofp, " "));
        ofp << endl;
        copy(groups.begin(), groups.end(), ostream_iterator<unsigned>(ofp, "\n"));
        ofp.close();

        cout << "Adjusting centers.\n";
        adjust_centers(newcenters, group_sizes);
        cout << "done.\n";
        copy(newcenters.begin(), newcenters.end(), centers.begin());
    } while (nchange > 0);


    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
