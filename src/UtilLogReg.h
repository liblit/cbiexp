#ifndef INCLUDE_UTILLOGREG_H
#define INCLUDE_UTILLOGREG_H

namespace UtilLogReg
{
  extern double tau;  // learning rate
  extern double lambda;  // regularization parameter
  extern double delta1;
  extern double delta2;
  extern double delta3;
  extern unsigned niters; // number of epochs through the training data
}

#endif // !INCLUDE_UTILLOGREG_H
