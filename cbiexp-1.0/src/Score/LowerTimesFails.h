#ifndef INCLUDE_Score_LowerTimesFails_h
#define INCLUDE_Score_LowerTimesFails_h

#include "../utils.h"


namespace Score
{
  struct LowerTimesFails
  {
    double operator () (const pred_info &pred) const
    {
      return pred.f * pred.ps.lb;
    }

    static const char code[];
  };
}


#endif // !INCLUDE_Score_LowerTimesFails_h
