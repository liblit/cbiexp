#ifndef INCLUDE_symmetric_Reader_h
#define INCLUDE_symmetric_Reader_h

#include "../ReportReader.h"

class Candidates;
class Predicate;
class Run;


class Reader : public ReportReader
{
public:
  Reader(Boths &, Candidates &, Run *failure);

  void branchesSite(    const SiteCoords &, unsigned, unsigned);
  void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned);
  void returnsSite(     const SiteCoords &, unsigned, unsigned, unsigned);
  void scalarPairsSite( const SiteCoords &, unsigned, unsigned, unsigned);

private:
  Boths &boths;
  Candidates &candidates;
  Run * const failure;

  void tripleSite(const SiteCoords &, unsigned, unsigned, unsigned);
  void tallyPair(const SiteCoords &, int id, unsigned, unsigned);
};


#endif // !INCLUDE_symmetric_Reader_h
