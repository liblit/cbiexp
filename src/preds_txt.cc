#include "preds_txt.h"


const char *preds_txt_filename = "preds.txt";


////////////////////////////////////////////////////////////////////////
//
// Command line processing
//


static const argp_option options[] = {
  {
    "predicates",
    'p',
    "preds.txt",
    0,
    "predicate statistics file",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parse(int key, char *arg, argp_state *)
{
  static const char *filename = "preds.txt";

  switch (key) {
  case 'p':
    filename = arg;
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


const argp preds_txt_argp = {
  options, parse, 0, 0, 0, 0, 0
};
