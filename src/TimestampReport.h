#ifndef INCLUDE_TimestampReport_h
#define INCLUDE_TimestampReport_h

#include <argp.h>
#include <string>

namespace TimestampReport
{
  extern const argp argp;

  extern std::string when;
  extern std::string format(unsigned runId, const char * w = when.c_str());
}

#endif // !INCLUDE_TimestampReport_h
