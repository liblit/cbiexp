#include <argp.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <ext/hash_map>
#include <fstream>
#include <numeric>
#include <queue>
#include "CompactReport.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "PredCoords.h"
#include "classify_runs.h"
#include "fopen.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;

static unsigned cur_run;

////////////////////////////////////////////////////////////////
//
// Discrete Distribution
// (Actually, it's a measure, i.e., unnormalized counts
//

typedef hash_map<int, unsigned> DiscreteDist;
typedef pair<DiscreteDist, DiscreteDist> DistPair;
typedef DiscreteDist (DistPair::* Dist);

static ostream &
operator<<(ostream &out, const DiscreteDist &d)
{
  out << d.size() << '\n';
  for (DiscreteDist::const_iterator c = d.begin(); c != d.end(); c++) {
    out << c->first << ' ' << c->second << ' ';
  }
  return out << '\n';
}

////////////////////////////////////////////////////////////////
//
// Empirical distribution of number of times reached for one retained site
//
struct PredInfo
{
  DistPair reached;

  void notice(Dist, unsigned, unsigned);
  //void print(ostream &, ostream &) const;
};

// Since the site is reached n times during this run, increment the
// corresponding counter in the empirical measure
inline void
PredInfo::notice(Dist d, unsigned n, unsigned x){
  DiscreteDist &disthash = reached.*d;
  int ratio = (int) round((double) x*100/n); // round to nearest .01
  DiscreteDist::iterator found = disthash.find(ratio);
  if (found == disthash.end())
    disthash[ratio] = 1;
  else 
    disthash[ratio] += 1;
}

typedef hash_map<PredCoords, PredInfo> PredSeen;
static PredSeen predSeen;

typedef hash_map<PredCoords, unsigned> PredCounter;
static PredCounter predCount;

////////////////////////////////////////////////////////////////
//
// Report Reader
//

class Reader : public ReportReader
{
public:
  Reader(Dist);

protected:
  void handleSite(const SiteCoords &, vector<unsigned> &);

private:
  void notice(const SiteCoords &, unsigned, unsigned, unsigned);
  const Dist d;
};

inline
Reader::Reader(Dist _d)
  : d(_d)
{
  predCount.clear();
}

void
Reader::notice(const SiteCoords &coords, unsigned p, unsigned n, unsigned x)
{
  assert(x <= n);
  PredCoords pc(coords, p);
  PredCounter::iterator f = predCount.find(pc);
  if (f == predCount.end())
    predCount[pc] = 1;
  else
    predCount[pc] += 1;

  const PredSeen::iterator found = predSeen.find(pc);
  if (found != predSeen.end())
  {
    PredInfo &info = found->second;
    info.notice(d, n, x);
  }
}

void Reader::handleSite(const SiteCoords &coords, vector<unsigned> &counts)
{
  const unsigned sum = accumulate(counts.begin(), counts.end(), 0);
  assert(sum > 0);

  const size_t size = counts.size();
  if (size == 2)
    for (unsigned predicate = 0; predicate < size; ++predicate)
      notice(coords, predicate, sum, counts[predicate]);
  else
    for (unsigned predicate = 0; predicate < size; ++predicate) {
      notice(coords, 2 * predicate,     sum,       counts[predicate]);
      notice(coords, 2 * predicate + 1, sum, sum - counts[predicate]);
    }
}

////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static void process_cmdline(int argc, char **argv)
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
};

////////////////////////////////////////////////////////////////
//
// The main event
//

int main (int argc, char** argv)
{
  process_cmdline (argc, argv);

  classify_runs();

  ofstream ffp("ftruthfreq.dat");
  ofstream sfp("struthfreq.dat");

  typedef queue<PredCoords> Preds;
  Preds retainedPreds;

  FILE * const pfp = fopenRead(PredStats::filename);
  pred_info pi;
  while (read_pred_full(pfp, pi)) {
    const PredSeen::iterator found = predSeen.find(pi);
    if (found == predSeen.end()) {
      PredInfo &predInfo = predSeen[pi];
      predInfo.reached.first.clear();
      predInfo.reached.second.clear();
      retainedPreds.push(pi);
    }
  }

  fclose(pfp);

  Progress::Bounded progress("Gathering empirical distribution", NumRuns::count());
  for (cur_run = NumRuns::begin; cur_run < NumRuns::end; cur_run++) {
    progress.step();
    Dist d;
    if (is_frun[cur_run])
      d = &DistPair::first;
    else if (is_srun[cur_run])
      d = &DistPair::second;
    else
      continue;

    Reader r(d);
    r.read(cur_run);

  }

  while (!retainedPreds.empty()) {
    const PredCoords &coords = retainedPreds.front();
    const PredSeen::iterator found = predSeen.find(coords);
    if (found == predSeen.end())
      cerr << "Error: Cannot find pred " << coords.unitIndex << ' ' 
	   << coords.siteOffset << ' ' << coords.predicate << '\n';
    else {
      const PredInfo &info = found->second;
      ffp << coords.unitIndex << ' ' << coords.siteOffset << ' '
	  << coords.predicate << '\n';
      ffp << info.reached.first;
      sfp << coords.unitIndex << ' ' << coords.siteOffset << ' ' 
	  << coords.predicate << '\n';
      sfp << info.reached.second;
    }
    retainedPreds.pop();
  }

  ffp.close();
  sfp.close();
  return 0;
}
