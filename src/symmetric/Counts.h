#ifndef INCLUDE_symmetric_Counts_h
#define INCLUDE_symmetric_Counts_h

#include "RunSet.h"


class Counts
{
public:
  Counts();

  void trueInRun(Run *);
  void reclassify(const Run &);

  RunSet trueInFailures;
  unsigned trueInSuccesses;

protected:
  static double uncertainty(unsigned, unsigned);

private:
  // unimplemented and forbidden
  Counts(const Counts &);
  Counts &operator=(const Counts &);
};


////////////////////////////////////////////////////////////////////////


inline
Counts::Counts()
{
}


#endif // !INCLUDE_symmetric_Counts_h
