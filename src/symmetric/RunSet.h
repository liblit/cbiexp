#ifndef INCLUDE_symmetric_RunSet_h
#define INCLUDE_symmetric_RunSet_h

#include <ext/hash_set>

class Run;


namespace __gnu_cxx
{
  template <> struct hash<const Run *>
  {
    size_t operator()(const Run *addr) const
    {
      return reinterpret_cast<size_t>(addr);
    }
  };
}


class RunSet : public __gnu_cxx::hash_set<const Run *>
{
};


#endif // !INCLUDE_symmetric_RunSet_h
