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
    LowerBound(const Stats &stats)
      : stats(stats)
    {
    }

    double operator () (const predIndex index) const
    {
      return stats[index].ps.lb;
    }

    static const char code[];

  private:
    const Stats &stats;
  };


  class IncreaseScore
  {
  public:
    IncreaseScore(const Stats &stats)
      : stats(stats)
    {
    }

    double operator () (const predIndex index) const
    {
      return stats[index].ps.in;
    }

    static const char code[];

  private:
    const Stats &stats;
  };


  class FailScore
  {
  public:
    FailScore(const Stats &stats)
      : stats(stats)
    {
    }

    double operator () (const predIndex index) const
    {
      return stats[index].ps.fs;
    }

    static const char code[];

  private:
    const Stats &stats;
  };


  class TrueInFails
  {
  public:
    TrueInFails(const Stats &stats)
      : stats(stats)
    {
    }

    double operator () (const predIndex index) const
    {
      return stats[index].f;
    }

    static const char code[];

  private:
    const Stats &stats;
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
      : get(stats)
    {
    }

    bool operator () (const predIndex &a, const predIndex &b)
    {
      return get(a) > get(b);
    }

  private:
    const Get get;
  };


  template <class Get>
  class Descending
  {
  public:
    Descending(const Stats &stats)
      : get(stats)
    {
    }

    bool operator () (const predIndex &a, const predIndex &b)
    {
      return get(a) > get(b);
    }

  private:
    const Get get;
  };
}


#endif // !INCLUDE_sorts_h
