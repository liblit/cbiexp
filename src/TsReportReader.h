#ifndef INCLUDE_TSREPORT_READER_H
#define INCLUDE_TSREPORT_READER_H

#include "TimestampReport.h"
#include "timestamp.h"

class SiteCoords;

class TsReportReader
{
public:
  void read (unsigned runId, const char *when = TimestampReport::when.c_str());

  virtual void siteTs(const SiteCoords &, timestamp) = 0;
};

#endif // !INCLUDE_TSREPORT_READER_H
