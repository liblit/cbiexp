#ifndef INCLUDE_corrective_ranking_RunSet_h
#define INCLUDE_corrective_ranking_RunSet_h

#include <iosfwd>
#include <vector>
#include "Outcome.h"

class Predicate;


////////////////////////////////////////////////////////////////////////
//
//  A set of runs represented as a membership bit vector
//


class RunSet : public std::vector<bool>
{
public:
  Outcome outcome;
  RunSet(Outcome);

  double count;
  void load(std::istream &);

  void dilute(const Predicate &, const RunSet &);

private:
  size_t intersectionSize(const RunSet &);
};


std::istream & operator>>(std::istream &, RunSet &);


#endif // !INCLUDE_corrective_ranking_RunSet_h
