#include <sstream>
#include "AdjWeightsOptions.h"

using namespace std;


string AdjWeightsOptions::adj_weights;
string AdjWeightsOptions::not_adj_weights;


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "truth-probability-weights",
    27,
    "FILE",
    0,
    "look for truth probability report named \"FILE\" (default is \"X.dat\")",
    0
  },
  {
    "not-truth-probability-weights",
    28,
    "FILE",
    0, 
    "look for truth probability report for complements named \"FILE\" (default is \"X.dat\")",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  using namespace AdjWeightsOptions;

  switch (key)
    {
    case 27:
      AdjWeightsOptions::adj_weights = arg;
      return 0;
    case 28:
      AdjWeightsOptions::not_adj_weights = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp AdjWeightsOptions::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
