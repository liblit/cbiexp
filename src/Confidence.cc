#include <iostream>
#include "Confidence.h"

using namespace std;


unsigned Confidence::level = 95;


double
Confidence::critical(unsigned level)
{
  switch (level)
    {
    case 90:
      return 1.645;
    case 95:
      return 1.96;
    case 96:
      return 2.05;
    case 98:
      return 2.33;
    case 99:
      return 2.58;
    default:
      std::cerr << "confidence table is incomplete for level " << level << "%\n";
      exit(1);
    }
}


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
    if (errno || *tail || level <= 0 || level > 100)
      argp_error(state, "invalid confidence level \"%s\"", arg);
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


const argp Confidence::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};
