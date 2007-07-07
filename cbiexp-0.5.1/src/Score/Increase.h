#ifndef INCLUDE_Score_Increase_h
#define INCLUDE_Score_Increase_h

#include "../utils.h"


namespace Score
{
  struct Increase
  {
    double operator () (const pred_info &pred) const
    {
      return pred.ps.in;
    }

    static const char code[];
  };
}


#endif // !INCLUDE_Score_Increase_h
