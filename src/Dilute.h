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


#include <cmath>


inline double
Dilute::Circular::operator () (double correlation) const
{
  return std::abs(std::cos(M_PIl / 2 * correlation));
}


inline double
Dilute::Linear::operator () (double correlation) const
{
  return 1 - correlation;
}


#endif // !INCLUDE_Dilute_h
