#include "SiteCoords.h"


bool
operator<(const SiteCoords &a, const SiteCoords &b)
{
  return a.siteIndex < b.siteIndex;
}
