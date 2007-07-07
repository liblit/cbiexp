#ifndef INCLUDE_Score_HarmonicMeanLog_h
#define INCLUDE_Score_HarmonicMeanLog_h

#include <cassert>
#include <cmath>
#include "../classify_runs.h"
#include "../utils.h"


namespace Score
{
  struct HarmonicMeanLog
  {
    double operator () (const pred_info &pred) const
    {
      return 2 / (1 / pred.ps.lb + log(double(num_fruns)) / log(double(pred.f)));
    }

    static const char code[];
  };
}


#endif // !INCLUDE_Score_HarmonicMeanLog_h
