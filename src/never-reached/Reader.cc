#include <iostream>
#include "../StaticSiteInfo.h"
#include "Locations.h"
#include "Reader.h"


void
Reader::handleSite(const SiteCoords &coords, const std::vector<unsigned> &)
{
  const site_t &site(staticSiteInfo.site(coords));
  const Location reached(site);
  unreached.erase(reached);
}
