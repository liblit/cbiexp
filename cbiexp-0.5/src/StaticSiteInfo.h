#ifndef INCLUDE_StaticSiteInfo_h
#define INCLUDE_StaticSiteInfo_h

#include "DynamicLibrary.h"
#include "site_t.h"
#include "unit_t.h"

class SiteCoords;


class StaticSiteInfo : private DynamicLibrary
{
public:
  StaticSiteInfo();

  typedef const unit_t *UnitIterator;
  const unsigned unitCount;
  UnitIterator unitsBegin() const;
  UnitIterator unitsEnd() const;
  const unit_t &unit(unsigned) const;

  typedef const site_t *SiteIterator;
  const unsigned siteCount;
  SiteIterator sitesBegin() const;
  SiteIterator sitesEnd() const;
  const site_t &site(unsigned) const;
  const site_t &site(const SiteCoords &) const;

private:
  const unit_t * const units;
  const site_t * const sites;
};


////////////////////////////////////////////////////////////////////////


#include <cassert>
#include "SiteCoords.h"


inline StaticSiteInfo::UnitIterator
StaticSiteInfo::unitsBegin() const
{
  return units;
}


inline StaticSiteInfo::UnitIterator
StaticSiteInfo::unitsEnd() const
{
  return units + unitCount;
}


inline const unit_t &
StaticSiteInfo::unit(unsigned unitIndex) const
{
  assert(unitIndex < unitCount);
  return units[unitIndex];
}


inline StaticSiteInfo::SiteIterator
StaticSiteInfo::sitesBegin() const
{
  return sites;
}


inline StaticSiteInfo::SiteIterator
StaticSiteInfo::sitesEnd() const
{
  return sites + siteCount;
}


inline const site_t &
StaticSiteInfo::site(unsigned siteIndex) const
{
  assert(siteIndex < siteCount);
  return sites[siteIndex];
}


inline const site_t &
StaticSiteInfo::site(const SiteCoords &coords) const
{
  const unit_t &owner = unit(coords.unitIndex);
  assert(coords.siteOffset < owner.num_sites);
  return owner.sites[coords.siteOffset];
}


#endif // !INCLUDE_StaticSiteInfo_h
