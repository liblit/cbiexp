#include "CullPredicates.h"
#include <cstring>
#include <sstream>

using namespace std;


bool CullPredicates::cull = true;


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "cull",
    50,
    "CULL",
    0,
    "culls predicates, default is yes", 
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  using namespace CullPredicates;

  switch (key)
    {
    case 50:
      if (strncmp(arg, "no", 3) == 0) cull = false; 
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp CullPredicates::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
