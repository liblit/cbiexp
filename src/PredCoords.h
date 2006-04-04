#ifndef INCLUDE_PredCoords_h
#define INCLUDE_PredCoords_h

#include "SiteCoords.h"


struct PredCoords : public SiteCoords
{
  PredCoords();
  PredCoords(const SiteCoords &, unsigned pred);
  PredCoords(unsigned unit, unsigned site, unsigned pred);
  unsigned predicate;

  size_t hash() const;
};


bool operator<(const PredCoords &, const PredCoords &);
bool operator==(const PredCoords &, const PredCoords &);

std::istream &operator>>(std::istream &, PredCoords &);
std::ostream &operator<<(std::ostream &, const PredCoords &);


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
PredCoords::PredCoords(const SiteCoords &siteCoords, unsigned pred)
  : SiteCoords(siteCoords),
    predicate(pred)
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
