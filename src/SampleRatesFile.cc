#include <sstream>
#include "Rates.h"

using namespace std;

const char *Rates::filename = "rates.txt";


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "sample-rates",
    'd',
    "RATES",
    0,
    "look for rates in RATES (default \"rates.txt\")",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key) {
  case 'd':
    Rates::filename = arg;
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


const argp Rates::argp = {
    options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
