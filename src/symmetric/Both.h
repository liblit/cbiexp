#ifndef INCLUDE_symmetric_Both_h
#define INCLUDE_symmetric_Both_h

#include "Counts.h"
#include "Predicate.h"


class Both : public Counts
{
public:
  Both();

  Predicate first;
  Predicate second;
};


////////////////////////////////////////////////////////////////////////


inline
Both::Both()
  : first(*this, second),
    second(*this, first)
{
}


#endif // !INCLUDE_symmetric_Both_h
