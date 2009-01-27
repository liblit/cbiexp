#include <cassert>
#include <iostream>
#include "../StaticSiteInfo.h"
#include "Locations.h"
#include "Reader.h"


Reader::Reader(const char* filename):
  ReportReader(filename)
{

  for (StaticSiteInfo::SiteIterator site = staticSiteInfo.sitesBegin();
       site != staticSiteInfo.sitesEnd(); ++site)
    {
      const Location location(*site);
      neverReached.insert(location);

      if (site->scheme_code == 'B')
	{
	  neverFalse.insert(location);
	  neverTrue.insert(location);
	}
    }
}


void
Reader::handleSite(const SiteCoords &coords, std::vector<count_tp> &counts)
{
  const site_t &site(staticSiteInfo.site(coords));
  const Location reached(site);

  neverReached.erase(reached);

  if (site.scheme_code == 'B')
    {
      assert(counts.size() == 2);
      if (counts[0]) neverFalse.erase(reached);
      if (counts[1]) neverTrue.erase(reached);
    }
}


void
Reader::dump() const
{
  neverReached.dump("reached");
  neverFalse.dump("false");
  neverTrue.dump("true");
}
