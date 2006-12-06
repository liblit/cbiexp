
/* Include public header files from the C API. */
#include <stdio.h>
#include "cs_sdg.h"
#include "cs_pdg.h"
#include "cs_types.h"


/* The macro CS_DEFINE_PLUGIN(s) registers s as a C API plugin.  It assumes
 * void cs_plug_main(void) is defined and acts as the entry point to the
 * plugin.
 */
void pred_distance();
void cs_plug_main(void);

#ifdef _MSC_VER
__declspec(dllexport)
#endif
CS_DEFINE_PLUGIN(pred_distance);


/* The main function of the plugin. */
void cs_plug_main(void)
{
  printf("---------------------\n");
  printf("C API Tutorial Output\n");
  printf("---------------------\n");

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

void pred_distance()
{
  cs_pdg *allPdgs;
  cs_integer nPdgs, size;
  cs_const_string result;
  int i;
  
  /* Clear global error state, just to be safe. */
  cs_clear_global_error();

  /* Memory allocation is done by the user.  cs_sdg_pdgs() will fill
   * a user-allocated array with all the cs_pdgs in a project.  Calling
   * cs_sdg_pdgs() with NULL instead of an array returns the expected
   * size of the array.
   */
  size = cs_sdg_pdgs(NULL, 0);
  if( size <= 0 ) {
    printf("ERROR: %s\n", cs_resolve_error(cs_get_last_error()));
    return; 
  }
  /* Allocate the array to hold the project cs_pdgs. */
  allPdgs = (cs_pdg *)malloc(sizeof(cs_pdg) * size);

  /* Fill allPdgs with the cs_pdgs in the project.  Return the number
   * of cs_pdg objects placed in allPdgs or a negative integer value
   * on error.
   */
  nPdgs = cs_sdg_pdgs(allPdgs, size);
  if( nPdgs <= 0 ) {
    printf("ERROR: %s\n", cs_resolve_error(cs_get_last_error()));
    free(allPdgs);
    return;
  }

    /* For each cs_pdg, invoke cs_pdg_procedure_name{defined,undefined}(cs_pdg).
     * If NULL is returned, print error information;
     * otherwise print the procedure name.
     */
  printf("Procedures defined in the project:\n");
  for( i = 0; i < nPdgs; i++ ) {
    result = cs_pdg_procedure_name(allPdgs[i]);
    if( result == NULL ) {
      printf("ERROR: %s\n", cs_resolve_error(cs_get_last_error()));
    }
    else
      printf("\t%s\n", result);
  }

  free(allPdgs);

}
