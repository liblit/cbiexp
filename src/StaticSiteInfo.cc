#include "StaticSiteInfo.h"


StaticSiteInfo::StaticSiteInfo()
  : DynamicLibrary(DYNAMIC_LIBRARY_NAME("static-site-info")),
    unitCount(lookup<const unsigned>("NumUnits")),
    siteCount(lookup<const unsigned>("NumSites")),
    units(&lookup<const unit_t>("units")),
    sites(&lookup<const site_t>("sites"))
{
}
