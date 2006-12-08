#include <assert.h>
#include <stdlib.h>

using namespace std;

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
#include "cs_pdg_vertex_set.h"
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
  /* cs_startup() must be called before any C API functions are
   * invoked.
   */
  if( cs_startup() != 0 ) {
    printf("ERROR: %s\n", cs_resolve_error(cs_get_last_error()));
    exit(1);
  }
  
  pred_distance();

  /* cs_cleanup() must be called after all C API functions have been 
   * called.
   */
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

void pred_distance()
{
  cs_pdg_vertex_set vertices = CBI_pred_to_pdg_nodes("main", 150);
  printf("Number of nodes: %d\n", cs_pdg_vertex_set_cardinality(vertices));
  cs_pdg_vertex_set_close(vertices);

}
