#ifndef INCLUDE_symmetric_Coords_h
#define INCLUDE_symmetric_Coords_h

#include <iosfwd>
#include "../SiteCoords.h"


class Coords : public SiteCoords
{
public:
  Coords(const SiteCoords &, int testId);
  const int testId;
  size_t hash() const;
};


bool operator==(const Coords &, const Coords &);
bool operator!=(const Coords &, const Coords &);

std::ostream &operator<<(std::ostream &, const Coords &);


namespace __gnu_cxx
{
  template <> struct hash<Coords>
  {
    size_t operator()(const Coords &) const;
  };
}


#endif // !INCLUDE_symmetric_Coords_h
