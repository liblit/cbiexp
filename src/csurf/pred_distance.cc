using namespace std;

#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"
#include <map>
#include <set>

extern "C" {
/* Include public header files from the C API. */
#include <stdio.h>
#include "cs_api_defs.h"
#include "cs_utility.h"
#include "cs_error.h"
#include "cs_sdg.h"
#include "cs_pdg.h"
#include "cs_types.h"
#include "cs_source_files.h"
#include "cs_int_pair_set.h"
#include "cs_pdg_vertex.h"
#include "cs_pdg_vertex_set.h"
#include "cs_pdg_edge_set.h"
}

/* The macro CS_DEFINE_PLUGIN(s) registers s as a C API plugin.  It assumes
 * void cs_plug_main(void) is defined and acts as the entry point to the
 * plugin.
 */
void pred_distance();
extern "C" {

void cs_plug_main(void);

#ifdef _MSC_VER
__declspec(dllexport)
#endif
CS_DEFINE_PLUGIN(pred_distance);
}

/* The main function of the plugin. */
extern "C" void cs_plug_main(void)
{
  if( cs_startup() != 0 ) {
    printf("ERROR: %s\n", cs_resolve_error(cs_get_last_error()));
    exit(1);
  }
  
  pred_distance();

  if( cs_cleanup() != 0 ) {
    printf("ERROR: %s\n", cs_resolve_error(cs_get_last_error()));
    exit(1);
  }
}

cs_pdg_vertex_set CBI_pred_to_pdg_nodes(char *fun, int line) {
  cs_const_uid uid;
  cs_sfid sfid;
  cs_offset_pair line_range;
  cs_int_pair_set pair = cs_int_pair_set_create();
  cs_integer beg, offset;  
  cs_pdg_vertex_set v;
  
  // Get uid, sfid
  uid = cs_pdg_compilation_uid(cs_pdg_find(fun));
  sfid = cs_file_get_line_num(uid, 0).sfid;  
  line_range = cs_file_get_line_range(sfid, line);
  
  beg = line_range.offset1;
  offset = line_range.offset2 - beg;
  
  cs_int_pair_set_put(pair, beg, offset);
  
  v = cs_int_pair_set_to_pdg_vertex_set(uid, pair);
  
  cs_int_pair_set_close(pair);
  
  return v;
}

bool valid(cs_pdg_kind kind) {
  return kind == cs_pdg_kind_user_defined ||
         kind == cs_pdg_kind_system_initialization ||
         kind == cs_pdg_kind_file_initialization;
}

cs_integer total_size() {
  // Get the PDGS in the project
  cs_integer numPDGs = cs_sdg_pdgs(NULL, 0);
  cs_pdg* PDGs = (cs_pdg *) malloc (sizeof(cs_pdg) * numPDGs);  
  cs_sdg_pdgs(PDGs, numPDGs);
  
  cs_integer numV = 0;
  for(int i = 0; i < numPDGs; i ++) {
    cs_pdg_kind kind = cs_pdg_get_kind(PDGs[i]);
    
    if(valid(kind))
         numV += cs_pdg_vertex_set_cardinality(cs_pdg_vertices(PDGs[i]));
  }  
  free(PDGs);
  
  return numV;
}

cs_integer effort;
void read_effort() {
  double percent, tmp;
  cs_integer size;
  char *str;
  
  // read the % of code that the programmer will explore: from the environment
  str = getenv("EFFORT");  
  if(str == NULL)
    percent = 5.0;
  else
    percent = atof(str);
    
  size = total_size();
  tmp = size * percent / 100;
  effort = round(tmp);
  printf("%d %d\n", size, effort);
}

void put_edges_into_vertex_set(
    cs_const_pdg_edge_set edges,
    cs_pdg_edge_kind kind,
    cs_pdg_vertex_set &vertices)
{
  cs_pdg pdg;
  cs_pdg_vertex v;
  cs_edge_kind e;
  cs_pdg_edge_set_iter *iter;
  cs_result res;
  
  if(edges == CS_PDG_EDGE_SET_INVALID || edges == 0)
    return;

  res = cs_pdg_edge_set_iter_first(edges, &v, &e, kind, &iter);
  if(res != cs_success)
    return;
  
  pdg = cs_pdg_vertex_pdg(v);
  if(pdg != CS_PDG_NULL && valid(cs_pdg_get_kind(pdg)))
    cs_pdg_vertex_set_put(vertices, v);

  while(true) {
    res = cs_pdg_edge_set_iter_next(&v, &e, &iter);
    
    if(res != cs_success)
      break;
      
    pdg = cs_pdg_vertex_pdg(v);
    if(pdg != CS_PDG_NULL && valid(cs_pdg_get_kind(pdg)))
      cs_pdg_vertex_set_put(vertices, v);
  }
  
  cs_pdg_edge_set_iter_close(&iter);
  return;
}

