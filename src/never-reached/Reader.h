#ifndef INCLUDE_Reader_h
#define INCLUDE_Reader_h

#include "../ReportReader.h"
#include "../StaticSiteInfo.h"

#include "Locations.h"


class Reader : public ReportReader
{
public:
  Reader();

  Locations neverReached;
  Locations neverFalse;
  Locations neverTrue;

  void dump() const;

protected:
  void handleSite(const SiteCoords &, const std::vector<unsigned> &);

private:
  const StaticSiteInfo staticSiteInfo;
};


#endif // !INCLUDE_Reader_h
