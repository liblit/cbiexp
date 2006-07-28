#include <argp.h>
#include <boost/config.hpp>
#include <boost/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/strong_components.hpp>
#include <stdlib.h>
#include <cassert>
#include <fstream>
#include <sstream>
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
#include "StaticSiteInfo.h"
#include "classify_runs.h"

using namespace boost;
using namespace std;
using __gnu_cxx::hash_map;

/****************************************************************
 * General data and storage
 ***************************************************************/
// Input graph properties
typedef float weight_t;
typedef property<vertex_name_t, SiteCoords> vName;
typedef property<vertex_index_t, unsigned, vName> Vprops;
typedef property<edge_weight2_t, weight_t> edgeW2;
typedef property<edge_weight_t, weight_t, edgeW2> Eprops;
typedef adjacency_list<setS, vecS, directedS, Vprops, Eprops> Graph;
typedef graph_traits<Graph>::vertex_descriptor VertexD;
typedef graph_traits<Graph>::edge_descriptor EdgeD;

// Output graph properties (graph of connected components)
typedef property<vertex_index_t, unsigned> Vprops2;
typedef property<edge_weight_t, weight_t> Eprops2;
typedef adjacency_list<setS, vecS, bidirectionalS, Vprops2, Eprops2> SCCGraph;
typedef graph_traits<SCCGraph>::vertex_descriptor SCCVert;
typedef graph_traits<SCCGraph>::edge_descriptor SCCEdge;

typedef adjacency_list<vecS, vecS, bidirectionalS, Vprops2, Eprops2> SCCGraph2;
typedef graph_traits<SCCGraph2>::vertex_descriptor SCCVert2;
typedef graph_traits<SCCGraph2>::edge_descriptor SCCEdge2;

class ts_hash_t : public hash_map<SiteCoords, unsigned>
{
};

static Graph G;
static SCCGraph SCCG;
static SCCGraph2 SCCGbrev;
static ts_hash_t siteIndex;
static ts_hash_t vertIndex;
static vector< std::queue<unsigned> > CompLists;
static vector<string> CompNames;
static vector<string> CompNamesbrev;
const SiteCoords firstSite(2, 169);

/****************************************************************
 * Utilities
 ***************************************************************/
void
build_siteIndex()
{
  StaticSiteInfo staticSiteInfo;
  unsigned s = 0;
  
  siteIndex.clear();
  SiteCoords coords;
  for (coords.unitIndex = 0; coords.unitIndex < staticSiteInfo.unitCount; ++(coords.unitIndex))
    for (coords.siteOffset = 0; 
         coords.siteOffset < staticSiteInfo.unit(coords.unitIndex).num_sites; 
         ++(coords.siteOffset), ++s) {
      siteIndex[coords] = s;
    }
}

inline unsigned
find_siteindex(VertexD &v)
{
  property_map<Graph, vertex_name_t>::type vname = get(vertex_name, G);
  const SiteCoords &coords = vname[v];     // get the site coordinates of the vertex
  ts_hash_t::iterator found = siteIndex.find(coords);  // find the site index
  assert(found != siteIndex.end());
  return found->second;
}

string
sitename_format(unsigned i)
{
  static StaticSiteInfo staticSiteInfo;
  ostringstream collect;
  const site_t &site = staticSiteInfo.site(i);
  collect << site.scheme_code << '\t' << site.file << '\t' 
	  << site.line << '\t' << site.fun << '\t';
  switch(site.scheme_code) {
  case 'B':
  case 'F':
  case 'G':
  case 'R':
    collect << site.args[0];
  case 'S':
    collect << '\t' << site.args[1];
    break;
  default:
    assert(0);
  }
  return collect.str();
}

inline string compname_format(unsigned c, unsigned si) {
  ostringstream collect;
  collect << "Comp " << c << ": " << sitename_format(si);
  return collect.str();
}

/****************************************************************
 * Reading input
 ***************************************************************/

void read_node (FILE *fp)
{
  SiteCoords coords;
  unsigned ind;
  int ctr = fscanf(fp, "%u\t%u\t%u\n", &coords.unitIndex, &coords.siteOffset, &ind);
  assert(ctr == 3);

  assert (vertIndex.find(coords) == vertIndex.end());
  vertIndex[coords] = ind;

  add_vertex(Vprops(ind, coords), G);
}

