#ifndef INCLUDE_AmplifyReport_h
#define INCLUDE_AmplifyReport_h

#include <string>
#include "arguments.h"


namespace AmplifyReport
{
  extern const argp argp;

  extern bool amplify;
  extern std::string format(unsigned runId);
}


#endif // !INCLUDE_AmplifyReport_h
