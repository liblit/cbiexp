#include <argp.h>
#include <cassert>
#include <cmath>
#include <ext/hash_map>
#include <fstream>
#include <gsl/gsl_randist.h>
#include <iomanip>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include "classify_runs.h"
#include "fopen.h"
#include "IndexedPredInfo.h"
#include "NumRuns.h"
#include "PredCoords.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "SiteCoords.h"
#include "Score/Importance.h"
#include "sorts.h"
#include "utils.h"


using namespace std;
using __gnu_cxx::hash_map;

typedef list<IndexedPredInfo> Stats;
static Stats predList;
static unsigned nfruns = 0, nsruns = 0;

/****************************************************************
 * Predicate-specific information
 ***************************************************************/

struct PredInfo
{
    double mean, std;
    PredInfo() {
        mean = std = 0.0;
    }
    void init() { mean = std = 0.0; }
    void update(count_tp _tru, count_tp _obs) {
      double ratio = (double) _tru/_obs;
      mean += ratio;
      std += ratio * ratio;
    }
    void set_stats(unsigned nruns) {
      mean /= (double) nruns;
      std = sqrt((std - (double) nruns * mean * mean) / (double) (nruns - 1.0));
    }
};

class PredInfoPair;
typedef PredInfo (PredInfoPair::* piptr);

class PredInfoPair
{
public:
    PredInfo f;
    PredInfo s;
    double zval;
    double score;
    double std;
    void init() { f.init(); s.init(); zval = score = std = 0.0; }
    double set_zval() {
      f.set_stats(nfruns);
      s.set_stats(nsruns);
      std = (s.std > 0.0) ? s.std : 1.0;
      zval = (f.mean - s.mean) / std * sqrt((double) nfruns);
      return zval;
    }
    double set_score() {
      set_zval();
      score = log(std) - log((double) nfruns)/2.0 + zval*zval/2.0;
      return score;
    }
};

class pred_hash_t : public hash_map<PredCoords, PredInfoPair>
{
};

static pred_hash_t predHash;
static piptr pptr = 0;

/****************************************************************
 * Information about each run
 ***************************************************************/
class Reader : public ReportReader
{
public:
  Reader(const char* filename) : ReportReader(filename) {}

protected:
  void handleSite(const SiteCoords &, vector<count_tp> &);

private:
    void update(const SiteCoords &, unsigned, count_tp, count_tp) const;
};


inline void
Reader::update(const SiteCoords &coords, unsigned p, count_tp obs, count_tp tru) const
{
    PredCoords pc(coords, p);
    const pred_hash_t::iterator found = predHash.find(pc);
    if (found != predHash.end()) {
	PredInfoPair &pp = found->second;
	(pp.*pptr).update(tru, obs);
    }
}

void
Reader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
{
    const count_tp sum = accumulate(counts.begin(), counts.end(), (count_tp) 0);
    assert(sum > 0);

    const size_t size = counts.size();
    if (size == 2)
	for (unsigned predicate = 0; predicate < size; ++predicate)
	    update(coords, predicate, sum, counts[predicate]);
    else
	for (unsigned predicate = 0; predicate < size; ++predicate) {
	    update(coords, 2 * predicate,     sum,       counts[predicate]);
	    update(coords, 2 * predicate + 1, sum, sum - counts[predicate]);
	}
}

/****************************************************************************
 * Utilities
 ***************************************************************************/

void
read_preds()
{
  FILE *pfp = fopenRead(PredStats::filename);
  if (!pfp) {
    cerr << "Cannot open " << PredStats::filename << " for reading\n";
    exit(1);
  }

  pred_info pi;
  unsigned index = 0;
  while (read_pred_full(pfp, pi)) {
      PredInfoPair &pp = predHash[pi];
      pp.init();
      IndexedPredInfo indexed(pi, index++);
      predList.push_back(indexed);
  }

  fclose(pfp);
}

void compute_stats() {
  ofstream outfp("sober.txt", ios_base::trunc);
  outfp << setprecision(10);
  for (Stats::iterator c = predList.begin(); c != predList.end(); ++c) {
    const pred_hash_t::iterator found = predHash.find(*c);
    assert (found != predHash.end());
    PredInfoPair &pp = found->second;
    c->ps.imp = pp.set_score();
    c->ps.fdenom = pp.f.mean;
    c->ps.sdenom = pp.s.mean;
    outfp << pp.f.mean << " " << pp.f.std << " "
          << pp.s.mean << " " << pp.s.std << " "
          << pp.zval << " " << pp.score << endl;
  }
  outfp.close();
}


/****************************************************************************
 * Print results to file
 ***************************************************************************/

void print_results()
{
  predList.sort(Sort::Descending<Score::Importance>());
 
  ofstream outfp ("sober.xml");
  outfp << "<?xml version=\"1.0\"?>" << endl
        << "<?xml-stylesheet type=\"text/xsl\" href=\""
        << string("pred-scores") << ".xsl\"?>" << endl
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


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &NumRuns::argp, 0, 0, 0 },
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

    Reader reader("counts.txt");
    Progress::Bounded prog("Calculating SOBER scores", NumRuns::end());
    for (unsigned r = NumRuns::begin(); r < NumRuns::end(); ++r) {

	if (is_srun[r]) {
	    pptr = &PredInfoPair::s;
            nsruns++;
        }
	else if (is_frun[r]) {
	    pptr = &PredInfoPair::f;
            nfruns++;
       }
	else
	    continue;

	prog.step();

	reader.read(r);

    }

    assert(nsruns > 1);
    assert(nfruns > 1);
    compute_stats();
    print_results();
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