void read_edge (FILE *fp)
{
  SiteCoords c1, c2;
  unsigned u, v;
  weight_t weight;
  EdgeD ed;
  bool added;

  int ctr = fscanf(fp, "%u\t%u\t%u\t%u\t%u\t%u\t%g\n", &u, &c1.unitIndex, &c1.siteOffset, &v, &c2.unitIndex, &c2.siteOffset, &weight);
  assert(ctr == 7);

  assert(vertIndex[c1] == u);
  assert(vertIndex[c2] == v);

  if (firstSite == c2) {
    assert(0);
  }

  tie(ed, added) = add_edge(u, v, G);
  assert(added);
  property_map<Graph, edge_weight_t>::type w = get(edge_weight, G);
  w[ed] = weight;
}

void read_graph ()
{
  string nfile = "ts_nodes." + TimestampReport::when + ".txt";
  FILE *nfd = fopenRead(nfile.c_str());
  while (true)
  {
    read_node (nfd);
    if (feof(nfd))
      break;
  }
  fclose(nfd);

  string efile = "ts_edges." + TimestampReport::when + ".txt";
  FILE *efd = fopenRead(efile.c_str());
  while (true) 
  {
    read_edge(efd);
    if (feof(efd))
      break;
  }
  fclose(efd);

  property_map<Graph, vertex_name_t>::type vname = get(vertex_name, G);
  for (ts_hash_t::iterator c = vertIndex.begin(); c != vertIndex.end(); ++c) {
    put(vname, c->second, c->first);
  }
}

/****************************************************************************
 * Computation and Processing
 ***************************************************************************/

inline bool
interesting(const SCCVert &v)
{
  bool branch = in_degree(v, SCCG) != 1;
  bool collect = out_degree(v, SCCG) != 1;
  bool sibling = in_degree(v,SCCG) == 1 && out_degree(*(inv_adjacent_vertices(v, SCCG).first), SCCG) > 1;
  return branch || collect || sibling;
}

