#include <sstream>
#include "EstimateTPs.h"
#include <string.h>
#include "fopen.h"

using namespace std;

bool EstimateTPs::estimate = true;

#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "estimate-tps",
    'e',
    "ESTIMATE",
    0,
    "estimate truth probabilities using ECML2006 algorithm (otherwise, make truth probabilities from actual counts)",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key) {
  case 'e':
    if (strcmp(arg, "true") == 0) {
        EstimateTPs::estimate = true;
        return 0;
    }
    if (strcmp(arg, "false") == 0) {
        EstimateTPs::estimate = false;
        return 0;
    }
    return EINVAL;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


const argp EstimateTPs::argp = {
    options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
