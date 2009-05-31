#include <string.h>
#include "CullPredicates.h"


bool CullPredicates::cull = true; 


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "cull",
    'c',
    "CULL",
    0,
    "cull predicates, default is true",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key)
    {
    case 'c':
      if (strncmp(arg, "true", 4) == 0) {
          CullPredicates::cull = true;
          return 0;
      }
      else if (strncmp(arg, "false", 5) == 0) {
          CullPredicates::cull = false;
          return 0;
      }
      return EINVAL; 
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp CullPredicates::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
