#ifndef INCLUDE_symmetric_RunSet_h
#define INCLUDE_symmetric_RunSet_h

#include <boost/dynamic_bitset.hpp>


class RunSet : private boost::dynamic_bitset<>
{
public:
  static unsigned universeSize;

  RunSet();

  using boost::dynamic_bitset<>::any;
  using boost::dynamic_bitset<>::count;
  using boost::dynamic_bitset<>::find_first;
  using boost::dynamic_bitset<>::find_next;
  using boost::dynamic_bitset<>::none;
  using boost::dynamic_bitset<>::npos;
  using boost::dynamic_bitset<>::reset;
  using boost::dynamic_bitset<>::resize;
  using boost::dynamic_bitset<>::set;
  using boost::dynamic_bitset<>::test;
};


////////////////////////////////////////////////////////////////////////


inline
RunSet::RunSet()
  : boost::dynamic_bitset<>(universeSize, false)
{
}


#endif // !INCLUDE_symmetric_RunSet_h
