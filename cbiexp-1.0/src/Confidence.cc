#include <cstdlib>
#include <iostream>
#include "Confidence.h"

using namespace std;


unsigned Confidence::level = 95;


double
Confidence::critical(unsigned level)
{
  // calculated using <http://davidmlane.com/hyperstat/z_table.html>
  switch (level)
    {
    case 0:
      return 0;
    case 80:
      return 1.2816;
    case 81:
      return 1.3106;
    case 82:
      return 1.3408;
    case 83:
      return 1.3722;
    case 84:
      return 1.4051;
    case 85:
      return 1.4395;
    case 86:
      return 1.4758;
    case 87:
      return 1.5141;
    case 88:
      return 1.5548;
    case 89:
      return 1.5982;
    case 90:
      return 1.6449;
    case 91:
      return 1.6954;
    case 92:
      return 1.7507;
    case 93:
      return 1.8119;
    case 94:
      return 1.8808;
    case 95:
      return 1.9600;
    case 96:
      return 2.0537;
    case 97:
      return 2.1701;
    case 98:
      return 2.3263;
    case 99:
      return 2.5758;
    default:
      std::cerr << "confidence table is incomplete for level " << level << "%\n";
      exit(1);
    }
}


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "confidence",
    'c',
    "PERCENT",
    0,
    "use confidence level PERCENT% (default 95) for increase scores",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *state)
{
  using namespace Confidence;

  switch (key) {
  case 'c':
    char *tail;
    errno = 0;
    level = strtol(arg, &tail, 0);
    if (errno || *tail || level > 100)
      argp_error(state, "invalid confidence level \"%s\"", arg);
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


const argp Confidence::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
