#ifndef INCLUDE_PredCoords_h
#define INCLUDE_PredCoords_h

#include "SiteCoords.h"


struct PredCoords : public SiteCoords
{
  PredCoords();
  PredCoords(unsigned unit, unsigned site, unsigned pred);
  unsigned predicate;

  size_t hash() const;
};


namespace __gnu_cxx
{
  template <> struct hash<PredCoords>
  {
    size_t operator()(const PredCoords &) const;
  };
}


////////////////////////////////////////////////////////////////////////


inline
PredCoords::PredCoords()
{
}


inline
PredCoords::PredCoords(unsigned unit, unsigned site, unsigned pred)
  : SiteCoords(unit, site),
    predicate(pred)
{
}


inline size_t
PredCoords::hash() const
{
  return SiteCoords::hash() ^ predicate;
}


inline size_t
__gnu_cxx::hash<PredCoords>::operator()(const PredCoords &coords) const
{
  return coords.hash();
}


#endif // !INCLUDE_PredCoords_h
