#ifndef INCLUDE_ReportReader_h
#define INCLUDE_ReportReader_h

class SiteCoords;


class ReportReader
{
public:
  virtual ~ReportReader() { }
  void read(unsigned runId);

  virtual void branchesSite(    const SiteCoords &, unsigned, unsigned) = 0;
  virtual void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned) = 0;
  virtual void returnsSite(     const SiteCoords &, unsigned, unsigned, unsigned) = 0;
  virtual void scalarPairsSite( const SiteCoords &, unsigned, unsigned, unsigned) = 0;
};


#endif // !INCLUDE_ReportReader_h
