#ifndef INCLUDE_symmetric_Candidates_h
#define INCLUDE_symmetric_Candidates_h

#include <ext/hash_map>
#include "Coords.h"

class Predicate;


struct Candidates : public __gnu_cxx::hash_map<Coords, Predicate *>
{
  void filter();
  iterator best();
};


#endif // !INCLUDE_symmetric_Candidates_h