cs_pdg_vertex_set bfs(cs_pdg_vertex_set &start) {
  cs_pdg_vertex_set tmp, tmp1, result, fringe;
  cs_integer explored;
  
  tmp = cs_pdg_vertex_set_create_default();
  result = cs_pdg_vertex_set_union(tmp, start);
  fringe = cs_pdg_vertex_set_union(tmp, start);
  cs_pdg_vertex_set_close(tmp);
  
  while(true) {
    explored = cs_pdg_vertex_set_cardinality(result);
    
    //printf("Starting Iteration: %d %d %d\n", explored, cs_pdg_vertex_set_cardinality(fringe), effort);
    
    if(explored >= effort || cs_pdg_vertex_set_cardinality(fringe) == 0)
      break;
    
    // accumulate adjacent vertices of all nodes in fringe
    tmp = cs_pdg_vertex_set_create_default();    
    int numV = cs_pdg_vertex_set_cardinality(fringe);
    cs_pdg_vertex *list = (cs_pdg_vertex *) malloc(sizeof(cs_pdg_vertex) * numV);
    cs_pdg_vertex_set_to_list(fringe, list, numV);
    
    for(int i=0; i<numV; i ++) {
      put_edges_into_vertex_set(cs_pdg_vertex_inter_sources(list[i]),
                               cs_inter_procedural,
                               tmp);
      put_edges_into_vertex_set(cs_pdg_vertex_inter_targets(list[i]),
                               cs_inter_procedural,
                               tmp);
      put_edges_into_vertex_set(cs_pdg_vertex_intra_sources(list[i]),
                               cs_intra_procedural,
                               tmp);
      put_edges_into_vertex_set(cs_pdg_vertex_intra_targets(list[i]),
                               cs_intra_procedural,
                               tmp);
    }
    free(list);

    cs_pdg_vertex_set_close(fringe);    
    fringe = cs_pdg_vertex_set_subtract(tmp, result);
    
    tmp1 = cs_pdg_vertex_set_union(tmp, result);
    cs_pdg_vertex_set_close(tmp);
    cs_pdg_vertex_set_close(result);
    
    result = tmp1;
  }
  return result;
}

#include "../PredStats.h"
#include "../StaticSiteInfo.h"
#include "../fopen.h"
#include "../utils.h"

map<int, pred_info> details;
map<int, set<int> > close_predicates;
map<int, cs_pdg_vertex_set> vertices;
map<int, cs_pdg_vertex_set> closure;

void add_close_pair(int p1, int p2) {
  int small, big;
  small = (p1 < p2) ? p1: p2;
  big = (p1 < p2) ? p2: p1;
  
  close_predicates[small].insert(big);
}

void pred_distance()
{
  read_effort();
  
  StaticSiteInfo staticSiteInfo;
  FILE * const statsFile = fopenRead("preds.txt");
  pred_info info;
  unsigned index = 0;
  while (read_pred_full(statsFile, info)) {
    site_t site = staticSiteInfo.site(info.siteIndex);
    char *func = site.fun;
    int line = site.line;
    
    details[index] = info;
    vertices[index] = CBI_pred_to_pdg_nodes(func, line);
    closure[index] = bfs(vertices[index]);
    
    /*printf("%s: %d %d %d ", func, line, info.siteIndex, info.predicate);
    printf("\tCardinality: %d\n", cs_pdg_vertex_set_cardinality(closure[index]));
    */
    index ++;
  }
  
  int src, dst, total = index;
  for(src = 0; src < total; src ++)
    for(dst = 0; dst < total; dst ++) {
      if(src == dst)
        continue;
      
      // No need to combine predicates at the same site
      if(details[src].siteIndex == details[dst].siteIndex)
        continue;
      
      if(cs_pdg_vertex_set_intersects(closure[src], vertices[dst]))
        add_close_pair(src, dst);
    }

  ofstream out("analysis/pairs.txt");  
  for(src = 0; src < total; src ++) {
    set<int>::iterator iter = close_predicates[src].begin();
    for(; iter != close_predicates[src].end(); iter ++)
      out << src << " " << *iter << "\n";
//       printf("%d %d\n", src, *iter);
  }
  out.close();
}
