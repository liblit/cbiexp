#include "StaticSiteInfo.h"


StaticSiteInfo::StaticSiteInfo()
  : DynamicLibrary(DYNAMIC_LIBRARY_NAME("static-site-info")),
    siteCount(lookup<const unsigned>("NumSites")),
    sites(&lookup<const site_t>("sites"))
{
}
