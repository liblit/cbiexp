#ifndef INCLUDE_ReportReader_h
#define INCLUDE_ReportReader_h

#include <vector>
#include "Counts.h"

class SiteCoords;

class ReportReader
{
public:
  virtual ~ReportReader() { }
  void read(unsigned runId);

protected:
  virtual void handleSite(const SiteCoords &, std::vector<count_tp> &) = 0;
};


#endif // !INCLUDE_ReportReader_h
