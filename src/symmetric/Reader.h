#ifndef INCLUDE_symmetric_Reader_h
#define INCLUDE_symmetric_Reader_h

#include "../ReportReader.h"

class PredCoords;


class Reader : public ReportReader
{
protected:
  void handleSite(const SiteCoords &, std::vector<unsigned> &);

private:
  void tallyPair(const SiteCoords &, int id, bool, bool) const;
  virtual void tallyPair(const PredCoords &, bool, const PredCoords &, bool) const = 0;
};


#endif // !INCLUDE_symmetric_Reader_h
