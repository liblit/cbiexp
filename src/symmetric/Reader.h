#ifndef INCLUDE_symmetric_Reader_h
#define INCLUDE_symmetric_Reader_h

#include "../ReportReader.h"

class PredCoords;


class Reader : public ReportReader
{
public:
  void branchesSite(    const SiteCoords &, unsigned, unsigned);
  void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned);
  void returnsSite(     const SiteCoords &, unsigned, unsigned, unsigned);
  void scalarPairsSite( const SiteCoords &, unsigned, unsigned, unsigned);

private:
  void tripleSite(const SiteCoords &, unsigned, unsigned, unsigned) const;
  void tallyPair(const SiteCoords &, int id, unsigned, unsigned) const;

  virtual void tallyPair(const PredCoords &, unsigned,
			 const PredCoords &, unsigned) const = 0;
};


#endif // !INCLUDE_symmetric_Reader_h
