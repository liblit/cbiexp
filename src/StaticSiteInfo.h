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

  typedef const site_t *SiteIterator;
  const unsigned siteCount;
  SiteIterator sitesBegin() const;
  SiteIterator sitesEnd() const;
  const site_t &site(unsigned) const;
  const site_t &site(const SiteCoords &) const;

private:
  const site_t * const sites;
};


////////////////////////////////////////////////////////////////////////


#include <cassert>
#include "SiteCoords.h"


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
  return sites[coords.siteIndex];
}


#endif // !INCLUDE_StaticSiteInfo_h
