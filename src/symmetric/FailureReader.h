#ifndef INCLUDE_symmetric_FailureReader_h
#define INCLUDE_symmetric_FailureReader_h

#include "Reader.h"

class Boths;
class Candidates;


class FailureReader : public Reader
{
public:
  FailureReader(Candidates &, Boths &, unsigned runId);

private:
  Candidates &candidates;
  Boths &boths;
  const unsigned runId;

  void tallyPair(const PredCoords &, unsigned,
		 const PredCoords &, unsigned) const;
};


#endif // !INCLUDE_symmetric_FailureReader_h
