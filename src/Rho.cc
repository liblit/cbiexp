#include <cassert>
#include "Rho.h"

using namespace std;


const char *Rho::filename = "rho.bin";


Rho::Rho(const size_t size, const char filename[])
  : mapping(filename),
    matrix(size, mapping.begin())
{
  assert(size * size == mapping.size());
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static const argp_option options[] = {
  {
    "rho",
    'r',
    "rho.bin",
    0,
    "correlation matrix file",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key)
    {
    case 'r':
      Rho::filename = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp Rho::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};
