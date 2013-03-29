#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include "../config.h"

extern "C" {
/* Include public header files from the C API. */
#include <stdio.h>
#include "cs_api_defs.h"
#include "cs_utility.h"
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
void libeffort();
extern "C" {

void cs_plug_main(void);

#ifdef _MSC_VER
__declspec(dllexport)
#endif
CS_DEFINE_PLUGIN(libeffort);
}

void check_cs_error(cs_result result) {
  if(result != CS_SUCCESS) {
    printf("ERROR: %s\n", cs_resolve_error(result));
    exit(1);
  }
}

/* The main function of the plugin. */
extern "C" void cs_plug_main(void)
{
  //  check_cs_error(cs_startup());
  libeffort();
  //  check_cs_error(cs_cleanup());
}

cs_pdg_vertex_set CBI_pred_to_pdg_nodes(const char *fun, int line) {
  cs_uid uid;
  cs_sfid sfid;
  cs_pdg pdg;
  cs_int_pair_set pair;
  cs_offset beg, end, offset;
  cs_pdg_vertex_set v;

  check_cs_error(cs_int_pair_set_create(&pair));
  // Get uid, sfid
  check_cs_error(cs_pdg_find(fun, &pdg));
  check_cs_error(cs_pdg_compilation_uid(pdg, &uid));
  check_cs_error(cs_file_uid_get_sfid(uid, &sfid));
  check_cs_error(cs_file_get_line_range(sfid, line, &beg, &end));
  offset = end - beg;

  cs_int_pair_set_put(pair, beg, offset);
  check_cs_error(cs_int_pair_set_to_pdg_vertex_set(uid, pair, &v));
  check_cs_error(cs_int_pair_set_close(pair));

  return v;
}

bool valid(cs_pdg_kind kind) {
  return kind == cs_pdg_kind_user_defined ||
         kind == cs_pdg_kind_system_initialization ||
         kind == cs_pdg_kind_file_initialization;
}

csint32 total_size() {
  cs_result result;

  // Get the PDGS in the project
  cs_size_t size;
  result = cs_sdg_pdgs(NULL, 0, &size);
  if(result != CS_TRUNCATED) {
    printf("ERROR: %s\n", cs_resolve_error(result));
    assert(false);
  }

  cs_pdg* PDGs = (cs_pdg *) malloc(size);
  check_cs_error(cs_sdg_pdgs(PDGs, size, &size));

  csint32 numV = 0;
  for(cs_size_t i = 0; i < size / sizeof(cs_pdg); ++ i) {
    cs_pdg_kind kind = cs_pdg_get_kind(PDGs[i]);

    if(valid(kind)) {
      cs_const_pdg_vertex_set vertices;
      check_cs_error(cs_pdg_vertices(PDGs[i], &vertices));
      numV += cs_pdg_vertex_set_cardinality(vertices);
    }
  }
  free(PDGs);

  return numV;
}

csint32 effort;
void read_effort() {
  double percent, tmp;
  csint32 size;
  char *str;

  // read the % of code that the programmer will explore: from the environment
  str = getenv("EFFORT");
  if(str == NULL)
    percent = 5.0;
  else
    percent = atof(str);

  size = total_size();
  tmp = size * percent / 100;
  effort = lround(tmp);
  printf("%d %d\n", (int) size, (int) effort);
}

void put_edges_into_vertex_set(
    cs_const_pdg_edge_set edges,
    cs_pdg_vertex_set &vertices)
{
  cs_pdg pdg;
  cs_pdg_vertex v;
  cs_pdg_edge_kind e;
  cs_pdg_edge_set_iter iter;
  cs_result res;

  // if(edges == CS_PDG_EDGE_SET_INVALID || edges == 0)
  //   return;

  res = cs_pdg_edge_set_iter_first(&edges, &v, &e, &iter);
  if(res == CS_OUT_OF_ELEMENTS)
    return;
  check_cs_error(res);

  cs_pdg_vertex_pdg(v, &pdg);
  if(pdg != CS_PDG_NULL && valid(cs_pdg_get_kind(pdg)))
    cs_pdg_vertex_set_put(vertices, v);

  while(true) {
    res = cs_pdg_edge_set_iter_next(&v, &e, &iter);
    if(res == CS_OUT_OF_ELEMENTS)
      break;
    check_cs_error(res);

    cs_pdg_vertex_pdg(v, &pdg);
    if(pdg != CS_PDG_NULL && valid(cs_pdg_get_kind(pdg)))
      cs_pdg_vertex_set_put(vertices, v);
  }

  check_cs_error(cs_pdg_edge_set_iter_close(&iter));
  return;
}

cs_pdg_vertex_set bfs(cs_pdg_vertex_set &start) {
  cs_pdg_vertex_set tmp, tmp1, result, fringe;
  csint32 explored;

  check_cs_error(cs_pdg_vertex_set_create_default(&tmp));
  check_cs_error(cs_pdg_vertex_set_union(tmp, start, &result));
  check_cs_error(cs_pdg_vertex_set_union(tmp, start, &fringe));
  check_cs_error(cs_pdg_vertex_set_close(tmp));

  while(true) {
    explored = cs_pdg_vertex_set_cardinality(result);

    //printf("Starting Iteration: %d %d %d\n", explored, cs_pdg_vertex_set_cardinality(fringe), effort);

    if(explored >= effort || cs_pdg_vertex_set_cardinality(fringe) == 0)
      break;

    // accumulate adjacent vertices of all nodes in fringe
    check_cs_error(cs_pdg_vertex_set_create_default(&tmp));
    csint32 numV = cs_pdg_vertex_set_cardinality(fringe);
    cs_size_t size = numV * sizeof(cs_pdg_vertex);

    cs_pdg_vertex *list = (cs_pdg_vertex *) malloc(size);
    check_cs_error(cs_pdg_vertex_set_to_list(fringe, list, size, &size));

    for(csint32 i = 0; i < numV; i ++) {
      cs_const_pdg_edge_set inter_sources, inter_targets, intra_sources, intra_targets;

      check_cs_error(cs_pdg_vertex_inter_sources(list[i], &inter_sources));
      check_cs_error(cs_pdg_vertex_inter_targets(list[i], &inter_targets));
      check_cs_error(cs_pdg_vertex_intra_sources(list[i], &intra_sources));
      check_cs_error(cs_pdg_vertex_intra_targets(list[i], &intra_targets));

      put_edges_into_vertex_set(inter_sources, tmp);
      put_edges_into_vertex_set(inter_targets, tmp);
      put_edges_into_vertex_set(intra_sources, tmp);
      put_edges_into_vertex_set(intra_targets, tmp);
    }
    free(list);

    check_cs_error(cs_pdg_vertex_set_close(fringe));
    check_cs_error(cs_pdg_vertex_set_subtract(tmp, result, &fringe));

    check_cs_error(cs_pdg_vertex_set_union(tmp, result, &tmp1));
    check_cs_error(cs_pdg_vertex_set_close(tmp));
    check_cs_error(cs_pdg_vertex_set_close(result));

    result = tmp1;
  }
  return result;
}

#include "../StaticSiteInfo.h"
#include "../fopen.h"
#include "../utils.h"

using namespace std;

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

void libeffort()
{
  read_effort();

  StaticSiteInfo staticSiteInfo;
  FILE * const statsFile = fopenRead("preds.txt");
  pred_info info;
  unsigned index = 0;
  while (read_pred_full(statsFile, info)) {
    site_t site = staticSiteInfo.site(info.siteIndex);
    const char *func = site.fun;
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

  ofstream out("pairs.txt");
  for(src = 0; src < total; src ++) {
    set<int>::iterator iter = close_predicates[src].begin();
    for(; iter != close_predicates[src].end(); iter ++)
      out << src << " " << *iter << "\n";
//       printf("%d %d\n", src, *iter);
  }
  out.close();
}
