#ifndef INCLUDE_Locations_h
#define INCLUDE_Locations_h

#include <ext/hash_set>
#include "Location.h"


class Locations : public __gnu_cxx::hash_set<Location>
{
public:
  void dump(const char []) const;
};


#endif // !INCLUDE_Locations_h
