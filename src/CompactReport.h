#ifndef INCLUDE_CompactReport_h
#define INCLUDE_CompactReport_h

#include <argp.h>
#include <string>


namespace CompactReport
{
  extern const argp argp;

  extern unsigned sparsity;
  extern std::string format(unsigned runId, unsigned = sparsity);
}


#endif // !INCLUDE_CompactReport_h
