#ifndef INCLUDE_corrective_ranking_RunSet_h
#define INCLUDE_corrective_ranking_RunSet_h

#include <iosfwd>
#include <list>


////////////////////////////////////////////////////////////////////////
//
//  A set of run numbers represented as an ordered list
//


struct RunSet : public std::list<unsigned>
{
  double count;
  void load(std::istream &, char expected);

  void dilute(const RunSet &winner);
};


std::istream & operator>>(std::istream &, RunSet &);


#endif // !INCLUDE_corrective_ranking_RunSet_h
