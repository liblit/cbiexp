#ifndef INCLUDE_IndexedPredInfo_h
#define INCLUDE_IndexedPredInfo_h

#include "utils.h"


////////////////////////////////////////////////////////////////////////
//
//  Predicate with rho index
//


struct IndexedPredInfo : public pred_info
{
  IndexedPredInfo(const pred_info &, unsigned);
  unsigned index;
};


inline
IndexedPredInfo::IndexedPredInfo(const pred_info &stats, unsigned index)
  : pred_info(stats),
    index(index)
{
}


#endif // !INCLUDE_IndexedPredInfo_h
