#ifndef INCLUDE_SiteCoords_h
#define INCLUDE_SiteCoords_h

#include <cstddef>


struct SiteCoords
{
  SiteCoords();
  SiteCoords(unsigned unit, unsigned site);
  unsigned unitIndex;
  unsigned siteOffset;

  size_t hash() const;
};


bool operator==(const SiteCoords &, const SiteCoords &);


namespace __gnu_cxx
{
  template <class Key> struct hash;

  template <>
  struct hash<SiteCoords>
  {
    size_t operator()(const SiteCoords &) const;
  };
}


////////////////////////////////////////////////////////////////////////


inline
SiteCoords::SiteCoords()
{
}


inline
SiteCoords::SiteCoords(unsigned unit, unsigned site)
  : unitIndex(unit),
    siteOffset(site)
{
}


inline size_t
SiteCoords::hash() const
{
  return unitIndex ^ siteOffset;
}


inline bool operator==(const SiteCoords &a, const SiteCoords &b)
{
  return a.unitIndex == b.unitIndex
    && a.siteOffset == b.siteOffset;
}


inline size_t
__gnu_cxx::hash<SiteCoords>::operator()(const SiteCoords &coords) const
{
  return coords.hash();
}


#endif // !INCLUDE_SiteCoords_h
