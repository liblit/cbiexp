#ifndef INCLUDE_corrective_ranking_RunSet_h
#define INCLUDE_corrective_ranking_RunSet_h

#include <iosfwd>
#include <list>
#include "Outcome.h"

class Predicate;


////////////////////////////////////////////////////////////////////////
//
//  A set of run numbers represented as an ordered list
//


class RunSet : public std::list<unsigned>
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
