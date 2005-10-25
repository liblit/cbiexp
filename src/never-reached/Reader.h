#ifndef INCLUDE_Reader_h
#define INCLUDE_Reader_h

#include "../ReportReader.h"

class Locations;
class StaticSiteInfo;



class Reader : public ReportReader
{
public:
  Reader(Locations &, const StaticSiteInfo &);

protected:
  void handleSite(const SiteCoords &, const std::vector<unsigned> &);

private:
  Locations &unreached;
  const StaticSiteInfo &staticSiteInfo;
};


////////////////////////////////////////////////////////////////////////


inline
Reader::Reader(Locations &locations, const StaticSiteInfo &staticSiteInfo)
  : unreached(locations),
    staticSiteInfo(staticSiteInfo)
{
}


#endif // !INCLUDE_Reader_h
