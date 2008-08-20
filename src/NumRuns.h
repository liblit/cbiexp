#ifndef INCLUDE_NumRuns_h
#define INCLUDE_NumRuns_h

#include "arguments.h"


namespace NumRuns
{
  extern const argp argp;

  extern unsigned begin;
  extern unsigned end;
  inline unsigned count() { return end - begin; }

  extern bool begin_is_set; 
  extern bool end_is_set;
}


#endif // !INCLUDE_NumRuns_h
