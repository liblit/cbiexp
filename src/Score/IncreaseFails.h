#ifndef INCLUDE_Score_IncreaseFails_h
#define INCLUDE_Score_IncreaseFails_h

#include "../utils.h"


namespace Score
{
  struct IncreaseFails
  {
    double operator () (const pred_info &pred) const
    {
      return pred.of - pred.f;
    }

    static const char code[];
  };
}


#endif // !INCLUDE_Score_IncreaseFails_h
