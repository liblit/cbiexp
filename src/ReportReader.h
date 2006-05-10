#ifndef INCLUDE_ReportReader_h
#define INCLUDE_ReportReader_h

#include <vector>
#include "Counts.h"
#include "arguments.h"

class SiteCoords;

class ReportReader
{
public:
  virtual ~ReportReader() { }
  void read(unsigned runId);

  static const struct argp argp;

protected:
  virtual void handleSite(const SiteCoords &, std::vector<count_tp> &) = 0;

private:
  static bool selected(const SiteCoords &);
};


#endif // !INCLUDE_ReportReader_h
