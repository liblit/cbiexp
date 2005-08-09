#include <argp.h>
#include <cassert>
#include <cstdlib>
#include <ext/hash_map>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>
#include "NumRuns.h"
#include "PredCoords.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "TsReportReader.h"
#include "classify_runs.h"
#include "fopen.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;

/****************************************************************
 * General data and storage
 ***************************************************************/
class pred_hash_t : public hash_map<PredCoords, double>
{
};

static pred_hash_t predHash;
static vector<PredCoords> predVec;

class ts_hash_t : public hash_map<SiteCoords, timestamp>
{
};

/****************************************************************
 * Run-specific storage
 ***************************************************************/

struct run_info_t {
    ts_hash_t ts;
    int runId;
    run_info_t() {
	ts.clear();
	runId = -1;
    }
};

static run_info_t curr_run;

class Reader : public TsReportReader
{
public:
  Reader(unsigned);
  void siteTs(const SiteCoords &, timestamp);
};

inline
Reader::Reader(unsigned r)
{
  curr_run.ts.clear();
  curr_run.runId = r;
}

inline void
Reader::siteTs(const SiteCoords &coords, timestamp ts)
{
  const ts_hash_t::iterator found = curr_run.ts.find(coords);
  if (found == curr_run.ts.end()) {
    curr_run.ts[coords] = ts;
  } else {
    cerr << "Duplicate timestamps for site " << coords << "\n";
    exit(1);
  }
}

/****************************************************************
 * Incorporating information from each run
 ***************************************************************/

void compute_weights()
{
    double n = (double) curr_run.ts.size();
    double total = 0.0;
    double weight = 0.0;
    for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
	c->second = 0.0;
	ts_hash_t::iterator found = curr_run.ts.find(c->first);
	if (found != curr_run.ts.end()) {
	    weight = (double) n - found->second;
	    c->second = weight;
	    assert(weight > 0.0);
	    total += weight;
	}
    }

    // normalize the weights to be 1
    for (pred_hash_t::iterator c = predHash.begin(); c != predHash.end(); ++c) {
	c->second /= total;
    }
}

static int ts_compare(const void *t1, const void *t2)
{
  const SiteCoords *s1 = (const SiteCoords *) t1;
  const SiteCoords *s2 = (const SiteCoords *) t2;
  const ts_hash_t::iterator found1 = curr_run.ts.find(*s1);
  const ts_hash_t::iterator found2 = curr_run.ts.find(*s2);
  if (found1 == curr_run.ts.end() || found2 == curr_run.ts.end()) {
    cerr << "Cannot find sites " << *s1 << " or " << *s2 << '\n';
    exit(1);
  }
  else {
    const unsigned t1 = found1->second;
    const unsigned t2 = found2->second;
    if (t1 < t2)
      return -1;
    if (t1 == t2)
      return 0;
    if (t1 > t2)
      return 1;
  }
  return 0;
}

void process_ts()
{
  unsigned n = curr_run.ts.size();
  SiteCoords * const siteArr = new SiteCoords[n];

  // copy all the keys of the hashtable into the array
  unsigned i = 0;
  for (ts_hash_t::const_iterator c = curr_run.ts.begin(); c != curr_run.ts.end(); c++) {
    const SiteCoords &site = c->first;
    siteArr[i].unitIndex = site.unitIndex;
    siteArr[i].siteOffset = site.siteOffset;
    i++;
  }

  // sort the sites by their timestamps
  qsort(siteArr, n, sizeof(SiteCoords), ts_compare);

  for (unsigned i = 0; i < n; ++i) {
      const SiteCoords &site = siteArr[i];
      curr_run.ts[site] = (timestamp) i;
  }

  compute_weights();

  delete[] siteArr;
}

/****************************************************************************
 * Utilities
 ***************************************************************************/

void read_preds()
{
    FILE * const pfp = fopenRead(PredStats::filename);
    pred_info pi;
    queue<PredCoords> predQueue;

    while (read_pred_full(pfp, pi)) {
	predHash[pi] = 0.0;
	predQueue.push(pi);
    }
    fclose(pfp);

    // transfer the queue content to the vector
    predVec.resize(predQueue.size());
    unsigned i = 0;
    while (!predQueue.empty()) {
	predVec[i++] = predQueue.front();
	predQueue.pop();
    }
}

/****************************************************************************
 * Final processing and printing
 ***************************************************************************/

inline ostream &
operator<< (ostream &out, const vector<PredCoords> &pv)
{
    double weight = 0.0;
    for (unsigned i = 0; i < pv.size(); ++i) {
	const PredCoords pc = pv[i];
	pred_hash_t::iterator found = predHash.find(pc);
	assert(found != predHash.end());
        weight = (double) found->second;
	out << weight << ' ';
    }
    return out;
}


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &TimestampReport::argp, 0, 0, 0 },
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
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    classify_runs();

    read_preds();

    string outfile = "W." + TimestampReport::when + ".dat";
    ofstream ofp (outfile.c_str(), ios_base::trunc);

    ofp << scientific << setprecision(12);

    string pstr = "Examining " + TimestampReport::when + " timestamps";
    Progress::Bounded progress(pstr.c_str(), NumRuns::count());
    for (unsigned r = NumRuns::begin; r < NumRuns::end; r++) {
      progress.step();
      if (!is_srun[r] && !is_frun[r])
        continue;

      Reader(r).read(r);
      // sort sites by their timestamps and compute predicate weights accordingly
      process_ts();
      ofp << predVec << endl;
    }

    ofp.close();
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
