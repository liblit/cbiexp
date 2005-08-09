#ifndef INCLUDE_Sites_h
#define INCLUDE_Sites_h

#include "DynamicLibrary.h"
#include "site_t.h"


class Sites : private DynamicLibrary
{
public:
  Sites();

  const unsigned size;
  const site_t &operator[](unsigned) const;

private:
  const site_t * const infos;
};


////////////////////////////////////////////////////////////////////////


inline const site_t &
Sites::operator[](unsigned index) const
{
  return infos[index];
}


#endif // !INCLUDE_Sites_h
