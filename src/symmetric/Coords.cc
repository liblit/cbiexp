#include "Coords.h"

using namespace std;


Coords::Coords(const SiteCoords &site, int testId)
  : SiteCoords(site),
    testId(testId)
{
}


bool
operator==(const Coords &a,
	   const Coords &b)
{
  return
    (static_cast<const SiteCoords &>(a) ==
     static_cast<const SiteCoords &>(b))
    && a.testId == b.testId;
}


bool
operator!=(const Coords &a,
	   const Coords &b)
{
  return !(a == b);
}


size_t
Coords::hash() const
{
  return SiteCoords::hash() ^ testId;
}


size_t
__gnu_cxx::hash<Coords>::operator()(const Coords &coords) const
{
  return coords.hash();
}
