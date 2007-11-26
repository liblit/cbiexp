#include "Stylesheet.h"


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "stylesheet",
    's',
    "FILE",
    0,
    "use XSLT stylesheet FILE (default \"projected-view.xsl\")",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key) {
  case 's':
    Stylesheet::filename = arg;
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


const argp Stylesheet::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
