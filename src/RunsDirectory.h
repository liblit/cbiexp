#ifndef INCLUDE_RunsDirectory_h
#define INCLUDE_RunsDirectory_h

#include <argp.h>
#include <string>


namespace RunsDirectory
{
  extern const argp argp;

  extern const char *root;
  extern std::string format(unsigned runId, const char filename[]);
}


#endif // !INCLUDE_RunsDirectory_h
