#include <argp.h>
#include <boost/config.hpp>
#include <boost/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ext/hash_map>
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

class ts_hash_t : public hash_map<SiteCoords, timestamp>
{
};

typedef pair<SiteCoords,timestamp> ts_pair_t;


static Graph G;
static ts_hash_t siteIndex;
static unsigned sitei = 0;
static weight_t maxweight = (std::numeric_limits<weight_t>::max)();

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
    ts_pair_t tsval(coords, ts);
    curr_run.ts.insert(tsval);
  } else {
    cerr << "Duplicate timestamps for site " << coords << "\n";
    exit(1);
  }

  // see if this is a new site,
  // if so, assign it a site index and insert it into the graph
  ts_hash_t::const_iterator found2 = siteIndex.find(coords);
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
	const ts_hash_t::iterator found1 = siteIndex.find(s1);
	const ts_hash_t::iterator found2 = siteIndex.find(s2);
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
    cerr << "Cannot find sites " << *s1 << " or " << *s1 << '\n';
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

  inc_ordering(siteArr, n);   // incorporate the new ordering into graph

  delete[] siteArr;
}

/****************************************************************************
 * Final processing and printing
 ***************************************************************************/

inline ostream &
operator<< (ostream &out, const ts_hash_t &th)
{
    for (ts_hash_t::const_iterator c = th.begin(); c != th.end(); c++) {
	const SiteCoords tc = c->first;
	const unsigned ts = c->second;
	out << tc << '\t' << ts << '\n';
    }
    return out;
}

void transform_weights()
{
    unsigned nruns = num_sruns + num_fruns;
    property_map<Graph, edge_weight_t>::type w = get(edge_weight, G);
    graph_traits<Graph>::edge_iterator e, e_end;
    for (boost::tie(e, e_end) = edges(G); e != e_end; ++e) {
	assert (w[*e] > 0.0);
	w[*e] = 1 - (weight_t) w[*e]/nruns;
	assert (w[*e] >= 0.0);
    }
}

inline bool
reach(weight_t ** D, unsigned i, unsigned j)
{
    return (abs(D[i][j])< maxweight);
}

void print_apsp(weight_t ** D, unsigned n)
{
    string tsFile = "ts_sites." + TimestampReport::when + ".txt";
    ofstream indexOs(tsFile.c_str(), ios_base::trunc);
    indexOs << siteIndex;
    indexOs.close();

    string siteFile = "site_orders." + TimestampReport::when + ".txt";
    ofstream os(siteFile.c_str(), ios_base::trunc);

    property_map<Graph, vertex_name_t>::type vname = get(vertex_name,G);
    for (unsigned i = 0; i < n; i++) {
	for (unsigned j = i+1; j < n; j++) {
	    if (reach(D, i, j) && !reach(D, j, i))
		os << vname[i] << '\t' << vname[j] << '\t' << D[i][j] << '\n';
	    else if (reach(D, j, i) && ! reach(D,i,j))
		os << vname[j] << '\t' << vname[i] << '\t' << D[j][i] << '\n';
	}
    }

    os.close();
}

void calc_apsp()
{
    cout << "The graph contains " << num_vertices(G) << " vertices and "
	 << num_edges(G) << " edges.\n";

    cout << "Transforming weights\n";
    transform_weights();

    cout << "Computing all-pairs shortest paths.\n";
    const unsigned V = num_vertices(G);
    vector<weight_t> d(V, maxweight);
    weight_t **D = new weight_t* [V];
    for (unsigned i = 0; i < V; i++)
      D[i] = new weight_t[V];

    bool result = johnson_all_pairs_shortest_paths(G, D, distance_map(&d[0]));
    if (!result) {
      cerr << "There exists a negative weight cycle in the graph.\n";
      exit(1);
    }
    else {
      print_apsp(D, V);
    }

    for (unsigned i = 0; i < V; i++)
      delete[] D[i];
    delete[] D;
}


// unsigned
// num_collisions(const ts_hash_t &table)
// {
//   unsigned b = table.bucket_count();
//   unsigned c = 0;
//   for (unsigned i = 0; i < b; i++) {
//       unsigned elems = table.elems_in_bucket(i);
//       if (i > 1)
// 	  c += elems - 1;
//   }
//   return c;
// }


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

      Reader(r).read(r);
      process_ts();       // sort sites by their timestamps
    }

    calc_apsp();

    // There are 18366 sites with "first" timestamps
    // There are 19587 edges in the resulting site graph
    // cout << "There are " << curr_run.ts.size() << "sites with timestamps.\n";

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
