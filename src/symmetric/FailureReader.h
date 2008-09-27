#ifndef INCLUDE_symmetric_FailureReader_h
#define INCLUDE_symmetric_FailureReader_h

#include "Reader.h"

class Boths;
class Candidates;


class FailureReader : public Reader
{
public:
  FailureReader(const char*, Candidates &, Boths &);
  void setid(unsigned);

private:
  Candidates &candidates;
  Boths &boths;
  unsigned failureId;

  void tallyPair(const PredCoords &, bool,
		 const PredCoords &, bool) const;
};


#endif // !INCLUDE_symmetric_FailureReader_h
