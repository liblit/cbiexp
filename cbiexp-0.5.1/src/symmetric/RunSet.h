#ifndef INCLUDE_symmetric_RunSet_h
#define INCLUDE_symmetric_RunSet_h

#include <boost/dynamic_bitset.hpp>


class RunSet : public boost::dynamic_bitset<>
{
public:
  static unsigned universeSize;

  RunSet();
};


////////////////////////////////////////////////////////////////////////


inline
RunSet::RunSet()
  : boost::dynamic_bitset<>(universeSize, false)
{
}


#endif // !INCLUDE_symmetric_RunSet_h
