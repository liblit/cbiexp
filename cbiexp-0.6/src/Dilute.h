#ifndef INCLUDE_Dilute_h
#define INCLUDE_Dilute_h


////////////////////////////////////////////////////////////////////////
//
//  Popularity dilution algorithms
//


namespace Dilute
{
  struct Circular
  {
    double operator () (double) const;
    static const char name[];
  };

  struct Linear
  {
    double operator () (double) const;
    static const char name[];
  };
}


////////////////////////////////////////////////////////////////////////


#define _USE_MATH_DEFINES
#include <cmath>


inline double
Dilute::Circular::operator () (double correlation) const
{
#ifdef M_PIl
# define DILUTE_PI M_PIl
#else
# define DILUTE_PI M_PI
#endif

  return std::abs(std::cos(DILUTE_PI / 2 * correlation));
}


inline double
Dilute::Linear::operator () (double correlation) const
{
  return 1 - correlation;
}


#endif // !INCLUDE_Dilute_h
