#include "SourceDirectory.h"


const char *SourceDirectory::root = ".";


static const argp_option options[] = {
  {
    "source-directory",
    'd',
    "DIR",
    0,
    "hyperlink to program sources in DIR (default \".\")",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key)
    {
    case 'd':
      SourceDirectory::root = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp SourceDirectory::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};
