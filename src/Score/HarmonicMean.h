#ifndef INCLUDE_Score_HarmonicMean_h
#define INCLUDE_Score_HarmonicMean_h

#include <cassert>
#include <cmath>
#include "../classify_runs.h"
#include "../utils.h"


namespace Score
{
  struct HarmonicMean
  {
    double operator () (const pred_info &pred) const
    {
      assert(is_srun);
      assert(is_frun);
      return 2 / (1 / pred.ps.lb + sqrt(double(num_fruns)) / sqrt(double(pred.f)));
    }

    static const char code[];
  };
}


#endif // !INCLUDE_Score_HarmonicMean_h
