#ifndef INCLUDE_Units_h
#define INCLUDE_Units_h

#include "DynamicLibrary.h"
#include "unit_t.h"


class Units : private DynamicLibrary
{
public:
  Units();

  const unsigned size;
  const unit_t &operator[](unsigned) const;

  const int numBPreds;
  const int numRPreds;
  const int numSPreds;
  const int numGPreds;

private:
  const unit_t * const infos;
};


////////////////////////////////////////////////////////////////////////


inline const unit_t &
Units::operator[](unsigned index) const
{
  return infos[index];
}


#endif // !INCLUDE_Units_h
