#ifndef INCLUDE_ReportReader_h
#define INCLUDE_ReportReader_h

#include <vector>

class SiteCoords;


class ReportReader
{
public:
  virtual ~ReportReader() { }
  void read(unsigned runId);

protected:
  virtual void handleSite(const SiteCoords &, std::vector<unsigned> &) = 0;
};


#endif // !INCLUDE_ReportReader_h
