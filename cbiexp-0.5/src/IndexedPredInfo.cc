#include <iostream>
#include "IndexedPredInfo.h"


std::ostream &
operator << (std::ostream &out, const IndexedPredInfo &info)
{
  return out << "<predictor index=\"" << info.index + 1 << "\"/>";
}
