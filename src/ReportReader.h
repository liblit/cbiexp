#ifndef INCLUDE_ReportReader_h
#define INCLUDE_ReportReader_h

class SiteCoords;


class ReportReader
{
public:
  void read(unsigned runId) const;

  virtual void branchesSite(    const SiteCoords &, unsigned, unsigned) const = 0;
  virtual void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned) const = 0;
  virtual void returnsSite(     const SiteCoords &, unsigned, unsigned, unsigned) const = 0;
  virtual void scalarPairsSite( const SiteCoords &, unsigned, unsigned, unsigned) const = 0;
};


#endif // !INCLUDE_ReportReader_h
