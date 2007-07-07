#ifndef INCLUDE_symmetric_Boths_h
#define INCLUDE_symmetric_Boths_h

#include <list>
#include "Both.h"


struct Boths : public std::list<Both *>
{
  ~Boths();
};


#endif // !INCLUDE_symmetric_Boths_h
