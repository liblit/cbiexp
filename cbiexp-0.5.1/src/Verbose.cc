#include "Verbose.h"


bool Verbose::on = false;


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  { "verbose", 'v', 0, 0, "trace analysis in greater detail", 0 },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *, argp_state *)
{
  switch (key)
    {
    case 'v':
      Verbose::on = true;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp Verbose::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
