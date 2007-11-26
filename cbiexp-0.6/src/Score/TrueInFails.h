#ifndef INCLUDE_Score_TrueInFails_h
#define INCLUDE_Score_TrueInFails_h

#include "../utils.h"


namespace Score
{
  struct TrueInFails
  {
    double operator () (const pred_info &pred) const
    {
      return pred.f;
    }

    static const char code[];
  };
}


#endif // !INCLUDE_Score_TrueInFails_h
