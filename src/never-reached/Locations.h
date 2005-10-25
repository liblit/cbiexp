#ifndef INCLUDE_Locations_h
#define INCLUDE_Locations_h

#include <ext/hash_set>
#include <iosfwd>

#include "Location.h"


class Locations : public __gnu_cxx::hash_set<Location>
{
public:
  void dump(std::ostream &) const;
};


#endif // !INCLUDE_Locations_h
