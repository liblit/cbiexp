#ifndef INCLUDE_NumRuns_h
#define INCLUDE_NumRuns_h

#include "arguments.h"
#include <stdexcept> 


namespace NumRuns
{
  extern const argp argp;

  extern bool begin_is_set; 
  extern bool end_is_set;

  extern unsigned begin_val;
  extern unsigned end_val;

  inline unsigned begin()
  {
    if (begin_is_set) return begin_val; 
    else throw std::runtime_error("begin_val was not set on command line");
  }

  inline unsigned end()
  {
    if (end_is_set) return end_val; 
    else throw std::runtime_error("end_val was not set on command line");
  }

  inline unsigned count()
  {
    return end() - begin();
  }

}


#endif // !INCLUDE_NumRuns_h
