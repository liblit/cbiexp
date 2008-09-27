#ifndef INCLUDE_symmetric_SuccessReader_h
#define INCLUDE_symmetric_SuccessReader_h

#include "Reader.h"

class Candidates;


class SuccessReader : public Reader
{
public:
  SuccessReader(const char*, const Candidates &);

private:
  const Candidates &candidates;

  void tallyPair(const PredCoords &, bool,
		 const PredCoords &, bool) const;
};


#endif // !INCLUDE_symmetric_SuccessReader_h
