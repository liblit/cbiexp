#ifndef INCLUDE_sorts_h
#define INCLUDE_sorts_h

#include <vector>
#include "utils.h"


typedef size_t predIndex;
typedef std::vector<pred_info> Stats;


////////////////////////////////////////////////////////////////////////


namespace Get
{
  class LowerBound
  {
  public:
    double operator () (const pred_info &pred) const
    {
      return pred.ps.lb;
    }

    static const char code[];
  };


  class IncreaseScore
  {
  public:
    double operator () (const pred_info &pred) const
    {
      return pred.ps.in;
    }

    static const char code[];
  };


  class FailScore
  {
  public:
    double operator () (const pred_info &pred) const
    {
      return pred.ps.fs;
    }

    static const char code[];
  };


  class TrueInFails
  {
  public:
    double operator () (const pred_info &pred) const
    {
      return pred.f;
    }

    static const char code[];
  };
}


////////////////////////////////////////////////////////////////////////


namespace Sort
{
  template <class Get>
  class Ascending
  {
  public:
    Ascending(const Stats &stats)
      : stats(stats)
    {
    }

    bool operator () (const predIndex &a, const predIndex &b)
    {
      return get(stats[a]) < get(stats[b]);
    }

  private:
    const Stats &stats;
    const Get get;
  };


  template <class Get>
  class Descending
  {
  public:
    Descending(const Stats &stats)
      : stats(stats)
    {
    }

    bool operator () (const predIndex &a, const predIndex &b)
    {
      return get(stats[a]) > get(stats[b]);
    }

  private:
    const Stats &stats;
    const Get get;
  };
}


#endif // !INCLUDE_sorts_h
