#ifndef INCLUDE_NumRuns_h
#define INCLUDE_NumRuns_h

#include <argp.h>


namespace NumRuns
{
  extern const argp argp;

  extern unsigned begin;
  extern unsigned end;
  inline unsigned count() { return end - begin; }
}


#endif // !INCLUDE_NumRuns_h
