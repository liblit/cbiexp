#ifndef INCLUDE_symmetric_FailureReader_h
#define INCLUDE_symmetric_FailureReader_h

#include "Reader.h"

class Boths;
class Candidates;


class FailureReader : public Reader
{
public:
  FailureReader(Candidates &, Boths &, unsigned failureId);

private:
  Candidates &candidates;
  Boths &boths;
  const unsigned failureId;

  void tallyPair(const PredCoords &, unsigned,
		 const PredCoords &, unsigned) const;
};


#endif // !INCLUDE_symmetric_FailureReader_h
