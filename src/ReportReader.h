#ifndef INCLUDE_ReportReader_h
#define INCLUDE_ReportReader_h


class ReportReader
{
public:
  void read(unsigned runId) const;

  virtual void branchesSite(    unsigned unitIndex, unsigned siteIndex, unsigned, unsigned) const = 0;
  virtual void gObjectUnrefSite(unsigned unitIndex, unsigned siteIndex, unsigned, unsigned, unsigned, unsigned) const = 0;
  virtual void returnsSite(     unsigned unitIndex, unsigned siteIndex, unsigned, unsigned, unsigned) const = 0;
  virtual void scalarPairsSite( unsigned unitIndex, unsigned siteIndex, unsigned, unsigned, unsigned) const = 0;
};


#endif // !INCLUDE_ReportReader_h
