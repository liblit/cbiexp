#ifndef INCLUDE_Score_Fail_h
#define INCLUDE_Score_Fail_h

#include "../utils.h"


namespace Score
{
  struct Fail
  {
    double operator () (const pred_info &pred) const
    {
      return pred.ps.fs;
    }

    static const char code[];
  };
}


#endif // !INCLUDE_Score_Fail_h
