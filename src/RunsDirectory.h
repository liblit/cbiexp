#ifndef INCLUDE_RunsDirectory_h
#define INCLUDE_RunsDirectory_h

#include <iosfwd>
#include <string>
#include "arguments.h"


namespace RunsDirectory
{
  extern const argp argp;

  extern const char *root;

  extern std::string format(unsigned runId, const char filename[]);
  extern void format(std::ostream &, unsigned runId, const char filename[]);
}


#endif // !INCLUDE_RunsDirectory_h
