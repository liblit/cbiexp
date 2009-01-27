#ifndef INCLUDE_SiteCoords_h
#define INCLUDE_SiteCoords_h

#include <cstddef>
#include <iosfwd>


class SiteCoords
{
public:
  SiteCoords();
  SiteCoords(unsigned site);
  unsigned siteIndex;

  size_t hash() const;
};


bool operator<(const SiteCoords &, const SiteCoords &);
bool operator==(const SiteCoords &, const SiteCoords &);
bool operator!=(const SiteCoords &, const SiteCoords &);

std::istream &operator>>(std::istream &, SiteCoords &);
std::ostream &operator<<(std::ostream &, const SiteCoords &);


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


#include <iostream>


inline
SiteCoords::SiteCoords()
{
}


inline
SiteCoords::SiteCoords(unsigned site)
  : siteIndex(site)
{
}


inline size_t
SiteCoords::hash() const
{
  return siteIndex;
}


inline bool
operator==(const SiteCoords &a, const SiteCoords &b)
{
  return a.siteIndex == b.siteIndex;
}

inline bool
operator!=(const SiteCoords &a, const SiteCoords &b)
{
  return a.siteIndex != b.siteIndex;
}

inline std::istream &
operator>>(std::istream &in, SiteCoords &coords)
{
  return in >> coords.siteIndex;
}


inline std::ostream &
operator<<(std::ostream &out, const SiteCoords &coords)
{
  return out << coords.siteIndex;
}


inline size_t
__gnu_cxx::hash<SiteCoords>::operator()(const SiteCoords &coords) const
{
  return coords.hash();
}


#endif // !INCLUDE_SiteCoords_h
