#ifndef INCLUDE_symmetric_Counts_h
#define INCLUDE_symmetric_Counts_h

#include <iosfwd>
#include "RunSet.h"


class Counts
{
public:
  Counts();

  bool reclassify(unsigned runId);
  void print(std::ostream &, const char tag[]) const;

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
  : trueInSuccesses(0)
{
}


#endif // !INCLUDE_symmetric_Counts_h
