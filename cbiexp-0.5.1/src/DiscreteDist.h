#ifndef INCLUDE_DiscreteDist_h
#define INCLUDE_DiscreteDist_h

#include <ext/hash_map>
#include "Counts.h"

using __gnu_cxx::hash_map;
typedef hash_map<count_tp, unsigned> DiscreteDist;

namespace __gnu_cxx
{
  template <class Key> struct hash;

  template <>
  struct hash<count_tp>
  {
    size_t operator()(const count_tp) const;
  };
}

inline size_t
__gnu_cxx::hash<count_tp>::operator()(const count_tp key) const
{
  return key;
}

#endif // !INCLUDE_DiscreteDist_h
