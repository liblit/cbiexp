#include <argp.h>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <ext/hash_map>
#include <fstream>
#include <queue>
#include "CompactReport.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
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

typedef hash_map<unsigned, unsigned> DiscreteDist;
typedef pair<DiscreteDist, DiscreteDist> DistPair;
typedef DiscreteDist (DistPair::* Dist);

static ostream &
operator<<(ostream &out, const DiscreteDist &d)
{
  out << d.size() << '\n';
  for (DiscreteDist::const_iterator c = d.begin(); c != d.end(); c++) {
    const unsigned n = c->first;
    const unsigned val = c->second;
    out << n << ' ' << val << ' ';
  }
  return out << '\n';
}

////////////////////////////////////////////////////////////////
//
// Empirical distribution of number of times reached for one retained site
//
struct SiteInfo
{
  DistPair reached;

  void notice(Dist, unsigned);
  //void print(ostream &, ostream &) const;
};

// Since the site is reached n times during this run, increment the
// corresponding counter in the empirical measure
inline void
SiteInfo::notice(Dist d, unsigned n){
  DiscreteDist &disthash = reached.*d;
  DiscreteDist::iterator found = disthash.find(n);
  if (found == disthash.end())
    disthash[n] = 1;
  else 
    disthash[n] += 1;
}

typedef hash_map<SiteCoords, SiteInfo> SiteSeen;
static SiteSeen siteSeen;

typedef hash_map<SiteCoords, unsigned> SiteCounter;
static SiteCounter siteCount;

////////////////////////////////////////////////////////////////
//
// Report Reader
//

class Reader : public ReportReader
{
public:
  Reader(Dist);

  void branchesSite(const SiteCoords &, unsigned, unsigned);
  void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned);
  void returnsSite(const SiteCoords &, unsigned, unsigned, unsigned);
  void scalarPairsSite(const SiteCoords &, unsigned, unsigned, unsigned);

  void countZeros();

private:
  void notice(const SiteCoords &, unsigned);
  void tripleSite(const SiteCoords &, unsigned, unsigned, unsigned);
  const Dist d;
};

inline
Reader::Reader(Dist _d)
  : d(_d)
{
  siteCount.clear();
}

void
Reader::notice(const SiteCoords &coords, unsigned n)
{
  SiteCounter::iterator f = siteCount.find(coords);
  if (f == siteCount.end())
    siteCount[coords] = 1;
  else
    siteCount[coords] += 1;

  const SiteSeen::iterator found = siteSeen.find(coords);
  if (found != siteSeen.end())
  {
    SiteInfo &info = found->second;
    info.notice(d, n);
  }
}

void Reader::tripleSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z)
{
  assert(x || y || z);
  notice(coords, x+y+z);
}

void Reader::scalarPairsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z)
{
  tripleSite(coords, x, y, z);
}

void Reader::returnsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z)
{
  tripleSite(coords, x, y, z);
}

void Reader::branchesSite(const SiteCoords &coords, unsigned x, unsigned y)
{
  assert (x || y);
  notice(coords, x+y);
}

void Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z, unsigned w)
{
  assert (x || y || z || w);
  notice(coords, x+y+z+w);
}

// Compact reports do not contain sites which are not reached.
// Hence we must manually add those zeros into the empirical distribution
void Reader::countZeros()
{
  for (SiteSeen::iterator s = siteSeen.begin(); s != siteSeen.end(); s++) {
    const SiteCoords &coords = s->first;
    SiteInfo &info = s->second;
    const SiteCounter::iterator found = siteCount.find(coords);
    if (found == siteCount.end())
      info.notice(d, 0);
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

  ofstream ffp("fpriors.dat");
  ofstream sfp("spriors.dat");

  typedef queue<SiteCoords> Sites;
  Sites retainedSites;

  FILE * const pfp = fopenRead(PredStats::filename);
  pred_info pi;
  while (read_pred_full(pfp, pi)) {
    const SiteSeen::iterator found = siteSeen.find(pi);
    if (found == siteSeen.end()) {
      SiteInfo &siteInfo = siteSeen[pi];
      siteInfo.reached.first.clear();
      siteInfo.reached.second.clear();
      retainedSites.push(pi);
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
    r.countZeros();

  }

  while (!retainedSites.empty()) {
    const SiteCoords &coords = retainedSites.front();
    const SiteSeen::iterator found = siteSeen.find(coords);
    if (found == siteSeen.end())
      cerr << "Error: Cannot find site " << coords.unitIndex << ' ' 
	<< coords.siteOffset << '\n';
    else {
      const SiteInfo &info = found->second;
      ffp << coords.unitIndex << ' ' << coords.siteOffset << '\n';
      ffp << info.reached.first;
      sfp << coords.unitIndex << ' ' << coords.siteOffset << '\n';
      sfp << info.reached.second;
    }
    retainedSites.pop();
  }

  ffp.close();
  sfp.close();
  return 0;
}
