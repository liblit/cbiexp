#include "UtilLogReg.h"

double UtilLogReg::tau = 1e-5;
double UtilLogReg::lambda = 0.1;
double UtilLogReg::delta1 = 1.0; 
double UtilLogReg::delta2 = 1.0;
double UtilLogReg::delta3 = 0.0;
unsigned UtilLogReg::niters = 40;

static const argp_option options[] = {
  {
    "delta1",
    'x',
    "DELTA1",
    0,
    "value of delta1 in util-logreg",
    0
  },
  {
    "delta2",
    'y',
    "DELTA2",
    0,
    "value of delta2 in util-logreg",
    0
  },
  {
    "delta3",
    'z',
    "DELTA3",
    0,
    "value of delta3 in util-logreg",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};

static int
parseFlag(int key, char *arg, argp_state *)
{
  using namespace UtilLogReg;
  switch(key)
  {
  case 'x':
    delta1 = atof(arg);
    return 0;
  case 'y':
    delta2 = atof(arg);
    return 0;
  case 'z':
    delta3 = atof(arg);
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}

const argp UtilLogReg::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

