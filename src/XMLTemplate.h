#ifndef INCLUDE_XMLTemplate_h
#define INCLUDE_XMLTemplate_h

#include <argp.h>
#include <string>


namespace XMLTemplate
{
  extern const argp argp;

  extern std::string prefix;
  extern std::string format(char * name);
}


#endif // !INCLUDE_XMLTemplate_h
