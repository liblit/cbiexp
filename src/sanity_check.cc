#include <argp.h>
#include <boost/config.hpp>
#include <boost/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <stdlib.h>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ext/hash_map>
#include <queue>
#include <vector>
#include "fopen.h"
#include "TsReportReader.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "classify_runs.h"
#include "sites.h"
#include "units.h"

using namespace boost;
using namespace std;
using __gnu_cxx::hash_map;

/****************************************************************
 * General data and storage
 ***************************************************************/
typedef float weight_t;
typedef property<vertex_name_t, SiteCoords> vName;
typedef property<vertex_index_t, unsigned, vName> Vprops;
typedef property<edge_weight2_t, weight_t> edgeW2;
typedef property<edge_weight_t, weight_t, edgeW2> Eprops;
typedef adjacency_list<setS, vecS, directedS, Vprops, Eprops> Graph;
typedef graph_traits<Graph>::vertex_descriptor VertexD;
typedef graph_traits<Graph>::edge_descriptor EdgeD;

class ts_hash_t : public hash_map<SiteCoords, unsigned>
{
};

class site_hash_t : public hash_map<SiteCoords, timestamp>
{
};

static Graph G;
static site_hash_t siteIndex;
static std::queue<pair<unsigned, SiteCoords> > parentQueue;
static unsigned sitei = 0;
static unsigned nruns_used = 0;
const SiteCoords firstSite(2, 169);    // returns site from newregion() on line 5918
const SiteCoords secondSite(0, 32093); // scalar-pairs site: config.mem_database = newregion(); 
const SiteCoords siteA(0, 32027); // s-p: 5900 files_init i pindex (component 934)
const SiteCoords siteB(0, 26821);  // s-p: 5674 init_options c->errorfile (FILE *)0

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

  if (coords == firstSite)
    assert (ts == 1);
  if (coords == secondSite)
    assert (ts == 2);
  if (coords == firstSite || coords == secondSite)
    assert (ts != 0);

  // see if this is a new site,
  // if so, assign it a site index and insert it into the graph
  site_hash_t::const_iterator found2 = siteIndex.find(coords);
  if (found2 == siteIndex.end()) {
      siteIndex[coords] = sitei;
      add_vertex(Vprops(sitei++, coords), G);
  }
}

/****************************************************************
 * Incorporating information from each run
 ***************************************************************/

void inc_ordering(const SiteCoords *const siteArr, unsigned n)
{
    EdgeD ed;
    bool added;
    for (unsigned i = 1; i < n; i++) {
	const SiteCoords &s1 = siteArr[i-1];
	const SiteCoords &s2 = siteArr[i];
	const site_hash_t::iterator found1 = siteIndex.find(s1);
	const site_hash_t::iterator found2 = siteIndex.find(s2);
	if(s2 == firstSite)
          assert(0);
	if (siteB == s2 && !(siteA == s1)) {
          pair<unsigned, SiteCoords> pp;
          pp.first = curr_run.runId;
          pp.second = s1;
          parentQueue.push(pp);
        }
	assert(found1 != siteIndex.end());
	assert(found2 != siteIndex.end());
	VertexD u = vertex(found1->second, G);
	VertexD v = vertex(found2->second, G);

	// add the edge to the graph (will check for duplicates since
	// edgelist type is setS)
	tie(ed, added) = add_edge(u, v, G);
	property_map<Graph, edge_weight_t>::type w = get(edge_weight, G);
	if (!added) // edge was not added (edge already exists)
	    w[ed] += 1.0;  // increment weight by 1
	else
	    w[ed] = 1.0;   // set initial weight to 1
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
    const timestamp t1 = found1->second;
    const timestamp t2 = found2->second;
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

  inc_ordering(siteArr, n);   // incorporate the new ordering into graph

  delete[] siteArr;
}

inline ostream &
operator<< (ostream &out, const ts_hash_t &ts)
{
  for (ts_hash_t::const_iterator c = ts.begin(); c != ts.end(); ++c) 
  {
    out << c->first << '\t' << c->second << endl;
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
    // clear site hash
    siteIndex.clear();

    string pstr = "Examining " + TimestampReport::when + " timestamps";
    Progress::Bounded progress(pstr.c_str(), NumRuns::count());
    for (unsigned r = NumRuns::begin; r < NumRuns::end; r++) {
      progress.step();
      if (!is_srun[r] && !is_frun[r])
        continue;

      nruns_used++;
      Reader(r).read(r);
      process_ts();       // sort sites by their timestamps
    }

    // There are 18366 sites with "first" timestamps
    // There are 19587 edges in the resulting site graph
    // cout << "There are " << curr_run.ts.size() << "sites with timestamps.\n";

    cout << "Parents of site B (" << siteB << ")\n";
    while (!parentQueue.empty()) {
       unsigned r = parentQueue.front().first;
       SiteCoords &coords = parentQueue.front().second;
       cout << "Run " << r << " Site " << coords << endl;
       parentQueue.pop(); 
    }
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