// Find the strongly connected components, build up the SCC Grapg
// and output using graphviz
void find_sccs ()
{
  // first find the strongly connected components
  vector<unsigned> component(num_vertices(G));
  unsigned ncomp = strong_components(G, &component[0]);

  cout << "There are " << ncomp << " strongly connected components.\n";

  // Build up the component list
  CompLists.resize(ncomp);
  for (unsigned i = 0; i != component.size(); ++i) {
    unsigned c = component[i];
    CompLists[c].push(i); 
  }

  // build up the SCC Graph 
  // add vertices 
  for (unsigned i = 0; i < ncomp; ++i)
    add_vertex(i, SCCG); 

  // for each edge in the original graph, 
  // add an edge in the SCC graph if the two ends belong in different SCC's
  property_map<Graph, edge_weight_t>::type w = get(edge_weight, G);
  property_map<SCCGraph, edge_weight_t>::type sccw = get(edge_weight, SCCG);
  property_map<Graph, vertex_name_t>::type vname = get(vertex_name, G);
  graph_traits<Graph>::edge_iterator e, e_end;
  SCCEdge ed;
  bool added;

  for (boost::tie(e, e_end) = edges(G); e != e_end; ++e) {
    VertexD u = source(*e, G);
    VertexD v = target(*e, G);
    if (vname[v] == firstSite) {
      cerr << u << '\t' << vname[u] << '\t' << v << '\t' << vname[v] << endl;
      assert(0);
    }
    unsigned i = component[u];
    unsigned j = component[v];
    if (i != j) {
      tie(ed, added) = add_edge(i, j, SCCG);
      if (!added)
        sccw[ed] += w[*e];
      else
        sccw[ed] = w[*e];
    }
  }

  // build the abbreviated SCC graph, which only contains "interesting" edges,
  // i.e., edges that are not simply between adjacent scc's.  The chain part
  // of the original SCC graph will be abbreviated by an edge with weight equal
  // to the number of components along that chain.
  int nnodes = 0;
  hash_map<int, int> sccvertHash;
  graph_traits<SCCGraph>::vertex_iterator u_start, u_end;
  for (boost::tie(u_start, u_end) = vertices(SCCG); u_start != u_end; ++u_start) {
    if (interesting(*u_start))
      if (sccvertHash.find(*u_start) == sccvertHash.end()) {
        add_vertex(nnodes, SCCGbrev);
        sccvertHash[*u_start] = nnodes;
        nnodes++;
      }
  }

  for (hash_map<int,int>::iterator c = sccvertHash.begin();
       c != sccvertHash.end(); ++c) {
    SCCEdge2 ed;
    bool added;
    SCCVert u = c->first;
    SCCVert2 ubrev = c->second;
    graph_traits<SCCGraph>::adjacency_iterator v_start, v_end, v2_start, v2_end;
    for (boost::tie(v_start, v_end) = adjacent_vertices(u, SCCG); 
         v_start != v_end; ++v_start) {
      SCCVert v = *v_start;
      int len = 1;
      while (!interesting(v)) {
        boost::tie(v2_start, v2_end) = adjacent_vertices(v, SCCG);
        v = *v2_start;
        len++;
      }
      if (sccvertHash.find(v) == sccvertHash.end()) {
        cerr << "Looking for children of vertex " << u << endl;
        cerr << "It has out-degree " << out_degree(u, SCCG) << " and in-degree " 
             << in_degree(u, SCCG) << endl;
        cerr << "Vertex " << v << " is not in the sccvertHash.\n";
        cerr << "It has out-degree " << out_degree(v, SCCG) << " and in-degree " 
             << in_degree(v, SCCG) << endl;
        exit(1);
      }
      SCCVert2 vbrev = sccvertHash[v];
      tie(ed, added) = add_edge(ubrev, vbrev, SCCGbrev);
      assert(added);
      property_map<SCCGraph2, edge_weight_t>::type sccw2 = get(edge_weight, SCCGbrev);
      sccw2[ed] = len;
    }
  }

  CompNames.resize(ncomp);
  for (unsigned c = 0; c < ncomp; ++c) {
    VertexD v = CompLists[c].front();  // grab the first vertex under that compoent
    unsigned si = find_siteindex(v);  // get the site index of the first node of that component
    CompNames[c] = compname_format(c, si); // store the string-formatted site name
  }

  CompNamesbrev.resize(sccvertHash.size());
  for (hash_map<int, int>::iterator c = sccvertHash.begin();
       c != sccvertHash.end(); ++c) {
    int comp = c->first;
    int vertnum = c->second;
    VertexD v = CompLists[comp].front();  // grab the first vertex under that compoent
    unsigned si = find_siteindex(v);  // get the site index of the first node of that component
    CompNamesbrev[vertnum] = compname_format(comp, si); // store the string-formatted site name
  }


  // Output the SCC Graph using graphviz
  string dotfile = "scc_graph." + TimestampReport::when + ".dot";
  ofstream sccof(dotfile.c_str(), ios_base::trunc);
  write_graphviz(sccof, SCCG, make_label_writer(CompNames));
  sccof.close();

  dotfile = "scc_graph.brev." + TimestampReport::when + ".dot";
  sccof.open(dotfile.c_str(), ios_base::trunc);
  write_graphviz(sccof, SCCGbrev,
                 make_label_writer(CompNamesbrev), 
                 make_label_writer(get(edge_weight, SCCGbrev)));
  sccof.close();

  // Output the list of components
  string compfile = "scc_complist." + TimestampReport::when + ".txt";
  ofstream compof(compfile.c_str(), ios_base::trunc);
  for (unsigned c = 0; c < ncomp; ++c) {
    compof << "Component " << c << " has " << CompLists[c].size() << " elements" << endl;
    while (!CompLists[c].empty()) {
      VertexD v = CompLists[c].front();
      unsigned si = find_siteindex(v);
      compof << sitename_format(si) << endl;
      CompLists[c].pop();
    }
  }
  compof.close();
}

/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &TimestampReport::argp, 0, 0, 0 },
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

    build_siteIndex();

    read_graph();

    find_sccs();

    return 0;
}
