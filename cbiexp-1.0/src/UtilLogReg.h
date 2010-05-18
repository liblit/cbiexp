#ifndef INCLUDE_UTILLOGREG_H
#define INCLUDE_UTILLOGREG_H

#include <argp.h>
#include <stdlib.h>

namespace UtilLogReg
{
  extern double tau;  // learning rate
  extern double lambda;  // regularization parameter
  extern double delta1;
  extern double delta2;
  extern double delta3;
  extern unsigned niters; // number of epochs through the training data

  extern const argp argp;
}

#endif // !INCLUDE_UTILLOGREG_H
