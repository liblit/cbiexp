#include "../units.h"
#include "../utils.h"
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


ostream &
operator<<(ostream &out, const Coords &coords)
{
  const unit_t &unit = units[coords.unitIndex];

  return out << "<predicate unit=\"" << unit.signature
	     << "\" scheme=\"" << scheme_name(unit.scheme_code)
	     << "\" site=\"" << coords.siteOffset
	     << "\" test=\"" << coords.testId
	     << "\">";
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
