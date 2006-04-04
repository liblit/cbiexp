#include "PredCoords.h"

using namespace std;


bool
operator<(const PredCoords &a, const PredCoords &b)
{
  const SiteCoords &aSite(a);
  const SiteCoords &bSite(b);
  return aSite < bSite
    || (aSite == bSite && (a.predicate < b.predicate));
}

bool
operator==(const PredCoords &a, const PredCoords &b)
{
  const SiteCoords &aSite(a);
  const SiteCoords &bSite(b);
  return (aSite == bSite) && (a.predicate == b.predicate);
}


istream &
operator>>(istream &in, PredCoords &coords)
{
  return in >> reinterpret_cast<SiteCoords &>(coords)
	    >> coords.predicate;
}


ostream &
operator<<(ostream &out, const PredCoords &coords)
{
  return out << reinterpret_cast<const SiteCoords &>(coords)
	     << '\t' << coords.predicate;
}
