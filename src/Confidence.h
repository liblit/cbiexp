#ifndef INCLUDE_Confidence_h
#define INCLUDE_Confidence_h

#include <string>
#include "arguments.h"


namespace Confidence
{
  extern const argp argp;

  extern unsigned level;
  double critical(unsigned = level);
}


#endif // !INCLUDE_Confidence_h
