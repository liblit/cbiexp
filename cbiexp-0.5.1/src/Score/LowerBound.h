#ifndef INCLUDE_Score_LowerBound_h
#define INCLUDE_Score_LowerBound_h

#include "../utils.h"


namespace Score
{
  struct LowerBound
  {
    double operator () (const pred_info &pred) const
    {
      return pred.ps.lb;
    }

    static const char code[];
  };
}


#endif // !INCLUDE_Score_LowerBound_h
