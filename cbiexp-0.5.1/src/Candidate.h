#ifndef INCLUDE_Candidate_h
#define INCLUDE_Candidate_h

#include <iosfwd>
#include "IndexedPredInfo.h"


class Candidate : public IndexedPredInfo
{
public:
  Candidate(const IndexedPredInfo &, double popularity);
  const double firstImpression;
  double popularity;
};


bool operator < (const Candidate &, const Candidate &);

std::ostream & operator << (std::ostream &, const Candidate &);


////////////////////////////////////////////////////////////////////////


inline
Candidate::Candidate(const IndexedPredInfo &info, double initial)
  : IndexedPredInfo(info),
    firstImpression(initial),
    popularity(initial)
{
}


inline
bool operator < (const Candidate &a, const Candidate &b)
{
  return a.popularity < b.popularity;
}


#endif // !INCLUDE_Candidate_h
