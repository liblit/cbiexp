#ifndef INCLUDE_symmetric_Candidates_h
#define INCLUDE_symmetric_Candidates_h

#include <ext/hash_map>
#include "../PredCoords.h"

class Predicate;


struct Candidates : public __gnu_cxx::hash_map<PredCoords, Predicate *>
{
  void filter();
  const_iterator best() const;
};

std::ostream &operator<<(std::ostream &, const Candidates::value_type &);
std::ostream &operator<<(std::ostream &, const Candidates &);


#endif // !INCLUDE_symmetric_Candidates_h
