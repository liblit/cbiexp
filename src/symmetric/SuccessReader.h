#ifndef INCLUDE_symmetric_SuccessReader_h
#define INCLUDE_symmetric_SuccessReader_h

#include "Reader.h"

class Candidates;


class SuccessReader : public Reader
{
public:
  SuccessReader(const Candidates &);

private:
  const Candidates &candidates;

  void tallyPair(const PredCoords &, unsigned,
		 const PredCoords &, unsigned) const;
};


#endif // !INCLUDE_symmetric_SuccessReader_h
