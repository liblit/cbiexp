#include "SiteCoords.h"


bool
operator<(const SiteCoords &a, const SiteCoords &b)
{
  return a.unitIndex < b.unitIndex
    || (a.unitIndex == b.unitIndex
	&& a.siteOffset < b.siteOffset);
}
